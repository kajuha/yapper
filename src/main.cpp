#include <std_msgs/Empty.h>
#include <boost/thread.hpp>

#include <ros/ros.h>

#include <string>

#include <yapper/YapIn.h>

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <chrono>
#include <iostream>
#include <sys/time.h>
#include <ctime>

#define ON                  1
#define OFF                 0
#define TCP_DUMMY_SEND      OFF

// 조그 정보
typedef struct _JogInfo {
	uint32_t x_p;	// push: 1, release: 0
	uint32_t x_n;	// push: 1, release: 0
	uint32_t y_p;	// push: 1, release: 0
	uint32_t y_n;	// push: 1, release: 0
	uint32_t z_p;	// push: 1, release: 0
	uint32_t z_n;	// push: 1, release: 0
} JogInfo;

// 명령어 종류
typedef enum _Command {
	None=0,
	Stop=1, Estop,
	SetPosMode=3, SetJogMode, SetControlMode,
	SetJogParam=6, SetPosParam,
	SetJogState=8,
	SetPos=9,
	StartPosControl=10,
	GetTotal=11, GetPlatform, GetSensor, GetPos,
	InitPlatform=15,
    GetWhisper=16,
    SetSensor=17
} Command;

int sock, client_sock;

// 시그널 핸들러(클라이언트 TCP 해제)
#include <signal.h>
int readWriteInfinite = 1;
int clientOpen = 1;

void sigpipe_handler(int sig) {
	// signal(SIGPIPE, sigpipe_handler);
	printf("received SIGPIPE: %d \n", sig);
	readWriteInfinite = 0;
}

void sigint_handler(int sig) {
	// signal(SIGINT, sigint_handler);
	printf("received SIGINT: %d \n", sig);
	close(sock);
	clientOpen = 0;
}

void fThread(int* thread_rate, ros::Publisher *yapIn_pub) {
    ros::NodeHandlePtr node = boost::make_shared<ros::NodeHandle>();
    ros::Rate rate(*thread_rate);

    yapper::YapIn yapIn;

    struct sockaddr_in addr, client_addr;
    #define BUF_SIZE	1024
    char buffer[BUF_SIZE];
    uint32_t command;
    Command cmd = None;
    int len, addr_len, recv_len;
    int ret;

    ros::Time now;

    printf("clientOpen while start (%d line)\n", __LINE__);
    while (clientOpen && ros::ok()) {
        printf("clientOpen while started (%d line)\n", __LINE__);
        // 소켓 열기
        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            // perror("socket open error, ");
            printf("socket open error\n");
            // return;
            continue;
        }
        printf("socket open\n");

        // 소켓 설정(TCP, 포트)
        memset(&addr, 0x00, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
        
        int tcp_port;  // sin_port == uint16_t
        ros::param::get("~tcp_port", tcp_port);
        printf("tcp_port: %d \n", tcp_port);
        addr.sin_port = htons(tcp_port);

        // time_wait 제거하기
        int option = 1;
        setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

        // 소켓 설정 등록
        if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
            // perror("bind error, ");
            printf("bind error\n");
            // return;
            usleep(500000);
            continue;
        }
        printf("bind registerd\n");

        // 시그널 핸들러 등록
        // signal(SIGINT, sigint_handler);

        // 시그널 핸들러 등록
        readWriteInfinite = 1;
        signal(SIGPIPE, sigpipe_handler);

        // 리슨을 타임아웃으로 설정하고 싶을 경우
        struct timeval timeout;
#define TCP_TIMEOUT_SEC 3
        timeout.tv_sec = TCP_TIMEOUT_SEC;
        timeout.tv_usec = 0;
        
        if (setsockopt (sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0) {
            // perror("setsockopt failed, ");
            printf("setsockopt failed\n");
        }

        // 연결 요청 대기
        if (listen(sock, 5) < 0) {
            // perror("listen error, ");
            printf("listen error\n");
            // return;
            usleep(500000);
            continue;
        } else {
            printf("listen\n");
        }

        addr_len = sizeof(client_addr);

        printf("waiting for client..\n");

        printf("socket : %d\n", sock);
        // 연결 수락
        if ((client_sock = accept(sock, (struct sockaddr *)&client_addr, (socklen_t*)&addr_len)) < 0) {
            // perror("accept error, ");
            printf("accept error\n");
            // goto PROGRAM_END;
            // return;
            printf("socket : %d\n", sock);
            printf("client_sock: %d\n", client_sock);
            close(client_sock);
            close(sock);
            usleep(500000);
            continue;
        } else {
            printf("clinet ip : %s\n", inet_ntoa(client_addr.sin_addr));	
            printf("client accept\n");	
        }
        printf("socket : %d\n", sock);
        printf("client_sock: %d\n", client_sock);

        memset(buffer, '\0', sizeof(buffer));
        
        struct timeval time_now{};
        gettimeofday(&time_now, nullptr);
        time_t ts_now, ts_total, ts_platform, ts_sensor, ts_position, ts_whisper, ts_dummy, ts_total_once;
        ts_total_once = ts_dummy = ts_total = ts_platform = ts_sensor = ts_position = ts_whisper = ts_now = (time_now.tv_sec * 1000) + (time_now.tv_usec / 1000);

        char *u8_ptr;
        int byte_size;

        printf("readWriteInfinite while start (%d line)\n", __LINE__);
        // 통신
        while (readWriteInfinite && ros::ok()) {
            // printf("readWriteInfinite while started (%d line)\n", __LINE__);
            gettimeofday(&time_now, nullptr);
            ts_now = (time_now.tv_sec * 1000) + (time_now.tv_usec / 1000);
            #define DATA_LENGTH_BYTE	4
            // 버퍼개수 읽기(MSG_PEEK는 버퍼확인용)
            recv_len = recv(client_sock, buffer, DATA_LENGTH_BYTE, MSG_PEEK|MSG_DONTWAIT);
            if (recv_len >= DATA_LENGTH_BYTE) {
                #if 0
                printf("1st recv_len: %d, ", DATA_LENGTH_BYTE);
                for (int cnt=0; cnt<DATA_LENGTH_BYTE; cnt++) {
                    printf("%02x ", buffer[cnt]&0xff);
                }
                printf("\n");
                #endif

                int trail_len;
                // 뒤따르는 자료개수 확인
                memcpy(&trail_len, buffer, DATA_LENGTH_BYTE);
                // printf("trail_len: %x, %d\n", trail_len, trail_len);

                // 뒤따르는 버퍼 읽기(MSG_PEEK는 버퍼확인용)
                recv_len = recv(client_sock, buffer, DATA_LENGTH_BYTE+trail_len, MSG_PEEK|MSG_DONTWAIT);
                if (recv_len >= DATA_LENGTH_BYTE+trail_len) {
                    #if 0
                    printf("2st recv_len: %d, ", trail_len);
                    for (int i=0; i<trail_len; i++) {
                        printf("%02x ", *(buffer+DATA_LENGTH_BYTE+i)&0xff);
                    }
                    printf("\n");
                    #endif

                    // 응답 프레임 송신
                    #if 1
                    char ack_buffer[BUF_SIZE];
                    // 버퍼개수 버리기(4바이트)
                    recv(client_sock, buffer, DATA_LENGTH_BYTE, 0);
                    memcpy(ack_buffer, buffer, DATA_LENGTH_BYTE);
                    // 뒤따르는 버퍼 읽기
                    recv(client_sock, buffer, trail_len, 0);
                    memcpy(ack_buffer+DATA_LENGTH_BYTE, buffer, trail_len);

                    // ACK 프레임으로 만들기
                    #define ACK_OK		1
                    #define ACK_IDX		6
                    #define ACK_NONE	0x00
                    #define ACK_ACK		0x01
                    #define ACK_RES		0x02
                    #define ACK_ONE		0x04
                    #define ACK_STR		0x08
                    #if ACK_OK
                    ack_buffer[ACK_IDX] = ACK_ACK;
                    #else
                    ack_buffer[ACK_IDX] = ACK_NONE;
                    #endif
                    send(client_sock, ack_buffer, DATA_LENGTH_BYTE+trail_len, 0);
                    #else
                    // 버퍼개수 버리기(4바이트)
                    recv(client_sock, buffer, DATA_LENGTH_BYTE, 0);
                    // 뒤따르는 버퍼 읽기
                    recv(client_sock, buffer, trail_len, 0);
                    #endif

                    // 명령어 바이트 읽기
                    #define COMMAND_SIZE	4
                    memcpy((char*)&command, buffer, COMMAND_SIZE);
                    cmd = (Command)command;
                    printf("command: %5d\tts: %ld\n", command, ts_now);
    
                    switch (cmd) {
                        case None:
                            printf("None command\n");
                        break;
                        case SetJogState:
                            JogInfo jogInfoIn;
                            memcpy(&jogInfoIn, buffer+COMMAND_SIZE, sizeof(jogInfoIn));
                            printf("Receviced SetJogState, trail byte(%d)\n", trail_len-COMMAND_SIZE);
                            printf("x_p: %d, x_n: %d, y_p: %d, y_n: %d, z_p: %d, z_n: %d\n",
                                jogInfoIn.x_p, jogInfoIn.x_n, jogInfoIn.y_p, jogInfoIn.y_n, jogInfoIn.z_p, jogInfoIn.z_n);

                            now = ros::Time::now();
                            yapIn.header.stamp = now;
                            
                            yapIn.jogInfo.x_p = jogInfoIn.x_p;
                            yapIn.jogInfo.x_n = jogInfoIn.x_n;
                            yapIn.jogInfo.y_p = jogInfoIn.y_p;
                            yapIn.jogInfo.y_n = jogInfoIn.y_n;
                            yapIn.jogInfo.z_p = jogInfoIn.z_p;
                            yapIn.jogInfo.z_n = jogInfoIn.z_n;

                            yapIn_pub->publish(yapIn);
                        break;
                        case Stop:
                        case Estop:
                        case SetPosMode:
                        case SetJogMode:
                        case SetControlMode:
                        case SetJogParam:
                        case SetPosParam:
                        case SetPos:
                        case StartPosControl:
                        case GetTotal:
                        case GetPlatform:
                        case GetSensor:
                        case GetPos:
                        case InitPlatform:
                        case GetWhisper:
                        case SetSensor:
                        default:
                            printf("unknown command\n");
                    }
                } else {
                }
            } else if (recv_len == 0) {
                // 참고자료
                // recv를 호출했을때 0이 리턴되거나 send를 호출했을때 -1로 에러리턴이면 상대편 연결이 종료된 것

                // 커넥션중일 때는 MSG_PEEK|MSG_DONTWAIT에서 아무것도 송신안할 경우 -1이 발생하였음
                // 커넥션을 강제해제했을 경우 0이 발생하였음
                // MSG_DONTWIT를 추가하지 않았을 경우에는 무한대기가 되었음
                // 우선 이케이스에는 0을 커넥션 종료로 처리함
                printf("client disconnect error, recv: %d \n", recv_len);
                readWriteInfinite = 0;
                continue;
            } else {
            }

            // printf("milli: %ld %ld\n", msecs_time, sizeof(msecs_time));
        }
        printf("tcp read/write end\n");
        
        ret = close(client_sock);
        printf("client socket closed, ret: %d\n", ret);

        ret = close(sock);
        printf("socket closed, ret: %d\n", ret);
        // 소켓을 정상적으로 너무 빨리 닫고 재 열기할 경우
        // 해당 포트가 이미 사용중이라는 표시가 나타날 수 있음
        usleep(500000);
    }

    printf("program end\n");
}

int main(int argc, char** argv)
{
    ros::init(argc, argv, "yapper");
    ros::NodeHandle nh("~");
    
    ros::Publisher yapIn_pub = nh.advertise<yapper::YapIn>("yapIn_topic", 100);

    int thread_rate = 200;
    boost::thread hThread(fThread, &thread_rate, &yapIn_pub);

    ros::Rate main_rate(1000);

    double ts_cur = ros::Time::now().toSec();
    double ts_pre = ts_cur;
    double ts_diff;

    while(ros::ok())
    {
        ts_cur = ros::Time::now().toSec();
        ts_diff = ts_cur - ts_pre;
#define PERIOD  0.1
        if ( ts_diff > PERIOD) {
            ts_pre = ts_cur;
        }

        ros::spinOnce();

        main_rate.sleep();
    }

    printf("hThread join\n");
    hThread.join();
    printf("hThread joined\n");
}
