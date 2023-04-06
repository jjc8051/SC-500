//=================================================================================================
//		File  Name	:	loadcell.h
//		File  내용	:	load cell 펌웨어 헤더
//=================================================================================================
#ifndef __LoadCell_H__
#define __LoadCell_H__

//=================================================================================================
#include <main.h>

//=================================================================================================
uint64_t RO_10000;						// 로드셀 RO 10000의 자리
uint64_t RO_1000;						// 로드셀 RO  1000의 자리
uint64_t RO_100;						// 로드셀 RO   100의 자리
uint64_t RO_10;							// 로드셀 RO    10의 자리
uint64_t RO_1;							// 로드셀 RO     1의 자리
uint64_t RO;							// 로드셀 RO

double Cell_RO_1;						// 1번 로드셀 RO
double Cell_RO_2;						// 2번 로드셀 RO
double Cell_RO_3;						// 3번 로드셀 RO
//-------------------------------------------------------------------------------------------------
uint8_t INT_100;						// 로드셀 보정값 100의 자리
uint8_t INT_10;							// 로드셀 보정값  10의 자리
uint8_t INT_1;							// 로드셀 보정값   1의 자리
int16_t INT;							// 로드셀 보정값

int16_t Interval_1;						// 1번 로드셀 보정값
int16_t Interval_2;						// 2번 로드셀 보정값
int16_t Interval_3;						// 3번 로드셀 보정값

//-------------------------------------------------------------------------------------------------
uint8_t CAPA_1000;						// 로드셀 CAPA 1000의 자리
uint8_t CAPA_100;						// 로드셀 CAPA  100의 자리
uint8_t CAPA_10;						// 로드셀 CAPA   10의 자리
uint8_t CAPA_1;							// 로드셀 CAPA    1의 자리
uint32_t CAPA;							// 로드셀 CAPA

uint32_t Cell_Capa_1;						// 1번 로드셀 CAPA
uint32_t Cell_Capa_2;						// 2번 로드셀 CAPA
uint32_t Cell_Capa_3;						// 3번 로드셀 CAPA

//-------------------------------------------------------------------------------------------------
uint8_t ID;							// 로드셀 ID
uint8_t Cell_ID_1;						// 1번 로드셀 ID
uint8_t Cell_ID_2;						// 2번 로드셀 ID
uint8_t Cell_ID_3;						// 3번 로드셀 ID

uint8_t CNT;							// 로드셀 갯수 설정값
uint8_t Cell_CNT_1;						// 1번 로드셀 갯수 설정값
uint8_t Cell_CNT_2;						// 2번 로드셀 갯수 설정값
uint8_t Cell_CNT_3;						// 3번 로드셀 갯수 설정값

//-------------------------------------------------------------------------------------------------
uint8_t IN_S_10000;						// 입구 체중  10.000kg의 자리
uint8_t IN_S_1000;						// 입구 체중   1.000kg의 자리
uint8_t IN_S_100;						// 입구 체중   0.100kg의 자리
uint8_t IN_S_10;						// 입구 체중   0.010kg의 자리
uint8_t IN_S_1;							// 입구 체중   0.001kg의 자리
uint16_t IN_S;							// 입구 체중

//=================================================================================================
void Value_Desplay(uint8_t Number);				// 로드셀 측정 무게 표시
void Cell_Desplay(uint8_t Number);				// 로드셀 무게 표시
void INT_Desplay(uint8_t Number);				// 보정값 무게 표시

int32_t CellRead_1(void);					// 1 번 셀 Data 읽기 HX711 제어
int32_t CellRead_2(void);					// 2 번 셀 Data 읽기 HX711 제어
int32_t CellRead_3(void);					// 3 번 셀 Data 읽기 HX711 제어

uint16_t Read_Load_Cell(uint8_t Number);			// 로드셀 측정값 읽기
void Cell_Zero_Set(uint8_t Number);				// 로드셀 0 값 찾기

void RO_Set(uint8_t Number);					// 로드셀 RO 설정
void Interval_Set(uint8_t Number);				// Interval 설정
void CAPA_Set(uint8_t Number);					// 로드셀 CAPA 설정
void CNT_Set(uint8_t Number);					// 로드셀 갯수 설정
void ID_Set(uint8_t Number);					// 로드셀 ID 설정

void Cell_Select(uint8_t Number);				// 로드셀 선택
void Loadcell_Set(void);					// 로드셀 설정 모드

//=================================================================================================
//		END OF "loadcell.h"
//=================================================================================================
#endif
