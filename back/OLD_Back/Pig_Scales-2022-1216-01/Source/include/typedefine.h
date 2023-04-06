//=================================================================================================
//		File  Name	:	TypeDefine.h
//		File  내용	:	TypeDefine 펌웨어 헤더
//=================================================================================================
#ifndef __TypeDefine_H__
#define __TypeDefine_H__

//=================================================================================================
#include <main.h>

//=================================================================================================
typedef enum RTC_NUM						// RTC_NUM 열거형 상수 선언
{
	RTC_SECOND	= 0x00,					// 초   읽기 / 쓰기
	RTC_MINUTE	= 0x01,					// 분   읽기 / 쓰기
	RTC_HOUR	= 0x02,					// 시   읽기 / 쓰기
	RTC_DAYOFWEEK	= 0x03,					// 요일 읽기 / 쓰기
	RTC_DATE	= 0x04,					// 일   읽기 / 쓰기
	RTC_MONTH	= 0x05,					// 월   읽기 / 쓰기
	RTC_YEAR	= 0x06,					// 년   읽기 / 쓰기
	RTC_CONTROL	= 0x07,					// 제어 읽기 / 쓰기
	RTC_RAM_FIRST	= 0x08,					// RAM  읽기 / 쓰기
	RTC_RAM_LAST	= 0x3F,					// RAM  읽기 / 쓰기
} RTC_NUM;
//=================================================================================================
typedef struct							// _RTC_T 구조체 선언
{
	uint8_t Sec;						// 초
	uint8_t Min;						// 분
	uint8_t Hour;						// 시
	uint8_t Day;						// 요일
	uint8_t Date;						// 일
	uint8_t Month;						// 월
	uint8_t Year;						// 년

	uint8_t OLD_D;						// 기존 날짜

} _RTC_T;
_RTC_T RTC_T;							// RTC_T 구조체명 선언

//=================================================================================================
typedef struct							// _TIMER 구조체 선언
{
	volatile uint8_t Input_Key;				// Input_Key
	volatile uint16_t RegExitTimer;				// 설정 모드후 일정시간(10초) 버튼 입력 없으면 탈출
	volatile uint16_t RTC_Timer;				// RTC 시간을 읽어오는 타이머
	volatile uint16_t Check_Limit_Timer;			// 무게측정 제한 시간 Timer

} _TIMER;
_TIMER Timer;							// Timer 구조체명 선언

//=================================================================================================
typedef struct							// _MODE 구조체 선언
{
	volatile uint8_t Menu;					// 메뉴 : key 4
	volatile uint8_t Select;				// 선택 : key 2
	volatile uint8_t Blink;					// 케이트 웨이 ID, 입구 체 설정 모드 진입

} _MODE;
_MODE SET;							// SET 구조체명 선언

//=================================================================================================
typedef struct							// _KEY 구조체 선언
{
	volatile uint8_t Count;					// key 눌린 갯수
	volatile uint8_t Buff;					// Key Buff
	volatile uint8_t Key;					// key 값
	volatile uint8_t Old;					// Old key 값
	volatile uint8_t In_Count;				// in put count

} _KEY;
_KEY KEY;							// KEY 구조체명 선언

//=================================================================================================
typedef struct							// _RoadCell 구조체 선언
{
	volatile int8_t Zero_Buff;				// 로드셀 측정 값 Zero Buff

	volatile uint16_t Count;				// 로드셀 측정 횟수

	volatile int32_t Value;					// 로드셀 측정 값
	volatile int32_t Old_Value;				// 이전 측정 값
	volatile int32_t Old_ValSum;				// 이전 측정 값
	volatile int32_t OFF_Set;				// 로드셀 OFF_Set(입력값을 0값 으로 만들기위한 값)

	volatile double Proportion;				// 로드셀 비율

} _RoadCell;
_RoadCell CELL;							// CELL 구조체명 선언

uint16_t Data[20];						// 로드셀 측정 값 버퍼 20개

//=================================================================================================
//		END OF "TypeDefine.h"
//=================================================================================================
#endif