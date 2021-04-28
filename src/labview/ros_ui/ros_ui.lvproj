<?xml version='1.0' encoding='UTF-8'?>
<Project Type="Project" LVVersion="18008000">
	<Item Name="My Computer" Type="My Computer">
		<Property Name="server.app.propertiesEnabled" Type="Bool">true</Property>
		<Property Name="server.control.propertiesEnabled" Type="Bool">true</Property>
		<Property Name="server.tcp.enabled" Type="Bool">false</Property>
		<Property Name="server.tcp.port" Type="Int">0</Property>
		<Property Name="server.tcp.serviceName" Type="Str">My Computer/</Property>
		<Property Name="server.tcp.serviceName.default" Type="Str">My Computer/</Property>
		<Property Name="server.vi.callsEnabled" Type="Bool">true</Property>
		<Property Name="server.vi.propertiesEnabled" Type="Bool">true</Property>
		<Property Name="specify.custom.address" Type="Bool">false</Property>
		<Item Name="mainVI" Type="Folder" URL="../mainVI">
			<Property Name="NI.DISK" Type="Bool">true</Property>
		</Item>
		<Item Name="subVI" Type="Folder" URL="../subVI">
			<Property Name="NI.DISK" Type="Bool">true</Property>
		</Item>
		<Item Name="Dependencies" Type="Dependencies">
			<Item Name="user.lib" Type="Folder">
				<Item Name="AddToOldMasters.vi" Type="VI" URL="/&lt;userlib&gt;/ROS for LabVIEW Software/ROS/Code/SubVIs/AddToOldMasters.vi"/>
				<Item Name="AddToQueue.vi" Type="VI" URL="/&lt;userlib&gt;/ROS for LabVIEW Software/ROS/Code/SubVIs/AddToQueue.vi"/>
				<Item Name="CheckBuildFolder.vi" Type="VI" URL="/&lt;userlib&gt;/ROS for LabVIEW Software/ROS/Code/SubVIs/CheckBuildFolder.vi"/>
				<Item Name="CheckForErrors.vi" Type="VI" URL="/&lt;userlib&gt;/ROS for LabVIEW Software/ROS/Code/ROS_Tools/CheckForErrors.vi"/>
				<Item Name="CheckForNewTopic.vi" Type="VI" URL="/&lt;userlib&gt;/ROS for LabVIEW Software/ROS/Code/SubVIs/CheckForNewTopic.vi"/>
				<Item Name="CheckMasterConnection.vi" Type="VI" URL="/&lt;userlib&gt;/ROS for LabVIEW Software/ROS/Code/SubVIs/CheckMasterConnection.vi"/>
				<Item Name="CheckNodeName.vi" Type="VI" URL="/&lt;userlib&gt;/ROS for LabVIEW Software/ROS/Code/SubVIs/CheckNodeName.vi"/>
				<Item Name="CleanupString.vi" Type="VI" URL="/&lt;userlib&gt;/ROS for LabVIEW Software/ROS/Code/SubVIs/CleanupString.vi"/>
				<Item Name="ConvertVItoHTML.vi" Type="VI" URL="/&lt;userlib&gt;/ROS for LabVIEW Software/ROS/Code/SubVIs/ConvertVItoHTML.vi"/>
				<Item Name="ExtractLogTokens.vi" Type="VI" URL="/&lt;userlib&gt;/ROS for LabVIEW Software/ROS/Code/SubVIs/LogFileCodes/ExtractLogTokens.vi"/>
				<Item Name="GetAllPaths.vi" Type="VI" URL="/&lt;userlib&gt;/ROS for LabVIEW Software/ROS/Code/Console/GetAllPaths.vi"/>
				<Item Name="GetErrCodes.vi" Type="VI" URL="/&lt;userlib&gt;/ROS for LabVIEW Software/ROS/Code/SubVIs/GetErrCodes.vi"/>
				<Item Name="GetFIFOQueue.vi" Type="VI" URL="/&lt;userlib&gt;/ROS for LabVIEW Software/ROS/Code/SubVIs/GetFIFOQueue.vi"/>
				<Item Name="GetIPAddress.vi" Type="VI" URL="/&lt;userlib&gt;/ROS for LabVIEW Software/Devices/ROSRIO/GetIPAddress.vi"/>
				<Item Name="GetLogFilePath.vi" Type="VI" URL="/&lt;userlib&gt;/ROS for LabVIEW Software/ROS/Code/SubVIs/LogFileCodes/GetLogFilePath.vi"/>
				<Item Name="GetMyIPAddress.vi" Type="VI" URL="/&lt;userlib&gt;/ROS for LabVIEW Software/ROS/Code/SubVIs/GetMyIPAddress.vi"/>
				<Item Name="GetQueueValue.vi" Type="VI" URL="/&lt;userlib&gt;/ROS for LabVIEW Software/ROS/Code/SubVIs/GetQueueValue.vi"/>
				<Item Name="GetROSfromTopic.vi" Type="VI" URL="/&lt;userlib&gt;/ROS for LabVIEW Software/ROS/Code/SubVIs/GetROSfromTopic.vi"/>
				<Item Name="GetServerNamesAndContents.vi" Type="VI" URL="/&lt;userlib&gt;/ROS for LabVIEW Software/ROS/Code/SubVIs/GetServerNamesAndContents.vi"/>
				<Item Name="GetServerVIName.vi" Type="VI" URL="/&lt;userlib&gt;/ROS for LabVIEW Software/ROS/Code/SubVIs/GetServerVIName.vi"/>
				<Item Name="GetTagsForPreferences.vi" Type="VI" URL="/&lt;userlib&gt;/ROS for LabVIEW Software/ROS/Code/SubVIs/GetTagsForPreferences.vi"/>
				<Item Name="GetTopicNode_etc.vi" Type="VI" URL="/&lt;userlib&gt;/ROS for LabVIEW Software/ROS/Code/SubVIs/GetTopicNode_etc.vi"/>
				<Item Name="GetURI&amp;Port.vi" Type="VI" URL="/&lt;userlib&gt;/ROS for LabVIEW Software/ROS/Code/SubVIs/GetURI&amp;Port.vi"/>
				<Item Name="GetWriteQueue.vi" Type="VI" URL="/&lt;userlib&gt;/ROS for LabVIEW Software/ROS/Code/SubVIs/GetWriteQueue.vi"/>
				<Item Name="NewIP.vi" Type="VI" URL="/&lt;userlib&gt;/ROS for LabVIEW Software/ROS/Code/SubVIs/NewIP.vi"/>
				<Item Name="NodifyROS.vi" Type="VI" URL="/&lt;userlib&gt;/ROS for LabVIEW Software/ROS/Code/SubVIs/NodifyROS.vi"/>
				<Item Name="OpenNodes.vi" Type="VI" URL="/&lt;userlib&gt;/ROS for LabVIEW Software/ROS/Code/ROS_Tools/OpenNodes.vi"/>
				<Item Name="ROSTerminal.vi" Type="VI" URL="/&lt;userlib&gt;/ROS for LabVIEW Software/ROS/Code/ROS_Tools/ROSTerminal.vi"/>
				<Item Name="ROSToQueue.vi" Type="VI" URL="/&lt;userlib&gt;/ROS for LabVIEW Software/ROS/Code/Console/Servers/ServerSubs/ROSToQueue.vi"/>
				<Item Name="ROS_Topic_Close.vi" Type="VI" URL="/&lt;userlib&gt;/ROS for LabVIEW Software/ROS/Code/ROS_Topic_Close.vi"/>
				<Item Name="ROS_Topic_Close_Primitive.vi" Type="VI" URL="/&lt;userlib&gt;/ROS for LabVIEW Software/ROS/Code/ROS_Topic_Close_Primitive.vi"/>
				<Item Name="ROS_Topic_Init.vi" Type="VI" URL="/&lt;userlib&gt;/ROS for LabVIEW Software/ROS/Code/ROS_Topic_Init.vi"/>
				<Item Name="ROS_Topic_Read.vi" Type="VI" URL="/&lt;userlib&gt;/ROS for LabVIEW Software/ROS/Code/ROS_Topic_Read.vi"/>
				<Item Name="ROS_Topic_Read_Primative.vi" Type="VI" URL="/&lt;userlib&gt;/ROS for LabVIEW Software/ROS/Code/ROS_Topic_Read_Primative.vi"/>
				<Item Name="ROS_Topic_Write.vi" Type="VI" URL="/&lt;userlib&gt;/ROS for LabVIEW Software/ROS/Code/ROS_Topic_Write.vi"/>
				<Item Name="ROS_Topic_Write_Primitive.vi" Type="VI" URL="/&lt;userlib&gt;/ROS for LabVIEW Software/ROS/Code/ROS_Topic_Write_Primitive.vi"/>
				<Item Name="RedefineMasterIP.vi" Type="VI" URL="/&lt;userlib&gt;/ROS for LabVIEW Software/ROS/Code/SubVIs/RedefineMasterIP.vi"/>
				<Item Name="SaveReadPrefs.vi" Type="VI" URL="/&lt;userlib&gt;/ROS for LabVIEW Software/ROS/Code/SubVIs/SaveReadPrefs.vi"/>
				<Item Name="StartSeparateServer.vi" Type="VI" URL="/&lt;userlib&gt;/ROS for LabVIEW Software/ROS/Code/SubVIs/StartSeparateServer.vi"/>
				<Item Name="TopicDef.ctl" Type="VI" URL="/&lt;userlib&gt;/ROS for LabVIEW Software/ROS/Code/Console/Servers/ServerSubs/TopicDef.ctl"/>
				<Item Name="WaitForStartup.vi" Type="VI" URL="/&lt;userlib&gt;/ROS for LabVIEW Software/ROS/Code/SubVIs/WaitForStartup.vi"/>
				<Item Name="_ROSControl.ctl" Type="VI" URL="/&lt;userlib&gt;/ROS for LabVIEW Software/ROS/Code/SubVIs/_ROSControl.ctl"/>
				<Item Name="_ROSDefinition.vi" Type="VI" URL="/&lt;userlib&gt;/ROS for LabVIEW Software/ROS/Code/_ROSDefinition.vi"/>
				<Item Name="_ROSDirection.ctl" Type="VI" URL="/&lt;userlib&gt;/ROS for LabVIEW Software/ROS/Code/SubVIs/_ROSDirection.ctl"/>
				<Item Name="add_Header.vi" Type="VI" URL="/&lt;userlib&gt;/ROS for LabVIEW Software/ROS/MessageBuilding/std_msgs/add_Header.vi"/>
				<Item Name="add_bool.vi" Type="VI" URL="/&lt;userlib&gt;/ROS for LabVIEW Software/ROS/MessageBuilding/std_msgs/add_bool.vi"/>
				<Item Name="add_string.vi" Type="VI" URL="/&lt;userlib&gt;/ROS for LabVIEW Software/ROS/MessageBuilding/std_msgs/add_string.vi"/>
				<Item Name="add_time.vi" Type="VI" URL="/&lt;userlib&gt;/ROS for LabVIEW Software/ROS/MessageBuilding/std_msgs/add_time.vi"/>
				<Item Name="add_uint32.vi" Type="VI" URL="/&lt;userlib&gt;/ROS for LabVIEW Software/ROS/MessageBuilding/std_msgs/add_uint32.vi"/>
				<Item Name="boolArray.vi" Type="VI" URL="/&lt;userlib&gt;/ROS for LabVIEW Software/ROS/MessageBuilding/std_msgs/subs/boolArray.vi"/>
				<Item Name="boolScalar.vi" Type="VI" URL="/&lt;userlib&gt;/ROS for LabVIEW Software/ROS/MessageBuilding/std_msgs/subs/boolScalar.vi"/>
				<Item Name="getOpenPort.vi" Type="VI" URL="/&lt;userlib&gt;/ROS for LabVIEW Software/ROS/Code/SubVIs/getOpenPort.vi"/>
				<Item Name="getTimeStamp.vi" Type="VI" URL="/&lt;userlib&gt;/ROS for LabVIEW Software/ROS/MessageBuilding/getTimeStamp.vi"/>
				<Item Name="parseErrorCheck.vi" Type="VI" URL="/&lt;userlib&gt;/ROS for LabVIEW Software/ROS/MessageParsing/subs/parseErrorCheck.vi"/>
				<Item Name="parse_bool.vi" Type="VI" URL="/&lt;userlib&gt;/ROS for LabVIEW Software/ROS/MessageParsing/std_msgs/parse_bool.vi"/>
				<Item Name="parse_bool_array.vi" Type="VI" URL="/&lt;userlib&gt;/ROS for LabVIEW Software/ROS/MessageParsing/std_msgs/subs/parse_bool_array.vi"/>
				<Item Name="parse_bool_scalar.vi" Type="VI" URL="/&lt;userlib&gt;/ROS for LabVIEW Software/ROS/MessageParsing/std_msgs/subs/parse_bool_scalar.vi"/>
				<Item Name="parse_float32.vi" Type="VI" URL="/&lt;userlib&gt;/ROS for LabVIEW Software/ROS/MessageParsing/std_msgs/parse_float32.vi"/>
				<Item Name="parse_float32_array.vi" Type="VI" URL="/&lt;userlib&gt;/ROS for LabVIEW Software/ROS/MessageParsing/std_msgs/subs/parse_float32_array.vi"/>
				<Item Name="parse_float32_scalar.vi" Type="VI" URL="/&lt;userlib&gt;/ROS for LabVIEW Software/ROS/MessageParsing/std_msgs/subs/parse_float32_scalar.vi"/>
				<Item Name="parse_header.vi" Type="VI" URL="/&lt;userlib&gt;/ROS for LabVIEW Software/ROS/MessageParsing/std_msgs/parse_header.vi"/>
				<Item Name="parse_string.vi" Type="VI" URL="/&lt;userlib&gt;/ROS for LabVIEW Software/ROS/MessageParsing/std_msgs/parse_string.vi"/>
				<Item Name="parse_string_array.vi" Type="VI" URL="/&lt;userlib&gt;/ROS for LabVIEW Software/ROS/MessageParsing/std_msgs/subs/parse_string_array.vi"/>
				<Item Name="parse_string_scalar.vi" Type="VI" URL="/&lt;userlib&gt;/ROS for LabVIEW Software/ROS/MessageParsing/std_msgs/subs/parse_string_scalar.vi"/>
				<Item Name="parse_time.vi" Type="VI" URL="/&lt;userlib&gt;/ROS for LabVIEW Software/ROS/MessageParsing/std_msgs/parse_time.vi"/>
				<Item Name="parse_time_array.vi" Type="VI" URL="/&lt;userlib&gt;/ROS for LabVIEW Software/ROS/MessageParsing/std_msgs/subs/parse_time_array.vi"/>
				<Item Name="parse_time_scalar.vi" Type="VI" URL="/&lt;userlib&gt;/ROS for LabVIEW Software/ROS/MessageParsing/std_msgs/subs/parse_time_scalar.vi"/>
				<Item Name="parse_u8_array.vi" Type="VI" URL="/&lt;userlib&gt;/ROS for LabVIEW Software/ROS/MessageParsing/std_msgs/subs/parse_u8_array.vi"/>
				<Item Name="parse_u8_scalar.vi" Type="VI" URL="/&lt;userlib&gt;/ROS for LabVIEW Software/ROS/MessageParsing/std_msgs/subs/parse_u8_scalar.vi"/>
				<Item Name="parse_u32_array.vi" Type="VI" URL="/&lt;userlib&gt;/ROS for LabVIEW Software/ROS/MessageParsing/std_msgs/subs/parse_u32_array.vi"/>
				<Item Name="parse_u32_scalar.vi" Type="VI" URL="/&lt;userlib&gt;/ROS for LabVIEW Software/ROS/MessageParsing/std_msgs/subs/parse_u32_scalar.vi"/>
				<Item Name="parse_uint8.vi" Type="VI" URL="/&lt;userlib&gt;/ROS for LabVIEW Software/ROS/MessageParsing/std_msgs/parse_uint8.vi"/>
				<Item Name="parse_uint32.vi" Type="VI" URL="/&lt;userlib&gt;/ROS for LabVIEW Software/ROS/MessageParsing/std_msgs/parse_uint32.vi"/>
				<Item Name="prependLength.vi" Type="VI" URL="/&lt;userlib&gt;/ROS for LabVIEW Software/ROS/MessageBuilding/prependLength.vi"/>
				<Item Name="stringArray.vi" Type="VI" URL="/&lt;userlib&gt;/ROS for LabVIEW Software/ROS/MessageBuilding/std_msgs/subs/stringArray.vi"/>
				<Item Name="stringScalar.vi" Type="VI" URL="/&lt;userlib&gt;/ROS for LabVIEW Software/ROS/MessageBuilding/std_msgs/subs/stringScalar.vi"/>
				<Item Name="timeArray.vi" Type="VI" URL="/&lt;userlib&gt;/ROS for LabVIEW Software/ROS/MessageBuilding/std_msgs/subs/timeArray.vi"/>
				<Item Name="timeScalar.vi" Type="VI" URL="/&lt;userlib&gt;/ROS for LabVIEW Software/ROS/MessageBuilding/std_msgs/subs/timeScalar.vi"/>
				<Item Name="u32Array.vi" Type="VI" URL="/&lt;userlib&gt;/ROS for LabVIEW Software/ROS/MessageBuilding/std_msgs/subs/u32Array.vi"/>
				<Item Name="u32Scalar.vi" Type="VI" URL="/&lt;userlib&gt;/ROS for LabVIEW Software/ROS/MessageBuilding/std_msgs/subs/u32Scalar.vi"/>
			</Item>
			<Item Name="vi.lib" Type="Folder">
				<Item Name="Check if File or Folder Exists.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/libraryn.llb/Check if File or Folder Exists.vi"/>
				<Item Name="Create Directory Recursive.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/libraryn.llb/Create Directory Recursive.vi"/>
				<Item Name="Dflt Data Dir.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/file.llb/Dflt Data Dir.vi"/>
				<Item Name="Error Cluster From Error Code.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Error Cluster From Error Code.vi"/>
				<Item Name="Escape Characters for HTTP.vi" Type="VI" URL="/&lt;vilib&gt;/printing/PathToURL.llb/Escape Characters for HTTP.vi"/>
				<Item Name="Get System Web Browser.vi" Type="VI" URL="/&lt;vilib&gt;/Platform/browser.llb/Get System Web Browser.vi"/>
				<Item Name="Get Web Browser Path.vi" Type="VI" URL="/&lt;vilib&gt;/Platform/browser.llb/Get Web Browser Path.vi"/>
				<Item Name="LVDateTimeRec.ctl" Type="VI" URL="/&lt;vilib&gt;/Utility/miscctls.llb/LVDateTimeRec.ctl"/>
				<Item Name="NI_FileType.lvlib" Type="Library" URL="/&lt;vilib&gt;/Utility/lvfile.llb/NI_FileType.lvlib"/>
				<Item Name="NI_PackedLibraryUtility.lvlib" Type="Library" URL="/&lt;vilib&gt;/Utility/LVLibp/NI_PackedLibraryUtility.lvlib"/>
				<Item Name="Open URL in Default Browser (path).vi" Type="VI" URL="/&lt;vilib&gt;/Platform/browser.llb/Open URL in Default Browser (path).vi"/>
				<Item Name="Open URL in Default Browser (string).vi" Type="VI" URL="/&lt;vilib&gt;/Platform/browser.llb/Open URL in Default Browser (string).vi"/>
				<Item Name="Open URL in Default Browser core.vi" Type="VI" URL="/&lt;vilib&gt;/Platform/browser.llb/Open URL in Default Browser core.vi"/>
				<Item Name="Open URL in Default Browser.vi" Type="VI" URL="/&lt;vilib&gt;/Platform/browser.llb/Open URL in Default Browser.vi"/>
				<Item Name="Path to URL.vi" Type="VI" URL="/&lt;vilib&gt;/printing/PathToURL.llb/Path to URL.vi"/>
				<Item Name="Prompt Web Browser Path.vi" Type="VI" URL="/&lt;vilib&gt;/Platform/browser.llb/Prompt Web Browser Path.vi"/>
				<Item Name="System Exec.vi" Type="VI" URL="/&lt;vilib&gt;/Platform/system.llb/System Exec.vi"/>
				<Item Name="Trim Whitespace.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Trim Whitespace.vi"/>
				<Item Name="whitespace.ctl" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/whitespace.ctl"/>
			</Item>
		</Item>
		<Item Name="Build Specifications" Type="Build">
			<Item Name="executable" Type="EXE">
				<Property Name="App_copyErrors" Type="Bool">true</Property>
				<Property Name="App_INI_aliasGUID" Type="Str">{D807B194-8707-11EB-91FF-C400AD627458}</Property>
				<Property Name="App_INI_GUID" Type="Str">{D807AB47-8707-11EB-91FF-C400AD627458}</Property>
				<Property Name="App_serverConfig.httpPort" Type="Int">8002</Property>
				<Property Name="Bld_autoIncrement" Type="Bool">true</Property>
				<Property Name="Bld_buildCacheID" Type="Str">{D807BDDB-8707-11EB-91FF-C400AD627458}</Property>
				<Property Name="Bld_buildSpecName" Type="Str">executable</Property>
				<Property Name="Bld_excludeInlineSubVIs" Type="Bool">true</Property>
				<Property Name="Bld_excludeLibraryItems" Type="Bool">true</Property>
				<Property Name="Bld_excludePolymorphicVIs" Type="Bool">true</Property>
				<Property Name="Bld_localDestDir" Type="Path">/home/kajuha/catkin_ws/src/labview_ui/builds/labview_ros_ui</Property>
				<Property Name="Bld_modifyLibraryFile" Type="Bool">true</Property>
				<Property Name="Bld_previewCacheID" Type="Str">{D807BE1E-8707-11EB-91FF-C400AD627458}</Property>
				<Property Name="Bld_version.build" Type="Int">3</Property>
				<Property Name="Bld_version.major" Type="Int">1</Property>
				<Property Name="DestinationCount" Type="Int">2</Property>
				<Property Name="Destination[0].destName" Type="Str">labview_ui</Property>
				<Property Name="Destination[0].path" Type="Path">/home/kajuha/catkin_ws/src/labview_ui/builds/labview_ros_ui/labview_ui</Property>
				<Property Name="Destination[0].path.type" Type="Str">&lt;none&gt;</Property>
				<Property Name="Destination[0].preserveHierarchy" Type="Bool">true</Property>
				<Property Name="Destination[0].type" Type="Str">App</Property>
				<Property Name="Destination[1].destName" Type="Str">Support Directory</Property>
				<Property Name="Destination[1].path" Type="Path">/home/kajuha/catkin_ws/src/labview_ui/builds/labview_ros_ui/data</Property>
				<Property Name="Destination[1].path.type" Type="Str">&lt;none&gt;</Property>
				<Property Name="SourceCount" Type="Int">2</Property>
				<Property Name="Source[0].itemID" Type="Str">{6EF6AC81-A1AF-11EB-A973-C400AD6273B6}</Property>
				<Property Name="Source[0].type" Type="Str">Container</Property>
				<Property Name="Source[1].destinationIndex" Type="Int">0</Property>
				<Property Name="Source[1].itemID" Type="Ref">/My Computer/mainVI/ui.vi</Property>
				<Property Name="Source[1].propertiesCount" Type="Int">5</Property>
				<Property Name="Source[1].properties[0].type" Type="Str">Show menu bar</Property>
				<Property Name="Source[1].properties[0].value" Type="Bool">false</Property>
				<Property Name="Source[1].properties[1].type" Type="Str">Show vertical scroll bar</Property>
				<Property Name="Source[1].properties[1].value" Type="Bool">false</Property>
				<Property Name="Source[1].properties[2].type" Type="Str">Show horizontal scroll bar</Property>
				<Property Name="Source[1].properties[2].value" Type="Bool">false</Property>
				<Property Name="Source[1].properties[3].type" Type="Str">Show toolbar</Property>
				<Property Name="Source[1].properties[3].value" Type="Bool">false</Property>
				<Property Name="Source[1].properties[4].type" Type="Str">Show Abort button</Property>
				<Property Name="Source[1].properties[4].value" Type="Bool">false</Property>
				<Property Name="Source[1].sourceInclusion" Type="Str">TopLevel</Property>
				<Property Name="Source[1].type" Type="Str">VI</Property>
				<Property Name="TgtF_fileDescription" Type="Str">executable</Property>
				<Property Name="TgtF_internalName" Type="Str">executable</Property>
				<Property Name="TgtF_legalCopyright" Type="Str">Copyright 2021 </Property>
				<Property Name="TgtF_productName" Type="Str">executable</Property>
				<Property Name="TgtF_targetfileGUID" Type="Str">{D807BBE8-8707-11EB-91FF-C400AD627458}</Property>
				<Property Name="TgtF_targetfileName" Type="Str">labview_ui</Property>
				<Property Name="TgtF_versionIndependent" Type="Bool">true</Property>
			</Item>
		</Item>
	</Item>
</Project>
