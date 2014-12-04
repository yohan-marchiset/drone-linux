#include "udp_protocol.h"

uint8_t sendFrame(char type, uint32_t data1, uint32_t data2, uint32_t data3, char stateMission){
	wifiFrame f = createWifiFrame(type, data1, data2, data3, stateMission);
	char * convertedFrame = wifiFrameToChar(f);
	if(udp_async_driver_write((unsigned char *)convertedFrame, CONVERTED_WIFI_FRAME_SIZE, fd_protocol)==0) {
		#ifdef DEBUG_UDP_PROTOCOL
			printf("[Debug] L%d %s : Write OK !\n",__LINE__,__FUNCTION__);		
		#endif
		return 0;
	}
	else {
		return 1;
	}
}


void readFrame(unsigned char * data, int size){
	if(size==CONVERTED_WIFI_FRAME_SIZE)
	{
	        wifiFrame wf = wifiFrameFromChar((char*)data);

		if (wf.stateMission == LAUNCH_MISSION)
		{
			sendFrame(MISSION_FRAME, wf.positions[0], wf.positions[1], wf.positions[2], LAUNCH_MISSION) ;
		}
		else if (wf.stateMission == STOP_MISSION)
		{
			sendFrame(MISSION_FRAME, wf.positions[0], wf.positions[1], wf.positions[2], STOP_MISSION) ;
		}
		//printf("frame:\nseqnum=%d\ntype=%c\ndata=%d; %d; %d; %d\n",f.seqNum,f.type,f.data[0],f.data[1],f.data[2],f.data[3]);
	}
}


uint8_t initCommunication(){
	#ifdef DEBUG_INFO
		printf("[Debug] L%d %s : Initializing UDP ...\n",__LINE__,__FUNCTION__);			
	#endif
	//char IP[IP_SIZE];
	wifiFrame disco_frame = createWifiFrame(DISCOVERY_FRAME, 0, 0, 0,STOP_MISSION);
	char * disco_data = wifiFrameToChar(disco_frame);
	
	// waiting for PC to answer discovery frame (to get its IP for future communication)
	while(udp_driver_discover_network(LOCAL_PORT,REMOTE_PORT,(unsigned char *)disco_data, CONVERTED_WIFI_FRAME_SIZE ,(unsigned char *)disco_data, CONVERTED_WIFI_FRAME_SIZE , MAX_DISCOVERY_WAIT_TIME, IP)<0) {
		printf("[Warning] L%d %s : Can't find a computer\n",__LINE__,__FUNCTION__);			
	}

	#ifdef DEBUG_INFO
		printf("[Debug] L%d %s : Computer found %s\n",__LINE__,__FUNCTION__,IP);			
	#endif
	
	// initializing communication with PC
	if(udp_async_driver_init(LOCAL_PORT, REMOTE_PORT,IP, &fd_protocol)==0){
		if(udp_async_driver_enable_read(&readFrame, MAX_PACKET_SIZE, fd_protocol)==0){
			#ifdef DEBUG_INFO
				printf("[Debug] L%d %s : Done !\n",__LINE__,__FUNCTION__);		
			#endif
			return 0;
		}
	}
	return 1;
}


uint8_t closeCommunication(){
	#ifdef DEBUG_INFO
		printf("[Debug] L%d %s : Closing UDP ...\n",__LINE__,__FUNCTION__);		
	#endif
	if(udp_async_driver_close(fd_protocol)==0) {
			#ifdef DEBUG_INFO
				printf("[Debug] L%d %s : Done !\n",__LINE__,__FUNCTION__);		
			#endif
		return 0;
	}
	else {
		return 1;
	}
}
