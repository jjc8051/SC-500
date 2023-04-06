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
// 	RTC_CONTROL	= 0x07,					// 제어 읽기 / 쓰기
// 	RTC_RAM_FIRST	= 0x08,					// RAM  읽기 / 쓰기
// 	RTC_RAM_LAST	= 0x3F,					// RAM  읽기 / 쓰기
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
} _TIMER;
_TIMER Timer;							// Timer 구조체명 선언

//=================================================================================================
typedef struct							// _MODE 구조체 선언
{
	volatile uint8_t Menu;					// 메뉴 : key 4
	volatile uint8_t Select;				// 선택 : key 2
	volatile uint8_t NumSET;				// NumSET : key 1
	volatile uint8_t Blink;					// 케이트 웨이 ID, 입구 체중 설정 모드 진입
	volatile uint8_t Save;					// 저장 : Not Menu Mode & key 2
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

	volatile int32_t Value;					// 로드셀 측정 값
	volatile int32_t OFF_Set;				// 로드셀 OFF_Set(입력값을 0값 으로 만들기위한 값)
	volatile int32_t Old_Value;				// 이전 측정 값

	volatile double Proportion;				// 로드셀 비율
} _RoadCell;
_RoadCell CELL;							// CELL 구조체명 선언


//=================================================================================================
typedef struct							// _SaveData 구조체 선언
{
	int8_t AutoSet[300];					// 자동 설정
	uint16_t Count[300];					// 측정 횟수
	uint16_t Data[300];					// 측정 값 버퍼 300개
	int8_t RoomNum[300];					// 방 번호
	int8_t StalNum[300];					// 스톨 번호

	int8_t Year[300];					// 년
	int8_t Month[300];					// 월
	int8_t Date[300];					// 일
	int8_t Hour[300];					// 시
	int8_t Min[300];					// 분
	int8_t Sec[300];					// 초
} _SaveData;
_SaveData SAVE;							// SAVE 구조체명 선언

//=================================================================================================
//		END OF "TypeDefine.h"
//=================================================================================================
#endif
