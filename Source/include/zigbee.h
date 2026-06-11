//=================================================================================================
//		File  Name	:	zigbee.h
//		File  내용	:	zigbee 펌웨어 헤더
//=================================================================================================
#ifndef __ZigBee_H__
#define __ZigBee_H__

//=================================================================================================
#include <main.h>

//=================================================================================================
uint16_t Pan_id_1000;							// ZigBee Pan_id 1000의 자리
uint16_t Pan_id_100;							// ZigBee Pan_id  100의 자리
uint16_t Pan_id_10;							// ZigBee Pan_id   10의 자리
uint16_t Pan_id_1;							// ZigBee Pan_id    1의 자리
uint16_t ZB_id;								// ZigBee Pan_id

uint8_t ZB_ch;								// ZigBee channel

//=================================================================================================
void ZigBee_Write_id(void);						// ZigBee PanID 저장
void ZigBee_PanID_Write(void);						// ZigBee PanID 설정
void ZigBee_Write_ch(void);						// ZigBee 채널 저장
void ZigBee_Channel_Write(void);					// ZigBee 채널 설정
void ZigBee_Read(void);							// ZigBee 설정값 읽기
void ZigBee_Set(void);							// ZigBee 채널 및 PanID 설정 모드

//=================================================================================================
//		END OF "zigbee.h"
//=================================================================================================
#endif
