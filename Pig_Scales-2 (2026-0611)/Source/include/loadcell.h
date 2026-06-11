//=================================================================================================
//		File  Name	:	loadcell.h
//		File  내용	:	load cell 펌웨어 헤더
//=================================================================================================
#ifndef __LoadCell_H__
#define __LoadCell_H__

//=================================================================================================
#include <main.h>

//=================================================================================================
extern double Cell_RO;							// 1번 로드셀 RO
extern uint64_t RO_10000;						// 로드셀 RO 10000의 자리
extern uint64_t RO_1000;						// 로드셀 RO  1000의 자리
extern uint64_t RO_100;							// 로드셀 RO   100의 자리
extern uint64_t RO_10;							// 로드셀 RO    10의 자리
extern uint64_t RO_1;							// 로드셀 RO     1의 자리
extern uint64_t RO;							// 로드셀 RO

//-------------------------------------------------------------------------------------------------
extern uint32_t Cell_Capa;						// 1번 로드셀 CAPA
extern uint8_t CAPA_1000;						// 로드셀 CAPA 1000의 자리
extern uint8_t CAPA_100;						// 로드셀 CAPA  100의 자리
extern uint8_t CAPA_10;							// 로드셀 CAPA   10의 자리
extern uint8_t CAPA_1;							// 로드셀 CAPA    1의 자리

//-------------------------------------------------------------------------------------------------
extern uint8_t Cell_ID;							// 로드셀 ID 값
extern uint8_t Cell_CNT;						// 로드셀 갯수 값

//-------------------------------------------------------------------------------------------------
extern int16_t IN_Scales;						// 입구 체중
extern uint8_t IN_S_10000;						// 입구 체중  10.000kg의 자리
extern uint8_t IN_S_1000;						// 입구 체중   1.000kg의 자리
extern uint8_t IN_S_100;						// 입구 체중   0.100kg의 자리
extern uint8_t IN_S_10;							// 입구 체중   0.010kg의 자리
extern uint8_t IN_S_1;							// 입구 체중   0.001kg의 자리

//-------------------------------------------------------------------------------------------------
extern uint8_t Weight_10000;						// 무게 100.00의 자리
extern uint8_t Weight_1000;						// 무게  10.00의 자리
extern uint8_t Weight_100;						// 무게   1.00의 자리
extern uint8_t Weight_10;						// 무게   0.10의 자리
extern uint8_t Weight_1;						// 무게   0.01의 자리

//=================================================================================================
void Cell_Desplay(void);						// 로드셀 무게 표시
int32_t Cell_Data_Read(void);						// 로드셀 Data 읽기 HX711 제어
uint16_t Read_Load_Cell(void);						// 로드셀 측정값 읽기
void Cell_Zero_Set(void);						// 로드셀 0 값 찾기
void IN_Scales_Set(void);						// IN_Scales 설정
void Interval_Set(void);						// Interval 설정
void RO_Set(void);							// 로드셀 RO 설정
void CAPA_Set(void);							// 로드셀 CAPA 설정
void CNT_Set(void);							// 로드셀 갯수 설정
void ID_Set(void);							// 로드셀 ID 설정
void Loadcell_Set(void);						// 로드셀 설정 모드

//=================================================================================================
//		END OF "loadcell.h"
//=================================================================================================
#endif
