#include <std_msgs/Empty.h>
#include <boost/thread.hpp>

#include <ros/ros.h>

#include <string>

#include "chatterbox/ChatIn.h"
#include "chatterbox/ChatOut.h"

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

// 플랫폼 위치 파라미터
typedef struct _PosState {
	double x;	// unit: m
	double y;	// unit: m
	double z;	// unit: rad
} PosState;

// 속도 파라미터
typedef struct _VelParam {
	double vel;		// unit: m/s
	double acc;		// unit: m/s^2
	double vmax;	// unit: m/s
} VelParam;

// 플랫폼 속도 파라미터
typedef struct _VelParams {
	VelParam x;
	VelParam y;
	VelParam z;
} VelParams;

// 조그 정보
typedef struct _JogInfo {
	uint32_t front;	// push: 1, release: 0
	uint32_t rear;	// push: 1, release: 0
	uint32_t left;	// push: 1, release: 0
	uint32_t right;	// push: 1, release: 0
	uint32_t cw;	// push: 1, release: 0
	uint32_t ccw;	// push: 1, release: 0
} JogInfo;

// 정지 정보
typedef struct _StopInfo {
	uint32_t stop;
} StopInfo;

// 상태입력 정보
typedef struct _StateInfo {
	int32_t period;
} StateInfo;

// 센서 정보
typedef struct _SensorState {
	double front;		// m
	double rear;		// m
	double right_front;	// m
	double right_rear;	// m
} SensorState;

// 플랫폼 정보
typedef struct _PlatformState {
	int32_t state;	// ready=0, stop=1, run=2, error=3
    int32_t mode;
} PlatformState;

// 총 정보
#pragma pack(push, 1)
typedef struct _TotalState {
	SensorState sensor;
	PosState pos;
	PlatformState platform;
} TotalState;
#pragma pack(pop)

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
	InitPlatform=15
} Command;

SensorState sensorStateOut;
PosState posStateOut;
PlatformState platformStateOut;
TotalState totalStateOut;

StateInfo totalState;
StateInfo platformState;
StateInfo sensorState;
StateInfo posState;

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

chatterbox::ChatOut chatOut_;
void chatOutCallBack(const chatterbox::ChatOut chatOut) {
    chatOut_ = chatOut;
}

void fThread(int* thread_rate, ros::Publisher *chatIn_pub) {
    ros::NodeHandlePtr node = boost::make_shared<ros::NodeHandle>();
    ros::Rate rate(*thread_rate);

    chatterbox::ChatIn chatIn;

    struct sockaddr_in addr, client_addr;
    #define BUF_SIZE	1024
    char buffer[BUF_SIZE];
    uint32_t command;
    Command cmd = None;
    int len, addr_len, recv_len;

    ros::Time now;

    while (clientOpen && ros::ok()) {
        // 소켓 열기
        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            perror("socket ");
            return;
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

        // 소켓 설정 등록
        if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
            perror("bind ");
            return;
        }
        printf("bind registerd\n");

        // 시그널 핸들러 등록
        signal(SIGINT, sigint_handler);

        // 시그널 핸들러 등록
        readWriteInfinite = 1;
        signal(SIGPIPE, sigpipe_handler);

        // struct timeval timeout;
        // timeout.tv_sec = 1;
        // timeout.tv_usec = 0;
        
        // if (setsockopt (sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0) {
        //     perror("setsockopt failed ");
        // }

        // 연결 요청 대기
        if (listen(sock, 5) < 0) {
            perror("listen ");
            return;
        }

        addr_len = sizeof(client_addr);

        printf("waiting for client..\n");

        // 연결 수락
        if ((client_sock = accept(sock, (struct sockaddr *)&client_addr, (socklen_t*)&addr_len)) < 0) {
            perror("accept ");
            goto PROGRAM_END;
            return;
        } else {
            printf("clinet ip : %s\n", inet_ntoa(client_addr.sin_addr));	
            printf("client socket open\n");	
        }
        printf("client_sock: %d\n", client_sock);

        memset(buffer, '\0', sizeof(buffer));
        
        struct timeval time_now{};
        gettimeofday(&time_now, nullptr);
        time_t ts_now, ts_total, ts_platform, ts_sensor, ts_position, ts_dummy;
        ts_dummy = ts_total = ts_platform = ts_sensor = ts_position = ts_now = (time_now.tv_sec * 1000) + (time_now.tv_usec / 1000);

        char *u8_ptr;
        int byte_size;

        memset((char*)&totalState, '\0', sizeof(totalState));
        memset((char*)&sensorState, '\0', sizeof(sensorState));
        memset((char*)&platformState, '\0', sizeof(platformState));
        memset((char*)&posState, '\0', sizeof(posState));

        memset((char*)&sensorStateOut, '\0', sizeof(sensorStateOut));
        memset((char*)&posStateOut, '\0', sizeof(posStateOut));
        memset((char*)&platformStateOut, '\0', sizeof(platformStateOut));
        memset((char*)&totalStateOut, '\0', sizeof(totalStateOut));

        printf("read/write start\n");
        // 통신
        while (readWriteInfinite && ros::ok()) {
            gettimeofday(&time_now, nullptr);
            ts_now = (time_now.tv_sec * 1000) + (time_now.tv_usec / 1000);
            #define DATA_LENGTH_BYTE	4
            // 버퍼개수 읽기(MSG_PEEK는 버퍼확인용)
            recv_len = recv(client_sock, buffer, DATA_LENGTH_BYTE, MSG_PEEK|MSG_DONTWAIT);
            if (recv_len >= DATA_LENGTH_BYTE) {
                // printf("recv: %d\n", recv_len);

                int trail_len;
                // 뒤따르는 자료개수 확인
                memcpy(&trail_len, buffer, DATA_LENGTH_BYTE);
                // printf("trail_len: %x, %d\n", trail_len, trail_len);

                // 뒤따르는 버퍼 읽기(MSG_PEEK는 버퍼확인용)
                recv_len = recv(client_sock, buffer, DATA_LENGTH_BYTE+trail_len, MSG_PEEK|MSG_DONTWAIT);
                if (recv_len >= DATA_LENGTH_BYTE+trail_len) {
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
                        case Stop:
                            StopInfo stopInfoIn;
                            printf("Receviced StopInfoIn, trail byte(%d)\n", trail_len-COMMAND_SIZE);
                            memcpy(&stopInfoIn, buffer+COMMAND_SIZE, sizeof(stopInfoIn));
                            printf("stop: %d\n", stopInfoIn.stop);
                            platformStateOut.mode = Stop;

                            now = ros::Time::now();
                            chatIn.header.stamp = now;

                            chatIn.command = platformStateOut.mode;

                            chatIn_pub->publish(chatIn);
                        break;
                        case Estop:
                            printf("Receviced Estop, trail byte(%d)\n", trail_len-COMMAND_SIZE);
                            platformStateOut.mode = Estop;

                            now = ros::Time::now();
                            chatIn.header.stamp = now;

                            chatIn.command = platformStateOut.mode;

                            chatIn_pub->publish(chatIn);
                        break;
                        case SetPosMode:
                            printf("Receviced SetPosMode, trail byte(%d)\n", trail_len-COMMAND_SIZE);
                            platformStateOut.mode = SetPosMode;

                            now = ros::Time::now();
                            chatIn.header.stamp = now;

                            chatIn.command = platformStateOut.mode;

                            // chatIn_pub->publish(chatIn);
                        break;
                        case SetJogMode:
                            printf("Receviced SetJogMode, trail byte(%d)\n", trail_len-COMMAND_SIZE);
                            platformStateOut.mode = SetJogMode;

                            now = ros::Time::now();
                            chatIn.header.stamp = now;

                            chatIn.command = platformStateOut.mode;

                            // chatIn_pub->publish(chatIn);
                        break;
                        case SetControlMode:
                            static uint32_t controlMode, controlModePre;

                            printf("Receviced SetControlMode, trail byte(%d)\n", trail_len-COMMAND_SIZE);
                            memcpy((char*)&controlMode, buffer+COMMAND_SIZE, sizeof(controlMode));
                            printf("mode: %d\n", controlMode);

                            #if ACK_OK
                            if (controlMode == 0) {
                                byte_size = sizeof(command) + sizeof(controlMode);
                                u8_ptr = (char*)&byte_size;
                                memcpy(buffer, u8_ptr, sizeof(byte_size));
                                u8_ptr = (char*)&command;
                                memcpy(buffer+sizeof(byte_size), u8_ptr, sizeof(command));
                                u8_ptr = (char*)&controlModePre;
                                memcpy(buffer+sizeof(byte_size)+sizeof(command), u8_ptr, sizeof(controlModePre));
                                buffer[ACK_IDX] = ACK_RES;
                                send(client_sock, buffer, sizeof(byte_size)+sizeof(command)+sizeof(controlModePre), 0);
                            } else {
                                controlModePre = controlMode;
                            }
                            #endif
                        break;
                        case SetJogParam:
                            static VelParams jogVelParamsIn, jogVelParamsInPre;
                            memcpy(&jogVelParamsIn, buffer+COMMAND_SIZE, sizeof(jogVelParamsIn));
                            printf("Receviced SetJogParam, trail byte(%d)\n", trail_len-COMMAND_SIZE);
                            printf("x.v: %.3lf, x.a: %.3lf, x.vm: %.3lf, y.v: %.3lf, y.a: %.3lf, y.vm: %.3lf, z.v: %.3lf, z.a: %.3lf, z.vm: %.3lf\n",
                                jogVelParamsIn.x.vel, jogVelParamsIn.x.acc, jogVelParamsIn.x.vmax,
                                jogVelParamsIn.y.vel, jogVelParamsIn.y.acc, jogVelParamsIn.y.vmax,
                                jogVelParamsIn.z.vel, jogVelParamsIn.z.acc, jogVelParamsIn.z.vmax);

                            #if ACK_OK
                            static VelParams *jvpi = &jogVelParamsIn;
                            if (jvpi->x.vel == 0.0 && jvpi->x.acc == 0.0 && jvpi->x.vmax == 0.0 &&
                                jvpi->y.vel == 0.0 && jvpi->y.acc == 0.0 && jvpi->y.vmax == 0.0 &&
                                jvpi->z.vel == 0.0 && jvpi->z.acc == 0.0 && jvpi->z.vmax == 0.0) {
                                byte_size = sizeof(command) + sizeof(jogVelParamsIn);
                                u8_ptr = (char*)&byte_size;
                                memcpy(buffer, u8_ptr, sizeof(byte_size));
                                u8_ptr = (char*)&command;
                                memcpy(buffer+sizeof(byte_size), u8_ptr, sizeof(command));
                                u8_ptr = (char*)&jogVelParamsInPre;
                                memcpy(buffer+sizeof(byte_size)+sizeof(command), u8_ptr, sizeof(jogVelParamsInPre));
                                buffer[ACK_IDX] = ACK_RES;
                                send(client_sock, buffer, sizeof(byte_size)+sizeof(command)+sizeof(jogVelParamsInPre), 0);
                            } else {
                                memcpy((char*)&jogVelParamsInPre, (char*)&jogVelParamsIn, sizeof(jogVelParamsIn));
                            }
                            #endif
                        break;
                        case SetPosParam:
                            static VelParams posVelParamsIn, posVelParamsInPre;
                            memcpy(&posVelParamsIn, buffer+COMMAND_SIZE, sizeof(posVelParamsIn));
                            printf("Receviced SetPosParam, trail byte(%d)\n", trail_len-COMMAND_SIZE);
                            printf("x.v: %.3lf, x.a: %.3lf, x.vm: %.3lf, y.v: %.3lf, y.a: %.3lf, y.vm: %.3lf, z.v: %.3lf, z.a: %.3lf, z.vm: %.3lf\n",
                                posVelParamsIn.x.vel, posVelParamsIn.x.acc, posVelParamsIn.x.vmax,
                                posVelParamsIn.y.vel, posVelParamsIn.y.acc, posVelParamsIn.y.vmax,
                                posVelParamsIn.z.vel, posVelParamsIn.z.acc, posVelParamsIn.z.vmax);

                            #if ACK_OK
                            static VelParams *pvpi = &posVelParamsIn;
                            if (pvpi->x.vel == 0.0 && pvpi->x.acc == 0.0 && pvpi->x.vmax == 0.0 &&
                                pvpi->y.vel == 0.0 && pvpi->y.acc == 0.0 && pvpi->y.vmax == 0.0 &&
                                pvpi->z.vel == 0.0 && pvpi->z.acc == 0.0 && pvpi->z.vmax == 0.0) {
                                byte_size = sizeof(command) + sizeof(posVelParamsIn);
                                u8_ptr = (char*)&byte_size;
                                memcpy(buffer, u8_ptr, sizeof(byte_size));
                                u8_ptr = (char*)&command;
                                memcpy(buffer+sizeof(byte_size), u8_ptr, sizeof(command));
                                u8_ptr = (char*)&posVelParamsInPre;
                                memcpy(buffer+sizeof(byte_size)+sizeof(command), u8_ptr, sizeof(posVelParamsInPre));
                                buffer[ACK_IDX] = ACK_RES;
                                send(client_sock, buffer, sizeof(byte_size)+sizeof(command)+sizeof(posVelParamsInPre), 0);
                            } else {
                                memcpy((char*)&posVelParamsInPre, (char*)&posVelParamsIn, sizeof(posVelParamsIn));
                            }
                            #endif
                        break;
                        case SetJogState:
                            JogInfo jogInfoIn;
                            memcpy(&jogInfoIn, buffer+COMMAND_SIZE, sizeof(jogInfoIn));
                            printf("Receviced SetJogState, trail byte(%d)\n", trail_len-COMMAND_SIZE);
                            printf("f: %d, r: %d, l: %d, r: %d, cw: %d, ccw: %d\n",
                                jogInfoIn.front, jogInfoIn.rear, jogInfoIn.left, jogInfoIn.right, jogInfoIn.cw, jogInfoIn.ccw);

                            now = ros::Time::now();
                            chatIn.header.stamp = now;

                            chatIn.command = platformStateOut.mode;
                            
                            chatIn.jogParam.jogInfo.front = jogInfoIn.front;
                            chatIn.jogParam.jogInfo.rear = jogInfoIn.rear;
                            chatIn.jogParam.jogInfo.left = jogInfoIn.left;
                            chatIn.jogParam.jogInfo.right = jogInfoIn.right;
                            chatIn.jogParam.jogInfo.cw = jogInfoIn.cw;
                            chatIn.jogParam.jogInfo.ccw = jogInfoIn.ccw;

                            chatIn.jogParam.velParams.x.acc = jogVelParamsIn.x.acc;
                            chatIn.jogParam.velParams.y.acc = jogVelParamsIn.y.acc;
                            chatIn.jogParam.velParams.z.acc = jogVelParamsIn.z.acc;
                            chatIn.jogParam.velParams.x.vel = jogVelParamsIn.x.vel;
                            chatIn.jogParam.velParams.y.vel = jogVelParamsIn.y.vel;
                            chatIn.jogParam.velParams.z.vel = jogVelParamsIn.z.vel;
                            chatIn.jogParam.velParams.x.vmax =jogVelParamsIn.x.vmax;
                            chatIn.jogParam.velParams.y.vmax =jogVelParamsIn.y.vmax;
                            chatIn.jogParam.velParams.z.vmax =jogVelParamsIn.z.vmax;

                            chatIn_pub->publish(chatIn);
                        break;
                        case SetPos:
                            static PosState posStateIn, posStateInPre;
                            memcpy(&posStateIn, buffer+COMMAND_SIZE, sizeof(posStateIn));
                            printf("Receviced SetPos, trail byte(%d)\n", trail_len-COMMAND_SIZE);
                            printf("pos.x: %.3lf, pos.y: %.3lf, pos.theta: %.3lf\n", posStateIn.x, posStateIn.y, posStateIn.z);

                            #if ACK_OK
                            static PosState *psi = &posStateIn;
                            if (psi->x == 0.0 && psi->y == 0.0 && psi->z == 0.0) {
                                byte_size = sizeof(command) + sizeof(posStateIn);
                                u8_ptr = (char*)&byte_size;
                                memcpy(buffer, u8_ptr, sizeof(byte_size));
                                u8_ptr = (char*)&command;
                                memcpy(buffer+sizeof(byte_size), u8_ptr, sizeof(command));
                                u8_ptr = (char*)&posStateInPre;
                                memcpy(buffer+sizeof(byte_size)+sizeof(command), u8_ptr, sizeof(posStateInPre));
                                buffer[ACK_IDX] = ACK_RES;
                                send(client_sock, buffer, sizeof(byte_size)+sizeof(command)+sizeof(posStateInPre), 0);
                            } else {
                                memcpy((char*)&posStateInPre, (char*)&posStateIn, sizeof(posStateIn));
                            }
                            #endif
                        break;
                        case StartPosControl:
                            printf("Receviced StartPosControl, trail byte(%d)\n", trail_len-COMMAND_SIZE);

                            now = ros::Time::now();
                            chatIn.header.stamp = now;

                            chatIn.command = platformStateOut.mode;

                            chatIn.posParam.posState.x = posStateIn.x;
                            chatIn.posParam.posState.y = posStateIn.y;
                            chatIn.posParam.posState.z = posStateIn.z;

                            chatIn.posParam.velParams.x.acc = posVelParamsIn.x.acc;
                            chatIn.posParam.velParams.y.acc = posVelParamsIn.y.acc;
                            chatIn.posParam.velParams.z.acc = posVelParamsIn.z.acc;
                            chatIn.posParam.velParams.x.vel = posVelParamsIn.x.vel;
                            chatIn.posParam.velParams.y.vel = posVelParamsIn.y.vel;
                            chatIn.posParam.velParams.z.vel = posVelParamsIn.z.vel;
                            chatIn.posParam.velParams.x.vmax = posVelParamsIn.x.vmax;
                            chatIn.posParam.velParams.y.vmax = posVelParamsIn.y.vmax;
                            chatIn.posParam.velParams.z.vmax = posVelParamsIn.z.vmax;

                            chatIn_pub->publish(chatIn);
                        break;
                        case GetTotal:
                            printf("Receviced GetTotal, trail byte(%d)\n", trail_len-COMMAND_SIZE);
                            memcpy(&totalState, buffer+COMMAND_SIZE, sizeof(StateInfo));
                            printf("period: %d\n", totalState.period);

                            if (totalState.period == 0) {
                            // 전체 송신할 바이트 계산
                            #if 0
                            totalStateOut.sensor.front += 0.1;
                            totalStateOut.sensor.rear += 0.2;
                            totalStateOut.sensor.right_front += 0.3;
                            totalStateOut.sensor.right_rear += 0.4;
                            totalStateOut.pos.x += 0.5;
                            totalStateOut.pos.y += 0.6;
                            totalStateOut.pos.z += 0.7;
                            totalStateOut.platform.state += 8;
                            totalStateOut.platform.mode += 9;
                            #else
                            platformStateOut.state = chatOut_.platformState.state; 
                            platformStateOut.mode = chatOut_.platformState.mode;

                            sensorStateOut.front = chatOut_.sensorState.front;
                            sensorStateOut.rear = chatOut_.sensorState.rear;
                            sensorStateOut.right_front = chatOut_.sensorState.right_front;
                            sensorStateOut.right_rear = chatOut_.sensorState.right_rear;
                            
                            posStateOut.x = chatOut_.posState.x;
                            posStateOut.y = chatOut_.posState.y;
                            posStateOut.z = chatOut_.posState.z;
                            #endif

                            #if 0
                            byte_size = sizeof(command) + sizeof(totalStateOut);
                            u8_ptr = (char*)&byte_size;
                            send(client_sock, u8_ptr, sizeof(byte_size), 0);
                            u8_ptr = (char*)&command;
                            send(client_sock, u8_ptr, sizeof(command), 0);
                            u8_ptr = (char*)&totalStateOut;
                            send(client_sock, u8_ptr, sizeof(totalStateOut), 0);
                            #else							
                            byte_size = sizeof(command) + sizeof(totalStateOut);
                            u8_ptr = (char*)&byte_size;
                            memcpy(buffer, u8_ptr, sizeof(byte_size));
                            u8_ptr = (char*)&command;
                            memcpy(buffer+sizeof(byte_size), u8_ptr, sizeof(command));
                            u8_ptr = (char*)&totalStateOut;
                            memcpy(buffer+sizeof(byte_size)+sizeof(command), u8_ptr, sizeof(totalStateOut));
                            buffer[ACK_IDX] = ACK_ONE;
                            send(client_sock, buffer, sizeof(byte_size)+sizeof(command)+sizeof(totalStateOut), 0);
                            #endif
                            } else {
                            }
                        break;
                        case GetPlatform:
                            printf("Receviced GetPlatform, trail byte(%d)\n", trail_len-COMMAND_SIZE);
                            memcpy(&platformState, buffer+COMMAND_SIZE, sizeof(StateInfo));
                            printf("period: %d\n", platformState.period);

                            if (platformState.period == 0) {
                            // 전체 송신할 바이트 계산
                            #if 0
                            platformStateOut.state += 2;
                            platformStateOut.mode += 3;
                            #else
                            platformStateOut.state = chatOut_.platformState.state; 
                            platformStateOut.mode = chatOut_.platformState.mode;
                            #endif

                            #if 0
                            byte_size = sizeof(command) + sizeof(platformStateOut);
                            u8_ptr = (char*)&byte_size;
                            send(client_sock, u8_ptr, sizeof(byte_size), 0);
                            u8_ptr = (char*)&command;
                            send(client_sock, u8_ptr, sizeof(command), 0);
                            u8_ptr = (char*)&platformStateOut;
                            send(client_sock, u8_ptr, sizeof(platformStateOut), 0);
                            #else							
                            byte_size = sizeof(command) + sizeof(platformStateOut);
                            u8_ptr = (char*)&byte_size;
                            memcpy(buffer, u8_ptr, sizeof(byte_size));
                            u8_ptr = (char*)&command;
                            memcpy(buffer+sizeof(byte_size), u8_ptr, sizeof(command));
                            u8_ptr = (char*)&platformStateOut;
                            memcpy(buffer+sizeof(byte_size)+sizeof(command), u8_ptr, sizeof(platformStateOut));
                            buffer[ACK_IDX] = ACK_ONE;
                            send(client_sock, buffer, sizeof(byte_size)+sizeof(command)+sizeof(platformStateOut), 0);
                            #endif
                            } else {
                            }
                        break;
                        case GetSensor:
                            printf("Receviced GetSensor, trail byte(%d)\n", trail_len-COMMAND_SIZE);
                            memcpy(&sensorState, buffer+COMMAND_SIZE, sizeof(StateInfo));
                            printf("period: %d\n", sensorState.period);

                            if (sensorState.period == 0) {
                            // 전체 송신할 바이트 계산
                            #if 0
                            sensorStateOut.front += 0.1;
                            sensorStateOut.rear += 0.2;
                            sensorStateOut.right_front += 0.3;
                            sensorStateOut.right_rear += 0.4;
                            #else
                            sensorStateOut.front = chatOut_.sensorState.front;
                            sensorStateOut.rear = chatOut_.sensorState.rear;
                            sensorStateOut.right_front = chatOut_.sensorState.right_front;
                            sensorStateOut.right_rear = chatOut_.sensorState.right_rear;
                            #endif

                            #if 0
                            byte_size = sizeof(command) + sizeof(sensorStateOut);
                            u8_ptr = (char*)&byte_size;
                            send(client_sock, u8_ptr, sizeof(byte_size), 0);
                            u8_ptr = (char*)&command;
                            send(client_sock, u8_ptr, sizeof(command), 0);
                            u8_ptr = (char*)&sensorStateOut;
                            send(client_sock, u8_ptr, sizeof(sensorStateOut), 0);
                            #else							
                            byte_size = sizeof(command) + sizeof(sensorStateOut);
                            u8_ptr = (char*)&byte_size;
                            memcpy(buffer, u8_ptr, sizeof(byte_size));
                            u8_ptr = (char*)&command;
                            memcpy(buffer+sizeof(byte_size), u8_ptr, sizeof(command));
                            u8_ptr = (char*)&sensorStateOut;
                            memcpy(buffer+sizeof(byte_size)+sizeof(command), u8_ptr, sizeof(sensorStateOut));
                            buffer[ACK_IDX] = ACK_ONE;
                            send(client_sock, buffer, sizeof(byte_size)+sizeof(command)+sizeof(sensorStateOut), 0);
                            #endif
                            } else {
                            }
                        break;
                        case GetPos:
                            printf("Receviced GetPos, trail byte(%d)\n", trail_len-COMMAND_SIZE);
                            memcpy(&posState, buffer+COMMAND_SIZE, sizeof(StateInfo));
                            printf("period: %d\n", posState.period);

                            if (posState.period == 0) {
                            // 전체 송신할 바이트 계산
                            #if 0
                            posStateOut.x += 0.1;
                            posStateOut.y += 0.2;
                            posStateOut.z += 0.3;
                            #else
                            posStateOut.x = chatOut_.posState.x;
                            posStateOut.y = chatOut_.posState.y;
                            posStateOut.z = chatOut_.posState.z;
                            #endif

                            #if 0
                            byte_size = sizeof(command) + sizeof(posStateOut);
                            u8_ptr = (char*)&byte_size;
                            send(client_sock, u8_ptr, sizeof(byte_size), 0);
                            u8_ptr = (char*)&command;
                            send(client_sock, u8_ptr, sizeof(command), 0);
                            u8_ptr = (char*)&posStateOut;
                            send(client_sock, u8_ptr, sizeof(posStateOut), 0);
                            #else
                            byte_size = sizeof(command) + sizeof(posStateOut);
                            u8_ptr = (char*)&byte_size;
                            memcpy(buffer, u8_ptr, sizeof(byte_size));
                            u8_ptr = (char*)&command;
                            memcpy(buffer+sizeof(byte_size), u8_ptr, sizeof(command));
                            u8_ptr = (char*)&posStateOut;
                            memcpy(buffer+sizeof(byte_size)+sizeof(command), u8_ptr, sizeof(posStateOut));
                            buffer[ACK_IDX] = ACK_ONE;
                            send(client_sock, buffer, sizeof(byte_size)+sizeof(command)+sizeof(posStateOut), 0);
                            #endif
                            } else {
                            }
                        break;
                        case InitPlatform:
                            printf("Receviced InitPlatform, trail byte(%d)\n", trail_len-COMMAND_SIZE);
                            platformStateOut.mode = InitPlatform;

                            now = ros::Time::now();
                            chatIn.header.stamp = now;

                            chatIn.command = platformStateOut.mode;

                            chatIn_pub->publish(chatIn);
                        break;
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
                printf("%d error\n", recv_len);
                readWriteInfinite = 0;
                continue;
            } else {
            }

            #if 0
            static PosState posStateTest = {0.0, 10.0, 0.0};
            #if 1
            #define SEND_PERIOD_MS	10
            if (SEND_PERIOD_MS <= (ts_now-ts_dummy)) {
                ts_dummy = ts_now;

                posStateTest.x += 0.1;
                posStateTest.y -= 0.1;
                posStateTest.z = posStateTest.x + 1;
                if (posStateTest.x > 10.0) {
                    posStateTest.x = 0.0;
                    posStateTest.y = 10.0;
                }

                command = SetPos;
                // 전체 송신할 바이트 계산
                byte_size = sizeof(command) + sizeof(posStateTest);
                u8_ptr = (char*)&byte_size;
                send(client_sock, u8_ptr, sizeof(byte_size), 0);

                u8_ptr = (char*)&command;
                send(client_sock, u8_ptr, sizeof(command), 0);

                // printf("count: %d\tdec: %d\toffset: %d\n", t.count, t.dec, t.offset);
                u8_ptr = (char*)&posStateTest;
                send(client_sock, u8_ptr, sizeof(posStateTest), 0);

                // printf("timestamp now : %ld\n", ts_now);
            }
            #else				
            posStateTest.x += 0.1;
            posStateTest.y -= 0.1;
            posStateTest.z = posStateTest.x + 1;
            if (posStateTest.x > 10.0) {
                posStateTest.x = 0.0;
                posStateTest.y = 10.0;
            }

            command = SetPos;
            byte_size = sizeof(command) + sizeof(posStateTest);
            u8_ptr = (char*)&byte_size;
            memcpy(buffer, u8_ptr, sizeof(byte_size));
            u8_ptr = (char*)&command;
            memcpy(buffer+sizeof(byte_size), u8_ptr, sizeof(command));
            u8_ptr = (char*)&posStateTest;
            memcpy(buffer+sizeof(byte_size)+sizeof(command), u8_ptr, sizeof(posStateTest));
            send(client_sock, buffer, sizeof(byte_size)+sizeof(command)+sizeof(posStateTest), 0);

            usleep(10000);

            // printf("usleep now : %ld\n", ts_now);
            #endif
            #endif

            // printf("milli: %ld %ld\n", msecs_time, sizeof(msecs_time));
            
            if (totalState.period !=0 && totalState.period <= (ts_now-ts_total)) {
                ts_total = ts_now;

                // 전체 송신할 바이트 계산
                totalStateOut.sensor.front += 0.1;
                totalStateOut.sensor.rear += 0.2;
                totalStateOut.sensor.right_front += 0.3;
                totalStateOut.sensor.right_rear += 0.4;
                totalStateOut.pos.x += 0.5;
                totalStateOut.pos.y += 0.6;
                totalStateOut.pos.z += 0.7;
                totalStateOut.platform.state += 8;

                command = GetTotal;

                byte_size = sizeof(command) + sizeof(totalStateOut);
                u8_ptr = (char*)&byte_size;
                memcpy(buffer, u8_ptr, sizeof(byte_size));
                u8_ptr = (char*)&command;
                memcpy(buffer+sizeof(byte_size), u8_ptr, sizeof(command));
                u8_ptr = (char*)&totalStateOut;
                memcpy(buffer+sizeof(byte_size)+sizeof(command), u8_ptr, sizeof(totalStateOut));
                buffer[ACK_IDX] = ACK_STR;
                send(client_sock, buffer, sizeof(byte_size)+sizeof(command)+sizeof(totalStateOut), 0);
            }
            
            if (platformState.period !=0 && platformState.period <= (ts_now-ts_platform)) {
                ts_platform = ts_now;

                // 전체 송신할 바이트 계산
                platformStateOut.state += 2;

                command = GetPlatform;

                byte_size = sizeof(command) + sizeof(platformStateOut);
                u8_ptr = (char*)&byte_size;
                memcpy(buffer, u8_ptr, sizeof(byte_size));
                u8_ptr = (char*)&command;
                memcpy(buffer+sizeof(byte_size), u8_ptr, sizeof(command));
                u8_ptr = (char*)&platformStateOut;
                memcpy(buffer+sizeof(byte_size)+sizeof(command), u8_ptr, sizeof(platformStateOut));
                buffer[ACK_IDX] = ACK_STR;
                send(client_sock, buffer, sizeof(byte_size)+sizeof(command)+sizeof(platformStateOut), 0);
            }
            
            if (sensorState.period !=0 && sensorState.period <= (ts_now-ts_sensor)) {
                ts_sensor = ts_now;

                // 전체 송신할 바이트 계산
                sensorStateOut.front += 0.1;
                sensorStateOut.rear += 0.2;
                sensorStateOut.right_front += 0.3;
                sensorStateOut.right_rear += 0.4;

                command = GetSensor;

                byte_size = sizeof(command) + sizeof(sensorStateOut);
                u8_ptr = (char*)&byte_size;
                memcpy(buffer, u8_ptr, sizeof(byte_size));
                u8_ptr = (char*)&command;
                memcpy(buffer+sizeof(byte_size), u8_ptr, sizeof(command));
                u8_ptr = (char*)&sensorStateOut;
                memcpy(buffer+sizeof(byte_size)+sizeof(command), u8_ptr, sizeof(sensorStateOut));
                buffer[ACK_IDX] = ACK_STR;
                send(client_sock, buffer, sizeof(byte_size)+sizeof(command)+sizeof(sensorStateOut), 0);
            }
            
            if (posState.period !=0 && posState.period <= (ts_now-ts_position)) {
                ts_position = ts_now;

                // 전체 송신할 바이트 계산
                posStateOut.x += 0.1;
                posStateOut.y += 0.2;
                posStateOut.z += 0.3;

                command = GetPos;

                byte_size = sizeof(command) + sizeof(posStateOut);
                u8_ptr = (char*)&byte_size;
                memcpy(buffer, u8_ptr, sizeof(byte_size));
                u8_ptr = (char*)&command;
                memcpy(buffer+sizeof(byte_size), u8_ptr, sizeof(command));
                u8_ptr = (char*)&posStateOut;
                memcpy(buffer+sizeof(byte_size)+sizeof(command), u8_ptr, sizeof(posStateOut));
                buffer[ACK_IDX] = ACK_STR;
                send(client_sock, buffer, sizeof(byte_size)+sizeof(command)+sizeof(posStateOut), 0);
            }
        }
        printf("read/write end\n");
        
        close(client_sock);
        printf("client socket closed\n");

        close(sock);
        // 소켓을 정상적으로 너무 빨리 닫고 재 열기할 경우
        // 해당 포트가 이미 사용중이라는 표시가 나타날 수 있음
        usleep(100000);
        printf("socket closed\n");
    }

    PROGRAM_END:
    printf("program end\n");
}

int main(int argc, char** argv)
{
    ros::init(argc, argv, "chatterbox");
    ros::NodeHandle nh("~");
    
    ros::Subscriber test1_sub = nh.subscribe("/chatterbox/chatOut_topic", 100, chatOutCallBack);
    
    ros::Publisher chatIn_pub = nh.advertise<chatterbox::ChatIn>("chatIn_topic", 100);

    int thread_rate = 200;
    boost::thread hThread(fThread, &thread_rate, &chatIn_pub);

    ros::Rate main_rate(1000);

    double time_cur = ros::Time::now().toSec();
    double time_pre = time_cur;
    double time_diff;

    while(ros::ok())
    {
        time_cur = ros::Time::now().toSec();
        time_diff = time_cur - time_pre;
#define PERIOD  0.1
        if ( time_diff > PERIOD) {
            time_pre = time_cur;
        }

        ros::spinOnce();

        main_rate.sleep();
    }

    hThread.join();
}
