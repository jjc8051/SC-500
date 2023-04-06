//=================================================================================================
//		ProductName 	:	Pig_Scales V4.1 SC-500
//=================================================================================================
//		File  Name	:	main.c
//		File  내용	:	이동형 체중 측정기(자돈)
//		작성  일자	:	2022년 12월 13일
//		수정  일자	:	2023년 01월 05일
//=================================================================================================
//		BOARD		:	Pig Scales Control Board
//		CPU		:	AVR128DA64 [ EXTCLK:14745600Hz, REG:32X8_Bit, SRAM:16K_Byte, EEPROM:512_Byte, FLASH:128K_Byte, PACKAGE:64_TQFP-10mm ]
//		CLOCK		:	14745600Hz (External Clock)
//		COMPILER	:	Microchip Studio (Version:7.0.2542)
//=================================================================================================
//		컴파일러 내부설정
//=================================================================================================
//		Application	:	Toolchain \ AVR/GNU Linker \ Memory Setting \ FLASH segment ==> .text=0x300 로 설정 (Boot Loader 사용)
//-------------------------------------------------------------------------------------------------
//		printf_usart_port = 0;							// printf 출력 Port 설정 0 ~ 5 까지
//		printf("---   Compiled : %s, %s ---\r\n\r\n", __DATE__, __TIME__);	// Compiled : 날짜, 시간 표시
//		printf("==>  Check Point : %d Line\r\n\r\n", __LINE__);			// Check Point : 행(줄) 표시
//=================================================================================================
//			펌웨어 이력
//=================================================================================================
//
//	2022-1213	Pig_Scales V3.1을 기본으로 SC-500 펌웨어 이식 작업 시작
//			250Kg 로드셀 4개짜리 1개의 장비만 설정
//	2022-1216	기존 Pig_Scales V3.1을 1개의 로드셀만 표시하도록 처리 완료
//
//	2022-1220	측정체중이 안정 적이면 FND 1에 ----- 표시
//			선택(Select)Key 가 눌려지고, 측정체중이 안정 되어 있으면 무게 측정 OK 처리
//			통신 프로토콜 적용 송신 루틴 정리
//
//	2022-1221	Normal Mode 일때 +(UP) ▲ key 가 눌리면 방 번호설정 모드로 진입
//			상, 하 Key 로 설정 후 선택버튼을 누르면 스톨번호 설정모드 진입
//			상, 하 Key 로 설정 후 선택버튼을 누르면 설정 완료, 저장 후 Normal Mode 로 복귀
//
//	2022-1222	Normal Mode 일때 선택 Key 입력이 있으면 무게를 측정하고 Data 저장, 카운터 증가
//			선택(저장) 입력이 없으면 무게 안정화 시 FND 1에 ----- 표시,
//			선택(저장) 입력이 있고, 무게 안정화 시 FND 1, 2에 ----- 표시,
//			선택(저장) 입력이 있으면서 무게가 안정화 되지 않으면 FND 1에만 ----- 표시하고 안정화 대기 후 측정 완료 		
//
//	2022-1223	방 번호, 스톨번호 EEPEOM 에 저장
//			날짜 변경시 기존 Data 초기화
//			자동 및 수동 설정 완료 (작동 Test 작업 진행 중)
//
//	2023-0105	EEPROM Data	
//
//=================================================================================================
#include <main.h>
#include <avr/fuse.h>

//=================================================================================================
FUSES =
{
	.SYSCFG0 = CRCSRC_NOCRC_gc  | RSTPINCFG_GPIO_gc,			// No CRC enabled, RST pin in GPIO mode
	.BODCFG  = ACTIVE_ENABLE_gc | LVL_BODLEVEL3_gc,				// BOD Configuration
	.WDTCFG  = PERIOD_4KCLK_gc  | WINDOW_OFF_gc,				// Watchdog Configuration
	.CODESIZE = 0x00,							// All remaining Flash used as App code (나머지 플래시는 모두 앱 코드로 사용됨)
	.BOOTSIZE = 0x03							// 0x0300
};

//=================================================================================================
const uint8_t fnd_tbl[]={FND_0, FND_1, FND_2, FND_3, FND_4, FND_5, FND_6, FND_7, FND_8, FND_9, FND_MINUS, FND_Point, FND_OFF};
//FND 숫자 테이블	{    0,     1,     2,     3,     4,     5,     6,     7,     8,     9,         -,         .,     OFF}

//=================================================================================================
uint8_t DIGIT[3][4] = {{0}};

//=================================================================================================
void MCU_Init(void)								// AVR128DA64 MCU Initialize
{
	PORTA_DIR = 0x0E;							//   Key4,   Key3,   Key2,   Key1,      x,  relay, buzzer,    CLK
	PORTB_DIR = 0x0D;							//      x,   IN 3,   IN 2,   IN 1,  RxLed,  TxLed,   RXD3,   TXD3
	PORTC_DIR = 0x55;							//    DO3,   SCK3,    DO2,   SCK2,    DO1,   SCK1,   RXD1,   TXD1
	PORTD_DIR = 0x00;							//      x,      x,      x,      x,      x,   ADC2,   ADC1,   ADC0
	PORTE_DIR = 0x00;							//      x,      x,      x,      x,      x,      x,      x,      x
	PORTF_DIR = 0x0C;							//      x,      x,      x,      x,   SCL1,   SDA1,      x,      x
	PORTG_DIR = 0xF7;							// DIGIT3, DIGIT2, DIGIT1, DIGIT0,      x,   DATA,  SHIFT,  LATCH : FND Display Control
//-------------------------------------------------------------------------------------------------
	PORTA_OUT = 0x00;
	PORTB_OUT = 0x00;
	PORTC_OUT = 0x00;
	PORTD_OUT = 0x00;
	PORTE_OUT = 0x00;
	PORTF_OUT = 0x00;
	PORTG_OUT = 0x00;
}

//=================================================================================================
void Fnd_Display(void)								// FND Display
{
	static uint8_t cnt = 0;							// cnt 변수 초기화

	for(int8_t j = 2; j >= 0; j--)						// FND 갯수만큼 (3회 반복)
	{
		for(uint8_t i = 0; i < 8; i++)					// FND 8bit data 전송(고정)
		{
			HC595_SHIFT_OFF;					// 8 X 1 bit Data Shift 전 HC595 SHIFT OFF
			if((DIGIT[j][cnt] >> i) & 0x01) {HC595_DATA_ON;}	// DIGIT[FND번호][DIGIT번호] 가 1 이면 HC595_DATA_ON 
			else				{HC595_DATA_OFF;}	// DIGIT[FND번호][DIGIT번호] 가 0 이면 HC595_DATA_OFF
			HC595_SHIFT_ON;						// 8 X 1 bit Data Shift 후 HC595 SHIFT ON
		}
	}
	HC595_LATCH_ON;								// FND 갯수 X 8bit Data Shift 후
	HC595_LATCH_OFF;							// HC595 LATCH ON, OFF
//-------------------------------------------------------------------------------------------------
	switch(cnt)
	{
		case 0: DIGIT3_OFF; DIGIT2_OFF; DIGIT1_OFF; DIGIT0_ON;  break;	// DIGIT0 ON 나머지 OFF
		case 1: DIGIT3_OFF; DIGIT2_OFF; DIGIT1_ON ; DIGIT0_OFF; break;	// DIGIT1 ON 나머지 OFF
		case 2: DIGIT3_OFF; DIGIT2_ON;  DIGIT1_OFF; DIGIT0_OFF; break;	// DIGIT2 ON 나머지 OFF
		case 3: DIGIT3_ON;  DIGIT2_OFF; DIGIT1_OFF; DIGIT0_OFF; break;	// DIGIT3 ON 나머지 OFF
	}
	cnt++;									// 다음 Digit 를 위해 cnt 증가
	if(cnt >= 4) {cnt = 0;}							// FND Digit 수만큼 (4회) 반복 후 초기화
}

//=================================================================================================
void FND_Value_Data(uint8_t start, uint8_t end, uint8_t value)			// FND 에 반복 되는 Data 표시
{
	for(uint8_t i = start; i <= end; i++)					// FND 에 start 부터 end 까지
	{
		for(uint8_t j = 0; j <= 3; j++)					// FND DIGIT 0 부터 3 까지 value Data 표시
		{
			DIGIT[i][j] = fnd_tbl[value];				// DIGIT[j] value Data 표시
		}
	}
}

//=================================================================================================
void FND_ALL_OFF(void)								// FND 전체 OFF
{
	for(uint8_t i = 0; i <= 2; i++)						// 0번 FND 부터 2번 FND 까지
	{
		DIGIT[i][3] = FND_OFF;						// DIGIT[3] OFF
		DIGIT[i][2] = FND_OFF;						// DIGIT[2] OFF
		DIGIT[i][1] = FND_OFF;						// DIGIT[1] OFF
		DIGIT[i][0] = FND_OFF;						// DIGIT[0] OFF
	}
}

//=================================================================================================
//		key 값 scan
//=================================================================================================
void Key_Scan(void)								// key 입력 체크
{
	if(!KEY.Buff)								// Key 입력이 없었으면
	{
		if(keytemp & 0x10) {Beep(10); KEY.Key = 1; KEY.Buff = 1; Timer.RegExitTimer = 0; Sec_Count = 0;}	// +(UP)  ▲ key 입력이 있으면 
		if(keytemp & 0x20) {Beep(10); KEY.Key = 2; KEY.Buff = 1; Timer.RegExitTimer = 0; Sec_Count = 0;}	// Select   key 입력이 있으면
		if(keytemp & 0x40) {Beep(10); KEY.Key = 3; KEY.Buff = 1; Timer.RegExitTimer = 0; Sec_Count = 0;}	// -(Down)▼ key 입력이 있으면
		if(keytemp & 0x80) {Beep(10); KEY.Key = 4; KEY.Buff = 1; Timer.RegExitTimer = 0; Sec_Count = 0;}	// Menu     key 입력이 있으면

		if(KEY.Key == 4)						// Menu key 입력이 있으면
		{
			if(SET.Menu) {SET.Menu = 0;}				// 메뉴모드 = 1 이면 메뉴모드 해제
			else	     {SET.Menu = 1;}				// 메뉴모드 = 0 이면 메뉴모드 설정 
		}

		if(SET.Menu && KEY.Key == 2 && !SET.Blink)			// 메뉴모드 이고 Blink = 0 이고 선택 Key 입력이 있으면
		{
			if(SET.Select) {SET.Select = 0;}			// 선택모드 = 1 이면 선택모드 해제
			else	       {SET.Select = 1;}			// 선택모드 = 0 이면 선택모드 설정
		}

		if(!SET.Menu && !SET.NumSET && KEY.Key == 2)			// 메뉴모드 와 SET.NumSET 가 1이 아니고 선택 Key 입력이 있으면
		{
			if(SET.Save) {SET.Save = 0;}				// 저장모드 = 1 이면 저장모드 해제
			else							// 저장모드 = 0 이면 저장모드 설정
			{
				SET.Save = 1;
				FND_Value_Data(2, 2, 10);			// FND 2 FND_MINUS 표시
			}
		}

		if(SET.Menu && KEY.Key == 2 && SET.Blink) {KEY.Count++;}	// 메뉴모드 이고 Blink = 1 이고 선택 Key 입력이 있으면 Key 가 눌린횟수 증가

		if(!SET.Menu && KEY.Key == 1) {if(!SET.NumSET) {SET.NumSET = 1;}}	// 메뉴모드가 아니고 선택 Key 입력이 있으면 NumSET모드 = 1 아니면 NumSET모드 설정

		if(KEY.Old == KEY.Key)						// Old Key 값이 New Key 값이 같으면
		{
			if(KEY.In_Count >= 2) {Timer.Input_Key = 20;}		// 연속입력이 2보다 많으면 연속입력시간 = 20
			else {Timer.Input_Key = 50; KEY.In_Count++;}		// 연속입력이 2보다 작으면 연속입력시간 = 50, 연속입력 증가
		}
		else								// Old Key 값이 New Key 값이 같지 않으면
		{
			Timer.Input_Key = 50;					// 연속입력 = 50
			KEY.Old = KEY.Key;					// Old Key 에 New Key 저장
			KEY.In_Count = 0;					// 연속입력 초기화
		}
	}
}

//=================================================================================================
void Read_eeprom(void)								// EEPROM Data 읽기
{
//-------------------------------------------------------------------------------------------------
//		게이트웨이 국번
//-------------------------------------------------------------------------------------------------
	Gukbun = ee_read_byte(Gateway_Gukbun_Addr);				// 게이트웨이 국번 설정값 불러오기
	if(Gukbun > 60 || Gukbun == 0)						// 게이트웨이 국번 설정값이 60보다 크거나 0과 같으면
	{
		Gukbun = 1;							// 게이트웨이 국번 설정값을 1로 초기화
		ee_write_byte(Gateway_Gukbun_Addr, Gukbun);			// 게이트웨이 국번 설정값을 EEPROM 에 저장
	}
//-------------------------------------------------------------------------------------------------
//		게이트웨이 ID
//-------------------------------------------------------------------------------------------------
	Gwid = ee_read_byte(Gateway_GWID_Addr);					// 게이트웨이 ID 설정값 불러오기
	if(Gwid > 60 || Gwid == 0)						// 게이트웨이 ID 설정값이 60보다 크거나 0과 같으면
	{
		Gwid = 1;							// 게이트웨이 ID 설정값을 1로 초기화
		ee_write_byte(Gateway_GWID_Addr, Gwid);				// 게이트웨이 ID 설정값을 EEPROM 에 저장
	}
//-------------------------------------------------------------------------------------------------
//		저장된 날짜 Date
//-------------------------------------------------------------------------------------------------
 	RTC_T.OLD_D = ee_read_byte(OLD_Date_Addr);				// 날짜 설정값 불러오기
	if(RTC_T.OLD_D > 31 || RTC_T.OLD_D < 0)					// 날짜 설정값이 31일보다 크거나 0보다 작으면
	{
		RTC_T.OLD_D = 1;						// 날짜 설정값을 1로 초기화
		ee_write_byte(OLD_Date_Addr, RTC_T.OLD_D);			// 날짜 설정값을 EEPROM 에 저장
	}
//-------------------------------------------------------------------------------------------------
//		로드셀 ID
//-------------------------------------------------------------------------------------------------
	Cell_ID = ee_read_byte(Cell_ID_Addr);					// 로드셀 ID 설정값 불러오기
	if(Cell_ID > 60 || Cell_ID < 0)						// 로드셀 ID 설정값이 60보다 크거나 0보다 작으면
	{
		Cell_ID = 1;							// 로드셀 ID 설정값을 1로 초기화
		ee_write_byte(Cell_ID_Addr, Cell_ID);				// 로드셀 ID 설정값을 EEPROM 에 저장
	}
//-------------------------------------------------------------------------------------------------
//		로드셀 갯수
//-------------------------------------------------------------------------------------------------
	Cell_CNT = ee_read_byte(Cell_CNT_Addr);					// 로드셀 갯수 설정값 불러오기
	if(Cell_CNT > 4 || Cell_CNT < 1)					// 로드셀 갯수 설정값이 8보다 크거나 2보다 작으면
	{
		Cell_CNT = 2;							// 로드셀 갯수 설정값을 2로 초기화
		ee_write_byte(Cell_CNT_Addr, Cell_CNT);				// 로드셀 갯수 설정값을 EEPROM 에 저장
	}
//-------------------------------------------------------------------------------------------------
//		로드셀 CAPA (용량)
//-------------------------------------------------------------------------------------------------
	Cell_Capa = ee_read_word(Cell_Capa_Addr);				// 로드셀 CAPA 설정값 불러오기
	if(Cell_Capa > 1000 || Cell_Capa < 100)					// 로드셀 CAPA 설정값이 1000보다 크거나 100보다 작으면
	{
		Cell_Capa = 100;						// 로드셀 CAPA 설정값을 100kg로 초기화, 곱하기 1,000으로 계산 100kg = 100000g
		ee_write_word(Cell_Capa_Addr, Cell_Capa);			// 로드셀 CAPA 설정값을 EEPROM 에 저장
	}
//-------------------------------------------------------------------------------------------------
//		로드셀 RO (정격출력)
//-------------------------------------------------------------------------------------------------
	Cell_RO = ee_read_dword(Cell_RO_Addr);					// 로드셀 RO 설정값 불러오기
	if(Cell_RO > 90000 || Cell_RO < 10000)					// 로드셀 RO 설정값이 9.000mV보다 크거나 3.000mV보다 작으면
	{
		Cell_RO = 40000;						// 로드셀 RO 설정값을 4.000mV로 초기화, 나누기 10,000으로 계산
		ee_write_dword(Cell_RO_Addr, Cell_RO);				// 로드셀 RO 설정값을 EEPROM 에 저장
	}
//-------------------------------------------------------------------------------------------------
//		입구 체중
//-------------------------------------------------------------------------------------------------
	IN_Scales = ee_read_word(Cell_IN_S_Addr);				// 입구체중 설정값 불러오기
	if(IN_Scales > 10000 || IN_Scales < 10)					// 입구체중 설정값이 10000g(10Kg)보다 크거나 10g보다 작으면
	{
		IN_Scales = 500;						// 입구체중 설정값을 500으로 초기화 (500g)
		ee_write_word(Cell_IN_S_Addr, IN_Scales);			// 입구체중 설정값을 EEPROM 에 저장
	}
//-------------------------------------------------------------------------------------------------
//		로드셀 보정 값
//-------------------------------------------------------------------------------------------------
	Interval = ee_read_word(Cell_Interval_Addr);				// 로드셀 보정 설정값 불러오기
	if((Interval > 99) || (Interval < -99))					// 로드셀 보정 설정값이 99보다 크거나 -99보다 작으면
	{
		Interval = 0;							// 로드셀 보정 설정값을 0 으로 초기화
		ee_write_word(Cell_Interval_Addr, Interval);			// 로드셀 보정 설정값을 EEPROM 에 저장
	}
//-------------------------------------------------------------------------------------------------
//		방 번호 값
//-------------------------------------------------------------------------------------------------
	Room_Number = ee_read_byte(Cell_ROOM_Addr);				// 방번호 설정값 불러오기
	if(Room_Number > 100 || Room_Number <= 0)				// 방번호 설정값이 100보다 크거나 0과 같거나 작으면
	{
		Room_Number = 1;						// 방번호 설정값을 1 으로 초기화
		ee_write_byte(Cell_ROOM_Addr, Room_Number);			// 방번호 설정값을 EEPROM 에 저장
	}
//-------------------------------------------------------------------------------------------------
//		스톨 번호 값
//-------------------------------------------------------------------------------------------------
	Stal_Number = ee_read_byte(Cell_STAL_Addr);				// 스톨번호 설정 불러오기
	if(Stal_Number > 100 || Stal_Number <= 0)				// 스톨번호 설정값이 100보다 크거나 0과 같거나 작으면
	{
		Stal_Number = 1;						// 스톨번호 설정값을 1 으로 초기화
		ee_write_byte(Cell_STAL_Addr, Stal_Number);			// 스톨번호 설정값을 EEPROM 에 저장
	}
//-------------------------------------------------------------------------------------------------
//		자동모드 설정 값
//-------------------------------------------------------------------------------------------------
	Auto_Mode = ee_read_byte(Cell_AUTO_Addr);				// 자동모드 설정값 불러오기
	if(Auto_Mode > 2 || Auto_Mode < 0)					// 자동모드 설정값이 2보다 크거나 0보다 작으면
	{
		Auto_Mode = 0;							// 자동모드 설정값을 0 으로 초기화
		ee_write_byte(Cell_AUTO_Addr, Auto_Mode);			// 자동모드 설정값을 EEPROM 에 저장
	}
//-------------------------------------------------------------------------------------------------
//		RTC 초기화
//-------------------------------------------------------------------------------------------------
	RTC_Time_Read();
	if(RTC_T.Sec > 59)							// 초 값이 59 보다 크면
	{
		RTC_Time_Write(RTC_SECOND, 0);					// RTC 에 00초 저장
		while(1);							// RTC 가 초기화 되지 않아 있으면 CPU ReSet
	}
//-------------------------------------------------------------------------------------------------
	if(RTC_T.Year == 0)							// 년 값이 00 이면
	{
		RTC_Time_Write(RTC_YEAR,      22);				// RTC 에 22년   저장
		RTC_Time_Write(RTC_MONTH,      9);				// RTC 에 09월   저장
		RTC_Time_Write(RTC_DATE,      20);				// RTC 에 20일   저장
		RTC_Time_Write(RTC_DAYOFWEEK,  3);				// RTC 에 03요일 저장
		RTC_Time_Write(RTC_HOUR,      12);				// RTC 에 12시   저장
		RTC_Time_Write(RTC_MINUTE,     0);				// RTC 에 00분   저장
		RTC_Time_Write(RTC_SECOND,     0);				// RTC 에 00초   저장
	}
}

//=================================================================================================
void Read_eeprom_Weight_Data(uint16_t cnt)					// EEPROM Weight Data 읽기
{
	SAVE.AutoSet[Data_Number] = ee_read_byte(MEASURE_AUTO   + (MEASURE_LENGTH * cnt));
	SAVE.Count  [Data_Number] = ee_read_word(MEASURE_COUNT  + (MEASURE_LENGTH * cnt));
	SAVE.Data   [Data_Number] = ee_read_word(MEASURE_WEIGHT + (MEASURE_LENGTH * cnt));
	SAVE.RoomNum[Data_Number] = ee_read_byte(ROOM_NUMBER    + (MEASURE_LENGTH * cnt));
	SAVE.StalNum[Data_Number] = ee_read_byte(STALL_NUMBER   + (MEASURE_LENGTH * cnt));
	
	SAVE.Year   [Data_Number] = ee_read_byte(MEASURE_YEAR   + (MEASURE_LENGTH * cnt));
	SAVE.Month  [Data_Number] = ee_read_byte(MEASURE_MONTH  + (MEASURE_LENGTH * cnt));
	SAVE.Date   [Data_Number] = ee_read_byte(MEASURE_DATE   + (MEASURE_LENGTH * cnt));
	SAVE.Hour   [Data_Number] = ee_read_byte(MEASURE_HOUR   + (MEASURE_LENGTH * cnt));
	SAVE.Min    [Data_Number] = ee_read_byte(MEASURE_MIN    + (MEASURE_LENGTH * cnt));
	SAVE.Sec    [Data_Number] = ee_read_byte(MEASURE_MIN    + (MEASURE_LENGTH * cnt));
}

//=================================================================================================
void Alarm(void)								// 알람(비프 3번) 
{
	Beep(15);								// 1mSec X 15 = 15mSec
	_delay_ms(120);								// Time Delay  120mSec
	Beep(15);								// 1mSec X 15 = 15mSec
	_delay_ms(120);								// Time Delay  120mSec
	Beep(15);								// 1mSec X 15 = 15mSec
}

//=================================================================================================
void Auto_Set(void)								// 자동모드,수동모드 설정
{
	while(SET.Menu && SET.Select)						// 메뉴가 1 이면 반복
	{
		if(KEY.Buff)							// Key 입력이 있으면
		{
			if     (KEY.Key == 1) {Auto_Mode = 1;}
			else if(KEY.Key == 3) {Auto_Mode = 0;}
		}

		if(Auto_Mode)
		{
			if(Blink) {FND_Value_Data(1,1,12);}			// Blink = 1 이면 FND 1 OFF
			else
			{
				DIGIT[1][3] = FND_OFF;
				DIGIT[1][2] = FND_OFF;
				DIGIT[1][1] = FND_O;
				DIGIT[1][0] = FND_N;
			}
		}
		else
		{
			if(Blink) {FND_Value_Data(1,1,12);}			// Blink = 1 이면 FND 1 OFF
			else
			{
				DIGIT[1][3] = FND_OFF;
				DIGIT[1][2] = FND_O;
				DIGIT[1][1] = FND_F;
				DIGIT[1][0] = FND_F;
			}
		}
		wdt_reset();							// 워치독 리셋
	}
	ee_write_byte(Cell_AUTO_Addr, Auto_Mode);				// 자동모드 설정값을 EEPROM 에 저장
}

//=================================================================================================
void Seting_Mode_0(void)							// 셋팅모드 0
{
	uint8_t Count = 1;							// Count 변수 초기화
	SET.Select = 0;								// Select 버튼 초기화

	FND_Value_Data(0, 2, 12);						// FND 0 ~ 2 FND_OFF

	while(SET.Menu)								// Seting_Mode 이면 무한 반복
	{
		if(KEY.Buff)							// Key 입력이 있으면
		{
			if     (KEY.Key == 1 && Count < 5) {Count++; KEY.Key = 0;}	// 업  버튼 이면 Count 증가
			else if(KEY.Key == 3 && Count > 0) {Count--; KEY.Key = 0;}	// 다운버튼 이면 Count 감소
		}
		DIGIT[2][3] = FND_OFF;
		DIGIT[2][2] = FND_OFF;
		DIGIT[2][1] = FND_OFF;
		DIGIT[2][0] = fnd_tbl[Count];					// FND 2.0 에 Count 표시
//-------------------------------------------------------------------------------------------------
		switch(Count)							// Count
		{
			case 0:							// Count = 0 이면
				Count = 5;					// 반복을 위해 Count를 최고로 증가 시킴
			break;

			case 1:							// Count = 1 이면
				DIGIT[0][3] = FND_a;				// cell 표시
				DIGIT[0][2] = FND_u;
				DIGIT[0][1] = FND_t;
				DIGIT[0][0] = FND_o;

				DIGIT[1][3] = FND_OFF;				// Set 표시
				DIGIT[1][2] = FND_S;
				DIGIT[1][1] = FND_e;
				DIGIT[1][0] = FND_t;

				if(SET.Select) {Auto_Set();}			// 자동모드,수동모드 설정
			break;

			case 2:							// Count = 2 이면
				DIGIT[0][3] = FND_c;				// cell 표시
				DIGIT[0][2] = FND_e;
				DIGIT[0][1] = FND_l;
				DIGIT[0][0] = FND_l;

				DIGIT[1][3] = FND_OFF;				// Set 표시
				DIGIT[1][2] = FND_S;
				DIGIT[1][1] = FND_e;
				DIGIT[1][0] = FND_t;

				if(SET.Select) {Loadcell_Set();}		// 로드셀 설정 모드
			break;

			case 3:							// Count = 3 이면
				DIGIT[0][3] = FND_OFF;				// gat 표시
				DIGIT[0][2] = FND_g;
				DIGIT[0][1] = FND_a;
				DIGIT[0][0] = FND_t;

				DIGIT[1][3] = FND_OFF;				// Set 표시
				DIGIT[1][2] = FND_S;
				DIGIT[1][1] = FND_e;
				DIGIT[1][0] = FND_t;

				if(SET.Select) {Gateway_Set();}			// 게이트 웨이 설정
			break;

			case 4:							// Count = 4 이면
				DIGIT[0][3] = FND_OFF;				// Zbe 표시
				DIGIT[0][2] = FND_Z;
				DIGIT[0][1] = FND_b;
				DIGIT[0][0] = FND_e;

				DIGIT[1][3] = FND_OFF;				// Set 표시
				DIGIT[1][2] = FND_S;
				DIGIT[1][1] = FND_e;
				DIGIT[1][0] = FND_t;

				if(SET.Select) {ZigBee_Set();}			// 지그비 설정 모드
			break;

			case 5:							// Count = 5 이면
				DIGIT[0][3] = FND_OFF;				// rtc 표시
				DIGIT[0][2] = FND_r;
				DIGIT[0][1] = FND_t;
				DIGIT[0][0] = FND_c;

				DIGIT[1][3] = FND_OFF;				// Set 표시
				DIGIT[1][2] = FND_S;
				DIGIT[1][1] = FND_e;
				DIGIT[1][0] = FND_t;

				if(SET.Select) {RTC_Set();}			// RTC 설정 모드
			break;

			case 6:							// Count = 6 이면
				Count = 1;					// 반복을 위해 Count를 최저로 감소 시킴
			break;
		}
		wdt_reset();							// 워치독 리셋
	}
	Var_Init();								// 변수 초기화
}

//=================================================================================================
void Seting_Mode_1(void)							// 셋팅모드 1
{
	uint8_t cnt = 0;
	_delay_ms(500);								// 모드 진입 시 연속 동작 방지를 위한 딜레이

	while(cnt != 2)								// 메뉴 와 선택 이 모두 1 이면 반복
	{
		if(cnt == 0)
		{
			if(KEY.Buff)						// Key 입력이 있으면
			{
				if     (KEY.Key == 1 && Room_Number < 100) {Room_Number++; KEY.Key = 0;}	// 업  버튼 이면 갯수 설정값 증가
				else if(KEY.Key == 3 && Room_Number > 1)   {Room_Number--; KEY.Key = 0;}	// 다운버튼 이면 갯수 설정값 감소
				else if(KEY.Key == 2)			   {cnt++; SET.Save = 0; KEY.Key = 0;}	// 선택버튼 이면 SET.Room 초기화
			}

			if(Blink) {FND_Value_Data(1,1,12);}			// Blink = 1 이면 FND 1 OFF
			else							// Blink = 0 이면 FND 1 에 방 번호 표시
			{
				DIGIT[1][3] = FND_OFF;				// FND 1 에 방 번호 표시
				DIGIT[1][2] = fnd_tbl[Room_Number / 100];
				DIGIT[1][1] = fnd_tbl[Room_Number % 100 / 10];
				DIGIT[1][0] = fnd_tbl[Room_Number % 10];
			}
			DIGIT[2][3] = FND_OFF;					// FND 2 에 스톨 번호 표시
			DIGIT[2][2] = fnd_tbl[Stal_Number / 100];
			DIGIT[2][1] = fnd_tbl[Stal_Number % 100 / 10];
			DIGIT[2][0] = fnd_tbl[Stal_Number % 10];
		}
		else if(cnt == 1)
		{
			if(KEY.Buff)						// Key 입력이 있으면
			{
				if     (KEY.Key == 1 && Stal_Number < 100) {Stal_Number++; KEY.Key = 0;}	// 업  버튼 이면 갯수 설정값 증가
				else if(KEY.Key == 3 && Stal_Number > 1)   {Stal_Number--; KEY.Key = 0;}	// 다운버튼 이면 갯수 설정값 감소
				else if(KEY.Key == 2)			   {cnt++; SET.Save = 0; KEY.Key = 0;}	// 선택버튼 이면 SET.Room 초기화
			}

			if(Blink) {FND_Value_Data(2,2,12);}			// Blink = 1 이면 FND 2 OFF
			else							// Blink = 0 이면 FND 2에 스톨 번호 표시
			{
				DIGIT[2][3] = FND_OFF;				// FND 2 에 스톨 번호 표시
				DIGIT[2][2] = fnd_tbl[Stal_Number / 100];
				DIGIT[2][1] = fnd_tbl[Stal_Number % 100 / 10];
				DIGIT[2][0] = fnd_tbl[Stal_Number % 10];
			}
			DIGIT[1][3] = FND_OFF;					// FND 1 에 방 번호 표시
			DIGIT[1][2] = fnd_tbl[Room_Number / 100];
			DIGIT[1][1] = fnd_tbl[Room_Number % 100 / 10];
			DIGIT[1][0] = fnd_tbl[Room_Number % 10];
		}
		wdt_reset();							// 워치독 리셋
	}
	SET.NumSET = 0;
	ee_write_byte(Cell_ROOM_Addr, Room_Number);
	ee_write_byte(Cell_STAL_Addr, Stal_Number);
	Alarm();
}

//=================================================================================================
void Room_Desplay(void)								// 방 번호 표시
{
	DIGIT[1][3] = FND_OFF;							// FND 1 에 방 번호 표시
	DIGIT[1][2] = fnd_tbl[Room_Number / 100];
	DIGIT[1][1] = fnd_tbl[Room_Number % 100 / 10];
	DIGIT[1][0] = fnd_tbl[Room_Number % 10];
}
//-------------------------------------------------------------------------------------------------
void Stal_Desplay(void)								// 스톨 번호 표시
{
	DIGIT[2][3] = FND_OFF;							// FND 2 에 스톨 번호 표시
	DIGIT[2][2] = fnd_tbl[Stal_Number / 100];
	DIGIT[2][1] = fnd_tbl[Stal_Number % 100 / 10];
	DIGIT[2][0] = fnd_tbl[Stal_Number % 10];
}

//=================================================================================================
void Check_Value(void)								// 로드셀 체중측정
{
	uint32_t Value = Read_Load_Cell();					// 로드셀 측정값 읽기
	uint32_t Old_Value = CELL.Old_Value;					// 로드셀 Old_Value 값
//-------------------------------------------------------------------------------------------------
	if(Value)								// 측정체중이 있으면
	{
		if((Value <= Old_Value + Admit_Weight) & (Value >= Old_Value - Admit_Weight))
		{								// 이전체중이 측정체중과 인증체중(50x10g)을 더한값과 같거나 작고 인증체중(50x10g)을 뺀값과 같거나 크면서 이전체중과 같으면
			Sec_Count = 0;
			OK_Count++;						// 측정체중 OK Count 증가
			if(OK_Count > 3)					// 측정체중 OK Count 가 3보다 크면
			{
				FND_Value_Data(1, 1, 10);			// FND 1 FND_MINUS 표시

				if((SET.Save || Auto_Mode) && !CELL.Zero_Buff)	// 저장 Key 가 눌렸거나 자동모드 이면서 CELL.Zero_Buff 가 0이면
				{

					if(EepCnt < 35) {EepCnt++;}		// EEPROM 저장횟수 가 0 ~ 34(35개)보다 작으면 저장횟수 증가
					else		{EepCnt=0;}		// 아니면 EEPROM 저장횟수 초기화

					if(Data_Number < 299) {Data_Number++;}		// 저장횟수 가 0 ~ 299(300개)보다 작으면 저장횟수 증가
					else		      {Data_Number=0; EepCnt=0;}// 아니면 저장횟수 초기화

//-------------------------------------------------------------------------------------------------
					SAVE.AutoSet[Data_Number] = Auto_Mode;
					SAVE.Count  [Data_Number] = Data_Number;
					SAVE.Data   [Data_Number] = CELL.Value = Value + Interval;
					SAVE.RoomNum[Data_Number] = Room_Number;
					SAVE.StalNum[Data_Number] = Stal_Number;
					SAVE.Year   [Data_Number] = RTC_Single_Read(RTC_YEAR);		// YEAR
					SAVE.Month  [Data_Number] = RTC_Single_Read(RTC_MONTH);		// MONTH
					SAVE.Date   [Data_Number] = RTC_Single_Read(RTC_DATE);		// DATE
					SAVE.Hour   [Data_Number] = RTC_Single_Read(RTC_HOUR);		// HOUR
					SAVE.Min    [Data_Number] = RTC_Single_Read(RTC_MINUTE);	// MINUTE
					SAVE.Sec    [Data_Number] = RTC_Single_Read(RTC_SECOND);	// SECOND
//-------------------------------------------------------------------------------------------------
					ee_write_byte(MEASURE_AUTO   + (MEASURE_LENGTH * EepCnt), Auto_Mode);
					ee_write_word(MEASURE_COUNT  + (MEASURE_LENGTH * EepCnt), Data_Number);
					ee_write_word(MEASURE_WEIGHT + (MEASURE_LENGTH * EepCnt), CELL.Value = Value + Interval);
					ee_write_byte(ROOM_NUMBER    + (MEASURE_LENGTH * EepCnt), Room_Number);
					ee_write_byte(STALL_NUMBER   + (MEASURE_LENGTH * EepCnt), Stal_Number);
					ee_write_byte(MEASURE_YEAR   + (MEASURE_LENGTH * EepCnt), RTC_Single_Read(RTC_YEAR));
					ee_write_byte(MEASURE_MONTH  + (MEASURE_LENGTH * EepCnt), RTC_Single_Read(RTC_MONTH));
					ee_write_byte(MEASURE_DATE   + (MEASURE_LENGTH * EepCnt), RTC_Single_Read(RTC_DATE));
					ee_write_byte(MEASURE_HOUR   + (MEASURE_LENGTH * EepCnt), RTC_Single_Read(RTC_HOUR));
					ee_write_byte(MEASURE_MIN    + (MEASURE_LENGTH * EepCnt), RTC_Single_Read(RTC_MINUTE));
					ee_write_byte(MEASURE_SEC    + (MEASURE_LENGTH * EepCnt), RTC_Single_Read(RTC_SECOND));
//-------------------------------------------------------------------------------------------------
					CELL.Zero_Buff = 1;
					SET.Save = 0;
					Alarm();
				}
				OK_Count = 0;					// 측정체중 OK Count 초기화
				Stal_Desplay();					// 스톨 번호 표시
				printf("Value %ld \r\n", Value + Interval);
			}
			CELL.Old_Value = Old_Value = Value;
			CELL.Value = Value + Interval;
			Cell_Desplay();
		}
		else	// 이전체중이 측정체중과 인증체중(50x10g)을 더한값과 같거나 작고 인증체중(50x10g)을 뺀값과 같거나 크면서 이전체중과 같지 않으면
		{
			CELL.Old_Value = Old_Value = Value;			// 이전체중에 측정체중 저장
			CELL.Value = Value + Interval;
			Zero_Count = 0;
			OK_Count = 0;						// 측정체중 OK Count 초기화
			Room_Desplay();						// 방 번호 표시
			Cell_Desplay();						// 로드셀 무게 표시
		}
	}
	else									// 측정체중이 없면
	{
		Zero_Count++;							// Zero_Count 증가
		if(Zero_Count > 3)						// Zero_Count 이 2보다 크면
		{
			CELL.Old_Value = 0;					// 이전측정체중 초기화
			CELL.Value = Value;
			CELL.Zero_Buff = 0;					// CELL.Zero_Buff 초기화
			Zero_Count = 0;						// Zero_Count 초기화
			OK_Count = 0;						// OK_Count 초기화
	
			if(!SET.Save) {Stal_Desplay();}				// Save 모드가 아니면 스톨 번호 표시
			Room_Desplay();						// 방 번호 표시
			Cell_Desplay();						// 측정체중 표시
		}
	}
}

//=================================================================================================
//		PC로 데이터 전송 (20개 측정체중 Data)
//=================================================================================================
// void Send_To_Server(uint8_t Number, uint8_t id)				// PC로 데이터 전송 (20개 측정체중 Data)
void Send_To_Server_0(void)							// PC로 데이터 전송 (1회 1개씩 전송 : 측정체중 Data)
{
	uint8_t cnt = 0;							// cnt 변수 초기화
	uint8_t ChkSum = 0;							// ChkSum 변수 초기화
	char SerialData[30] = {0};						// SerialData[30] 버퍼 선언
	static uint16_t CommCnt	= 0;

	memset(SerialData, 0x00, sizeof(SerialData));				// 버퍼 초기화

	if(CommCnt < Data_Number) {CommCnt++;}
	else			  {CommCnt=0;}

//-------------------------------------------------------------------------------------------------
//		Serial Protocol Start
//-------------------------------------------------------------------------------------------------
	SerialData[cnt++] = STX_Iontec;						//  0 : STX_Iontec : 0xF0
	SerialData[cnt++] = CTL2PC;						//  1 : 데이터방향
	SerialData[cnt++] = SORTING_MOVE;					//  2 : 21 장비기종 : 이동형 선별기(포유자돈, 비육돈)
	SerialData[cnt++] = Cell_ID;						//  3 : 장비id (Cell_ID)
	SerialData[cnt++] = CD_STATUS;						//  4 : 0 : 구분코드 (실시간상태 정보) 

	SerialData[cnt++] = FW_VER;						//  5 : Protocol Version 41
	SerialData[cnt++] = Gwid;						//  6 : GWID
	SerialData[cnt++] = 0x00;						//  7 : 개체순번
	SerialData[cnt++] = SAVE.AutoSet[CommCnt];				//  8 : 자동 1 / 수동 0
	SerialData[cnt++] = 0x02;						//  9 : 무게 소수점 사용:1, 무게 소수점 미사용:0, 10g단위:2

	SerialData[cnt++] = SAVE.Count[CommCnt] / 256;				// 10 : 측정횟수 상위
	SerialData[cnt++] = SAVE.Count[CommCnt] % 256;				// 11 : 측정횟수 하위
	SerialData[cnt++] = SAVE.Data[CommCnt] / 256;				// 12 : 측정무게 [CommCnt] 상위
	SerialData[cnt++] = SAVE.Data[CommCnt] % 256;				// 13 : 측정무게 [CommCnt] 하위

	SerialData[cnt++] = SAVE.RoomNum[CommCnt];				// 14 : 방 번호
	SerialData[cnt++] = SAVE.StalNum[CommCnt];				// 15 : 스톨 번호

	SerialData[cnt++] = SAVE.Year[CommCnt];					// 16 : 측정 년 [CommCnt]
	SerialData[cnt++] = SAVE.Month[CommCnt];				// 17 : 측정 월 [CommCnt]
	SerialData[cnt++] = SAVE.Date[CommCnt];					// 18 : 측정 일 [CommCnt]
	SerialData[cnt++] = SAVE.Hour[CommCnt];					// 19 : 측정 시 [CommCnt]
	SerialData[cnt++] = SAVE.Min[CommCnt];					// 20 : 측정 분 [CommCnt]
	SerialData[cnt++] = SAVE.Sec[CommCnt];					// 21 : 측정 초 [CommCnt]

	SerialData[cnt++] = CommCnt / 256;					// 22 : Data 순번 상위
	SerialData[cnt++] = CommCnt % 256;					// 23 : Data 순번 하위

	SerialData[cnt++] = 0x00;						// 24 : 예비
	SerialData[cnt++] = 0x00;						// 25 : 예비
	SerialData[cnt++] = 0x00;						// 26 : 예비
	SerialData[cnt++] = 0x00;						// 27 : 예비

	for(uint8_t icnt2 = 1; icnt2 < cnt; icnt2++) {ChkSum += SerialData[icnt2];}	// 체크썸 계산
	SerialData[cnt++] = ChkSum;						// 28 : 체크썸
	SerialData[cnt++] = ETX_Iontec;						// 29 : ETX_Iontec : 0xF1
//-------------------------------------------------------------------------------------------------
//		Serial Protocol End
//-------------------------------------------------------------------------------------------------
	for(uint8_t j = 0; j < cnt; j++) {USART3_Write(SerialData[j]);}		// SerialData 수만큼 송신
}

//=================================================================================================
//		PC로 데이터 전송 (설정정보 답변)
//=================================================================================================
// void Send_To_Server_1(uint8_t ID, uint8_t Code)				// PC로 데이터 전송 (설정정보 답변)
void Send_To_Server_1(uint8_t Code)						// PC로 데이터 전송 (설정정보 답변)
{
	uint8_t cnt = 0;							// cnt 변수 초기화
	uint8_t ChkSum = 0;							// ChkSum 변수 초기화
	char SerialData[10] = {};						// SerialData[10] 버퍼 선언
	memset(SerialData, 0x00, sizeof(SerialData));				// 버퍼 초기화
//-------------------------------------------------------------------------------------------------
//		Serial Protocol Start
//-------------------------------------------------------------------------------------------------
	SerialData[cnt++] = STX_Iontec;						// STX_Iontec : 0xF0			// 0
	SerialData[cnt++] = CTL2PC;						// 데이터방향
	SerialData[cnt++] = SORTING_MOVE;					// 21 장비기종 : 이동형 선별기(포유자돈, 비육돈)
	SerialData[cnt++] = Cell_ID;						// 장비id (Cell_ID)
	SerialData[cnt++] = Code;						// 구분코드

	SerialData[cnt++] = FW_VER;						// Protocol Version			// 5
	SerialData[cnt++] = Gwid;						// GWID
	SerialData[cnt++] = 0x00;						// res
	for(uint8_t icnt2 = 1; icnt2 < cnt; icnt2++) {ChkSum += SerialData[icnt2];}	// 체크썸 계산
	SerialData[cnt++] = ChkSum;											// 8
	SerialData[cnt++] = ETX_Iontec;						// ETX_Iontec : 0xF1			// 9
//-------------------------------------------------------------------------------------------------
//		Serial Protocol End
//-------------------------------------------------------------------------------------------------
	for(uint8_t j = 0; j < cnt; j++) {USART3_Write(SerialData[j]);}		// SerialData 수만큼 송신
}

//=================================================================================================
//		PC로 부터 데이터수신
//=================================================================================================
void Receiv_PC(void)								// PC로 부터 데이터수신
{
	uint8_t cnt1;								// cnt1 변수 초기화
	uint8_t ChkSum = 0;							// ChkSum 변수 초기화

	for(uint8_t cnt = 0; cnt < RX3_SIZE; cnt++)				// RX3_SIZE 만큼 반복
	{
		if(Rx3_Buff[cnt] == STX_Iontec && Rx3_Buff[cnt + 1] == PC2CTL && Rx3_Buff[cnt + 2] == 0x01 && Rx3_Buff[cnt + 4] == 1 && Rx3_Buff[cnt + 9] == ETX_Iontec)
		{//			STX			       PC2CTL			     선별기			실시간정보			ETX
			Rx_LED_ON;						// RX LED ON
			ChkSum = 0;						// ChkSum 초기화

			for(cnt1 = 1; cnt1 < 8; cnt1++) {ChkSum += Rx3_Buff[cnt + cnt1];}	// 체크썸 계산

			if(ChkSum == Rx3_Buff[cnt + 8])				// 체크썸 확인
			{
				if(Rx3_Buff[cnt + 3] == Cell_ID)		// ID 1번 이면
				{						// 실시간 정보
					Tx_LED_ON;				// TX LED ON
					Send_To_Server_0();			// 무게 Data PC로 전송
					_delay_ms(2);
					Tx_LED_OFF;				// TX LED OFF
				}
			}
			Rx3_Buff_Clear();					// Rx3 Buff Clear
			Rx_LED_OFF;						// RX LED OFF
			break;							// for 문 빠져 나가기
		}

		else if(Rx3_Buff[cnt] == STX_Iontec && Rx3_Buff[cnt + 1] == PC2CTL && Rx3_Buff[cnt + 4] == 9 && Rx3_Buff[cnt + 15] == ETX_Iontec)
		{//			    STX				    PC2CTL		    시간정보변경			        ETX
			Rx_LED_ON;						// RX LED ON
			ChkSum = 0;						// ChkSum 초기화

			for(cnt1 = 1; cnt1 < 14; cnt1++) {ChkSum += Rx3_Buff[cnt + cnt1];}	// 체크썸 계산

			if(ChkSum == Rx3_Buff[cnt + 14])			// 체크썸 확인
			{
				RTC_Time_Write(RTC_YEAR,  ((Rx3_Buff[cnt+ 8]&0xF0)>>4)*10+(Rx3_Buff[cnt+ 8]&0x0F));	// RTC 에 년 저장
				RTC_Time_Write(RTC_MONTH, ((Rx3_Buff[cnt+ 9]&0xF0)>>4)*10+(Rx3_Buff[cnt+ 9]&0x0F));	// RTC 에 월 저장
				RTC_Time_Write(RTC_DATE,  ((Rx3_Buff[cnt+10]&0xF0)>>4)*10+(Rx3_Buff[cnt+10]&0x0F));	// RTC 에 일 저장
				RTC_Time_Write(RTC_HOUR,  ((Rx3_Buff[cnt+11]&0xF0)>>4)*10+(Rx3_Buff[cnt+11]&0x0F));	// RTC 에 시 저장
				RTC_Time_Write(RTC_MINUTE,((Rx3_Buff[cnt+12]&0xF0)>>4)*10+(Rx3_Buff[cnt+12]&0x0F));	// RTC 에 분 저장
				RTC_Time_Write(RTC_SECOND,((Rx3_Buff[cnt+13]&0xF0)>>4)*10+(Rx3_Buff[cnt+13]&0x0F));	// RTC 에 초 저장
			}
			Rx3_Buff_Clear();					// Rx3 Buff Clear
			Rx_LED_OFF;						// RX LED OFF
			break;							// for 문 빠져 나가기
		}

		else if(Rx3_Buff[cnt] == STX_Iontec && Rx3_Buff[cnt + 1] == PC2CTL && Rx3_Buff[cnt + 2] == 0x01 && Rx3_Buff[cnt + 4] == 7 && Rx3_Buff[cnt + 31] == ETX_Iontec)
		{//			     STX			    PC2CTL			  선별기		설정정보(입구체중)		       ETX
			Rx_LED_ON;							// RX LED ON
			ChkSum = 0;							// ChkSum 초기화

			for(cnt1 = 1; cnt1 < 30; cnt1++) {ChkSum += Rx3_Buff[cnt + cnt1];}	// 체크썸 계산

			if(ChkSum == Rx3_Buff[cnt + 30])				// 체크썸 확인
			{
				IN_S = Rx3_Buff[cnt + 8] << 8 & Rx3_Buff[cnt + 8];	// 입구체중 값 변환

				if(Rx3_Buff[cnt + 3] == Cell_ID)			// Cell ID 1번 이면
				{
					IN_Scales = IN_S;				// 입구체중 값을 로드셀 1번에 적용
					ee_write_word(Cell_IN_S_Addr, IN_S);		// 입구체중 값을 EEPROM에 저장
					Tx_LED_ON;					// TX LED ON
					Send_To_Server_1(Rx3_Buff[cnt + 4]);		// 설정정보 PC로 전송
					_delay_ms(2);
					Tx_LED_OFF;					// TX LED OFF
				}
			}
		}
	}
}

//=================================================================================================
void Time_Check(void)								// 시간 체크
{
	if(Timer.RTC_Timer >= 1000)						// 5mSec X 1000 = 5Sec 이상이 되면 진입
	{
		Timer.RTC_Timer = 0;						// RTC_Timer 초기화 (타이머1 에서 5mSec 마다 증가)
		RTC_Time_Read();						// RTC에서 시간 읽어 오기

		if(RTC_T.OLD_D != RTC_T.Date)					// 날짜 Data 가 다르면
		{
			RTC_T.OLD_D = RTC_T.Date;				// 현재 날짜 저장
			ee_write_byte (OLD_Date_Addr, RTC_T.OLD_D);		// 날짜 를 EEPROM 에 저장

			memset(SAVE.AutoSet, 0, 300);				// 자동설정 버퍼 Clear
			memset(SAVE.Count,   0, 300);				// 측정횟수 버퍼 Clear
			memset(SAVE.Data,    0, 300);				// 측정무게 버퍼 Clear
			memset(SAVE.RoomNum, 0, 300);				// 방번호 버퍼 Clear
			memset(SAVE.StalNum, 0, 300);				// 스톨번호 버퍼 Clear
			memset(SAVE.Year,    0, 300);				// 년 버퍼 Clear
			memset(SAVE.Month,   0, 300);				// 월 버퍼 Clear
			memset(SAVE.Date,    0, 300);				// 일 버퍼 Clear
			memset(SAVE.Hour,    0, 300);				// 시 버퍼 Clear
			memset(SAVE.Min,     0, 300);				// 분 버퍼 Clear
			memset(SAVE.Sec,     0, 300);				// 초 버퍼 Clear

			Data_Number = 0;					// 측정 무게 Count Clear
		}
		Sec_Count++;							// Sec_Count 증가
		if(Sec_Count > 10) {Sec_Count = 10;}				// Sec_Count 가 20 보다 크면 20 으로 유지
	}
}

//=================================================================================================
void ADC_Check(void)								// ADC 체크 (로드셀 신호)
{
	AD_Channel = 0;								// AD Channel 변수 초기화
	for(uint8_t i = 0; i < 50; i++)						// 50번 반복 측정
	{
		AD_Channel += ADC_get_conversion(0);				// ADC 0 Conversion
	}
	AD_Channel = AD_Channel / 50;						// 50번 측정 후 평균
	ADC_Value  = AD_Channel * 0.001221;					// 5V 나누기 12Bit : 5/4095 = 0.00122100...
}

//=================================================================================================
void Proportion_Set(void)							// 로드셀 비율 설정 및 초기화
{
	CELL.Proportion = (Cell_RO / 10000000 / (ADC_Value + 0.48) * 128 / 0.5 * 8388608 / (Cell_Capa * 1000)) / Cell_CNT;
}

//=================================================================================================
void Var_Init(void)								// 변수 및 로드셀 초기화
{
	FND_Value_Data(0, 2, 10);						// FND 0 ~ 2 FND_MINUS

	SET.Menu = 0;								// Menu   모드 초기화
	SET.Select = 0;								// Select 모드 초기화
	SET.Blink = 0;								// 자릿수 깜박임 초기화
	KEY.Count = 0;								// Key 가 눌린횟수 초기화

	Read_eeprom();								// EEPROM Data 읽기
	_delay_ms(10);

	ADC_Check();								// ADC 체크 (로드셀 신호)
	Proportion_Set();							// 로드셀 비율 설정 및 초기화
	_delay_ms(10);

	Cell_Zero_Set();							// 로드셀 0 값 찾기 2회 반복
	_delay_ms(10);
	Cell_Zero_Set();							
	Alarm();								// 알람(비프 3번)

	CELL.Zero_Buff = 0;							// 측정 값 Zero Buff 초기화
	Sec_Count = 0;								// 일정시간(30초) 버튼 입력 없으면 FND OFF Timer 초기화
	
	FND_Value_Data(0, 2, 12);						// FND 0 ~ 2 FND_MINUS
}

//=================================================================================================
int main(void)
{
	atmel_start_init();
	MCU_Init();								// MCU Port 초기화
//-------------------------------------------------------------------------------------------------
#ifdef USART_SET								// USART 사용설정
	fdevopen(std_putchar, NULL);						// printf 함수사용 설정
	printf_port = 1;							// Usart Port 1 사용
	_delay_ms(50);								// 통신 Port 안정화 Time

	printf("\r\n");
	printf("//================================================//\r\n");
	printf("//===  Product Name : Pig Scales V4.1(SC-500)  ===//\r\n");	// ProductName
	printf("//===  File Content : Moving Weight Scales     ===//\r\n");	// File 내용 : 이동형 체중 측정기 
	printf("//===  Compiled     : %s, %s    ===//\r\n", __DATE__, __TIME__);// Compiled : 날짜, 시간 표시
	printf("//================================================//\r\n");
	printf("\r\n");
#endif
//-------------------------------------------------------------------------------------------------
#ifdef ADC_SET									// ADC 사용설정
	ADC_init();								// ADC 초기화 및 설정
	VREF_ADC_init();
	ADC_is_conversion_done();
#endif
//-------------------------------------------------------------------------------------------------
#ifdef PWM_SET									// PWM 사용설정
	PWM_0_enable();								// PWM 0 초기화 및 설정
	PWM_0_enable_output_ch0();
#endif
//-------------------------------------------------------------------------------------------------
	sei();									// 전체 인터럽터 시작
	Alarm();								// 알람(비프 3번)
	Rx_LED_OFF;								// RX LED OFF
	Tx_LED_OFF;								// TX LED OFF
//-------------------------------------------------------------------------------------------------
	DIGIT[0][3] = FND_OFF;							// Ver 3.1 표시
	DIGIT[0][2] = FND_4 + FND_Point;					
	DIGIT[0][1] = FND_1;						
	DIGIT[0][0] = FND_OFF;
	_delay_ms(1000);							// 1초 동안 표시	
	wdt_reset();								// Reset WT

	Time_Count = 5;								// 시간 표시 용 변수 설정
	Sec_Count = 0;
	SECOND = 0;								// 이전 시간(초)용 변수 설정
//-------------------------------------------------------------------------------------------------
	while(Time_Count)							// Time_Count 가 0이 아니면 반복(5초동안 시간표시)
	{
		RTC_Time_Read();						// RTC Data 읽기

		DIGIT[0][3] = fnd_tbl[RTC_T.Year / 10];				// 년 표시
		DIGIT[0][2] = fnd_tbl[RTC_T.Year % 10];

		DIGIT[0][1] = fnd_tbl[RTC_T.Month / 10];			// 월 표시
		DIGIT[0][0] = fnd_tbl[RTC_T.Month % 10];

		DIGIT[1][3] = fnd_tbl[RTC_T.Date / 10];				// 일 표시
		DIGIT[1][2] = fnd_tbl[RTC_T.Date % 10];

		DIGIT[1][1] = fnd_tbl[RTC_T.Hour / 10];				// 시 표시
		DIGIT[1][0] = fnd_tbl[RTC_T.Hour % 10];

		DIGIT[2][3] = fnd_tbl[RTC_T.Min / 10];				// 분 표시
		DIGIT[2][2] = fnd_tbl[RTC_T.Min % 10];

		DIGIT[2][1] = fnd_tbl[RTC_T.Sec / 10];				// 초 표시
		DIGIT[2][0] = fnd_tbl[RTC_T.Sec % 10];
		
		if(SECOND != RTC_T.Sec)						// 이전 시간(초)이 현재시간(초)와 같지 않으면
		{
			SECOND = RTC_T.Sec;					// 이전 시간(초)에 현재시간(초) 저장	
			Beep(10);						// 비프(1초에 1번씩)
			wdt_reset();						// Reset WDT
			Time_Count--;						// Time_Count 감소
		}
	}
	Var_Init();								// 변수 및 로드셀 초기화
	wdt_reset();								// Reset WDT
	
	for(uint8_t i = 0; i > 35; i++)
	{
		Read_eeprom_Weight_Data(i);
		Data_Number++;
	}

//-------------------------------------------------------------------------------------------------
	while(1)
	{
		if(Sec_Count >= 6) {FND_ALL_OFF();}				// 5Sec x 6 = 30Sec 보다 크면 FND ALLOFF
		else		   {Check_Value();}

		if(SET.Menu)   {Seting_Mode_0();}				// SET Menu   가 설정되어 있으면 Seting_Mode_0 진입
		if(SET.NumSET) {Seting_Mode_1();}				// SET.NumSET 가 설정되어 있으면 Seting_Mode_1 진입
		Receiv_PC();							// PC로 부터 데이터수신 확인
		Time_Check();							// 시간 체크(날짜변경 확인)
		_delay_ms(25);							// 연속 동작을 막기위한 Delay
		wdt_reset();							// Reset WDT
	}
}

//=================================================================================================
#ifdef USART_SET
int std_putchar(char c,FILE *stream)						// printf data 를 설정한 USART Port 로 전송
{
	#ifdef Usart_0
		if(printf_port == 0) {USART0_Write(c);}
	#endif

	#ifdef Usart_1
		if(printf_port == 1) {USART1_Write(c);}
	#endif

	#ifdef Usart_2
		if(printf_port == 2) {USART2_Write(c);}
	#endif

	#ifdef Usart_3
		if(printf_port == 3) {USART3_Write(c);}
	#endif

	#ifdef Usart_4
		if(printf_port == 4) {USART4_Write(c);}
	#endif

	#ifdef Usart_5
		if(printf_port == 5) {USART5_Write(c);}
	#endif

	return 0;
}
#endif

//=================================================================================================
void wdt_reset(void)								// Reset WDT
{
	asm("WDR");								// Reset WDT
}

//=================================================================================================
//		END OF "main.c"
//=================================================================================================
