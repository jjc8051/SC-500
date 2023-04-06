//=================================================================================================
//		ProductName 	:	Pig_Scales V4.1 SC-500
//=================================================================================================
//		File  Name	:	main.c
//		File  내용	:	AVR128DA64 MCU to BLDC Driver IC Board Test (PWM Control)
//		작성  일자	:	2022년 12월 13일
//		수정  일자	:	2022년 12월 16일
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
//	2022-1213	Pig_Scales V1.3을 기본으로 SC-500 펌웨어 이식 작업 시작
//			250Kg 로드셀 4개짜리 1개의 장비만 설정
//
//=================================================================================================#include <main.h>
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
	PORTG_DIR = 0xF7;							// DIGIT3, DIGIT2, DIGIT1, DIGIT0,      x,   DATA,  SHIFT,  LATCH : FND Display
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
	for(uint8_t i = start; i <= end; i++)					// FND 에 start 부터 end 까지 value Data 표시
	{
		DIGIT[i][3] = fnd_tbl[value];					// DIGIT[3] value Data 표시
		DIGIT[i][2] = fnd_tbl[value];					// DIGIT[2] value Data 표시
		DIGIT[i][1] = fnd_tbl[value];					// DIGIT[1] value Data 표시
		DIGIT[i][0] = fnd_tbl[value];					// DIGIT[0] value Data 표시
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
		if(keytemp & 0x10) {Beep(10); KEY.Key = 1; KEY.Buff = 1; Timer.RegExitTimer = 0;}	// +(UP)  key 입력이 있으면 
		if(keytemp & 0x20) {Beep(10); KEY.Key = 2; KEY.Buff = 1; Timer.RegExitTimer = 0;}	// Select key 입력이 있으면
		if(keytemp & 0x40) {Beep(10); KEY.Key = 3; KEY.Buff = 1; Timer.RegExitTimer = 0;}	// -(Down)key 입력이 있으면
		if(keytemp & 0x80) {Beep(10); KEY.Key = 4; KEY.Buff = 1; Timer.RegExitTimer = 0;}	// Menu   key 입력이 있으면

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

		if(SET.Menu && KEY.Key == 2 && SET.Blink)			// 메뉴모드 이고 Blink = 1 이고 선택 Key 입력이 있으면
		{
			KEY.Count++;						// Key 가 눌린횟수 증가
			if(KEY.Count > 5) {KEY.Count = 0;}			// Key 가 눌린횟수가 5보다 크면 0으로 초기화
		}

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
		Gukbun = 1;							// 게이트웨이 국번 값을 1로 초기화
		ee_write_byte(Gateway_Gukbun_Addr, Gukbun);			// 게이트웨이 국번 값을 EEPROM 에 저장
	}
//-------------------------------------------------------------------------------------------------
//		게이트웨이 ID
//-------------------------------------------------------------------------------------------------
	Gwid = ee_read_byte(Gateway_GWID_Addr);					// 게이트웨이 ID 설정값 불러오기
	if(Gwid > 60 || Gwid == 0)						// 게이트웨이 ID 설정값이 60보다 크거나 0과 같으면
	{
		Gwid = 1;							// 게이트웨이 ID 값을 1로 초기화
		ee_write_byte(Gateway_GWID_Addr, Gwid);				// 게이트웨이 ID 값을 EEPROM 에 저장
	}
//-------------------------------------------------------------------------------------------------
//		저장된 날짜 Date
//-------------------------------------------------------------------------------------------------
	RTC_T.OLD_D = ee_read_byte(OLD_Date_Addr);				// 날짜 불러오기
	if(RTC_T.OLD_D > 31 || RTC_T.OLD_D < 0)					// 날짜가 31일보다 크거나 0보다 작으면
	{
		RTC_T.OLD_D = 1;						// 날짜를 1로 초기화
		ee_write_byte(OLD_Date_Addr, RTC_T.OLD_D);			// 날짜를 EEPROM 에 저장
	}
//-------------------------------------------------------------------------------------------------
//		로드셀 ID
//-------------------------------------------------------------------------------------------------
	Cell_ID = ee_read_byte(Cell_ID_Addr);					// 로드셀 ID 설정값 불러오기
	if(Cell_ID > 60 || Cell_ID < 0)						// 로드셀 ID 설정값이 60보다 크거나 0보다 작으면
	{
		Cell_ID = 1;							// 로드셀 ID 값을 1로 초기화
		ee_write_byte(Cell_ID_Addr, Cell_ID);				// 로드셀 ID 값을 EEPROM 에 저장
	}
//-------------------------------------------------------------------------------------------------
//		로드셀 ID
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
		Cell_Capa = 100;						// 로드셀 CAPA 값을 100kg로 초기화, 곱하기 1,000으로 계산 100kg = 100000g
		ee_write_word(Cell_Capa_Addr, Cell_Capa);			// 로드셀 CAPA 값을 EEPROM 에 저장
	}
//-------------------------------------------------------------------------------------------------
//		로드셀 RO (정격출력)
//-------------------------------------------------------------------------------------------------
	Cell_RO = ee_read_dword(Cell_RO_Addr);					// 로드셀 RO 설정값 불러오기
	if(Cell_RO > 90000 || Cell_RO < 10000)					// 로드셀 RO 설정값이 9.000mV보다 크거나 3.000mV보다 작으면
	{
		Cell_RO = 40000;						// 로드셀 RO 값을 4.000mV로 초기화, 나누기 10,000으로 계산
		ee_write_dword(Cell_RO_Addr, Cell_RO);				// 로드셀 RO 값을 EEPROM 에 저장
	}
//-------------------------------------------------------------------------------------------------
//		1번, 2번, 3번 입구 체중
//-------------------------------------------------------------------------------------------------
	IN_Scales = ee_read_word(Cell_IN_S_Addr);				// 입구 체중 설정값 불러오기
	if(IN_Scales > 10000 || IN_Scales < 10)					// 입구 체중 설정값이 10000g(10Kg)보다 크거나 10g보다 작으면
	{
		IN_Scales = 500;						// 입구 체중 값을 500으로 초기화 (500g)
		ee_write_word(Cell_IN_S_Addr, IN_Scales);			// 입구 체중 값을 EEPROM 에 저장
	}
//-------------------------------------------------------------------------------------------------
//		로드셀 보정 값
//-------------------------------------------------------------------------------------------------
	Interval = ee_read_word(Cell_Interval_Addr);				// 로드셀 보정 값 불러오기
	if((Interval > 99) || (Interval < -99))					// 로드셀 보정 값이 99보다 크거나 -99보다 작으면
	{
		Interval = 0;							// 로드셀 보정 값을 0 으로 초기화
		ee_write_word(Cell_Interval_Addr, Interval);			// 로드셀 보정 값을 EEPROM 에 저장
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
	if(RTC_T.Year == 00)							// 년 값이 00 이면
	{
		RTC_Time_Write(RTC_YEAR,      22);				// RTC 에 22년   저장
		RTC_Time_Write(RTC_MONTH,     9);				// RTC 에 09월   저장
		RTC_Time_Write(RTC_DATE,      20);				// RTC 에 20일   저장
		RTC_Time_Write(RTC_DAYOFWEEK, 3);				// RTC 에 03요일 저장
		RTC_Time_Write(RTC_HOUR,      12);				// RTC 에 12시   저장
		RTC_Time_Write(RTC_MINUTE,    0);				// RTC 에 00분   저장
		RTC_Time_Write(RTC_SECOND,    0);				// RTC 에 00초   저장
	}
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
void Seting_Mode(void)								// 셋팅모드
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
				Count = 4;					// 반복을 위해 Count를 최고로 증가 시킴	
			break;

			case 1:							// Count = 1 이면
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

			case 2:							// Count = 2 이면
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

			case 3:							// Count = 3 이면
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

			case 4:							// Count = 5 이면
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

			case 5:							// Count = 5 이면
				Count = 1;					// 반복을 위해 Count를 최저로 감소 시킴
			break;
		}
		wdt_reset();							// 워치독 리셋
	}
	Var_Init();								// 변수 초기화
}

//=================================================================================================
void Check_Value(void)								// 로드셀 체중측정
{
	uint32_t Value = 0;							// Value 변수 초기화
	uint32_t Old_Value = 0;							// Old_Value 변수 초기화
	uint16_t Limit_Time = 0;						// Old_Value 변수 초기화

//-------------------------------------------------------------------------------------------------
	Value = CELL.Value = Read_Load_Cell();					// 로드셀 측정값 읽기
	Old_Value = CELL.Old_Value;						// 로드셀 Old_Value 값
	Zero_Buff = CELL.Zero_Buff;						// 로드셀 Zero_Buff 값
// 	OK_Count = OK_Count_1;							// 로드셀 OK_Count 값
	Limit_Time = Timer.Check_Limit_Timer;					// 로드셀 무게측정 제한 시간
//-------------------------------------------------------------------------------------------------
	if(Value)								// 측정체중이 있으면
	{
		if((Limit_Time >= 500) & ((Value <= Old_Value + Admit_Weight) & (Value >= Old_Value - Admit_Weight)))
		{								// 이전체중이 측정체중과 인증체중(100g)을 더한값과 같거나 작고 인증체중(100g)을 뺀값과 같거나 크면서 이전체중과 같으면
			OK_Count++;						// 측정체중 OK Count 증가
			if(OK_Count > 3)					// 측정체중 OK Count 가 3보다 크면
			{
				if(!Zero_Buff)					// Zero_Buff 가 0 이면
				{
					Zero_Buff = 1;				// Zero_Buff = 1

					CELL.Zero_Buff = Zero_Buff;		// 로드셀 이면 Zero_Buff 저장

					for(uint8_t i = 20; i > 1; i--)		// 20 번 부터 1번까지 20번 반복
					{
						Data_1[i] = Data_1[i-1];	// 로드셀 이면 체중 Data_1 1칸씩 뒤로 밀기
					}

					Data_1[1] = CELL.Value = CELL.Value + Interval;
					CELL.Count++;
					CELL.Zero_Buff = 0;
					Timer.Check_Limit_Timer = 0;
					Cell_Desplay();
					Alarm();
				}
				OK_Count = 0;					// 측정체중 OK Count 초기화
				printf("Value %ld \r\n", Value + Interval);
			}
// 			OK_Count_1 = OK_Count;
			CELL.Old_Value = Old_Value;
			CELL.Value = CELL.Value + Interval;
			Cell_Desplay();
		}
		else	// 이전체중이 측정체중과 인증체중(50)을 더한값과 같거나 작고 인증체중(50)을 뺀값과 같거나 크면서 이전체중과 같으면 이 아니면
		{
			Old_Value = Value;					// 이전체중에 측정체중 저장
			CELL.Old_Value = Old_Value;
			CELL.Value = CELL.Value + Interval;
			Zero_Count = 0;
			Cell_Desplay();
		}
	}
	else 									// 측정체중이 없면
	{
		Zero_Count++;							// Zero_Count 증가
		if(Zero_Count > 2)						// Zero_Count 이 2보다 크면
		{
			CELL.Zero_Buff = 0;					// CELL.Zero_Buff 초기화
			Zero_Count = 0;						// Zero_Count 초기화
			CELL.Old_Value = 0;					// 이전측정체중 초기화
			OK_Count = 0;						// OK_Count 초기화
			Cell_Desplay();						// 측정체중 표시
		}
	}
}

//=================================================================================================
//		PC로 데이터 전송 (20개 측정체중 Data)
//=================================================================================================
void Send_To_Server(uint8_t Number, uint8_t id)					// PC로 데이터 전송 (20개 측정체중 Data)
{
	uint8_t cnt = 0;							// cnt 변수 초기화
	uint8_t ChkSum = 0;							// ChkSum 변수 초기화
	uint16_t Count= 0;							// Count 변수 초기화
	char SerialData[102] = {};						// SerialData[102] 버퍼 선언
	memset(SerialData, 0x00, sizeof(SerialData));				// 버퍼 초기화
//-------------------------------------------------------------------------------------------------
//		Serial Protocol Start
//-------------------------------------------------------------------------------------------------
	SerialData[cnt++] = STX_Iontec;						// STX_Iontec : 0xF0	// 0
	SerialData[cnt++] = CTL2PC;						// 데이터방향
	SerialData[cnt++] = 0x01;						// 장비기종 : 자돈 무게 측정기
	SerialData[cnt++] = id;							// 장비id
	SerialData[cnt++] = 0x00;						// 구분코드
	SerialData[cnt++] = FW_VER;						// Protocol Version	// 5
	SerialData[cnt++] = Gwid;						// GWID
	SerialData[cnt++] = 0x00;						// 개체순번
	SerialData[cnt++] = 0x00;						// 자동 1 / 수동 0
	SerialData[cnt++] = 0x02;						// 무게 소수점 사용:1, 무게 소수점 미사용:0, 10g단위:2
	for(uint8_t i = 1; i <= 20; i++)					// 1부터 20까지 20번 반복
	{
		if(Number == 1)							// 1번 로드셀 이면
		{
			if     (CELL.Count >= 20) {Count = CELL.Count + i;}	// Count 가 20 보다 크거나 같으면 Count를 i 만큼 증가 하고
			else if(CELL.Count >= i)  {Count = i;}			// Count 가  i 보다 크거나 같으면 Count에 i 를널고
			else if(CELL.Count < i)   {Count = 0;}			// Count 가  i 보다 작으면 0 으로 초기화
			SerialData[cnt++] = Count / 256;			// 측정횟수 i 상위	// 10 ~ 89
			SerialData[cnt++] = Count % 256;			// 측정횟수 i 하위
			SerialData[cnt++] = Data_1[i] / 256;			// 측정무게 i 상위
			SerialData[cnt++] = Data_1[i] % 256;			// 측정무게 i 하위
		}
	}
	SerialData[cnt++] = 0x00;						// 예비			// 90
	SerialData[cnt++] = 0x00;						// 예비
	SerialData[cnt++] = 0x00;						// 예비
	SerialData[cnt++] = 0x00;						// 예비
	SerialData[cnt++] = 0x00;						// 예비
	SerialData[cnt++] = 0x00;						// 예비			// 95
	for(uint8_t icnt2 = 1; icnt2 < cnt; icnt2++) {ChkSum += SerialData[icnt2];}	// 체크썸 계산
	SerialData[cnt++] = ChkSum;						// 체크썸		// 96
	SerialData[cnt++] = ETX_Iontec;						// ETX_Iontec : 0xF1	// 97
//-------------------------------------------------------------------------------------------------
//		Serial Protocol End
//-------------------------------------------------------------------------------------------------
	for(uint8_t j = 0; j < cnt; j++) {USART3_Write(SerialData[j]);}		// SerialData 수만큼 송신
}

//=================================================================================================
//		PC로 데이터 전송 (설정정보 답변)
//=================================================================================================
void Send_To_Server_1(uint8_t ID, uint8_t Code)					// PC로 데이터 전송 (설정정보 답변)
{
	uint8_t cnt = 0;							// cnt 변수 초기화
	uint8_t ChkSum = 0;							// ChkSum 변수 초기화
	char SerialData[10] = {};						// SerialData[10] 버퍼 선언
	memset(SerialData, 0x00, sizeof(SerialData));				// 버퍼 초기화
//-------------------------------------------------------------------------------------------------
//		Serial Protocol Start
//-------------------------------------------------------------------------------------------------
	SerialData[cnt++] = STX_Iontec;						// STX_Iontec : 0xF0	// 0
	SerialData[cnt++] = CTL2PC;						// 데이터방향
	SerialData[cnt++] = 0x01;						// 장비기종 : 자돈 무게 측정기
	SerialData[cnt++] = ID;							// 장비id
	SerialData[cnt++] = Code;						// 구분코드
	SerialData[cnt++] = FW_VER;						// Protocol Version	// 5
	SerialData[cnt++] = Gwid;						// GWID
	SerialData[cnt++] = 0x00;						// res
	for(uint8_t icnt2 = 1; icnt2 < cnt; icnt2++) {ChkSum += SerialData[icnt2];}	// 체크썸 계산
	SerialData[cnt++] = ChkSum;									// 8
	SerialData[cnt++] = ETX_Iontec;						// ETX_Iontec : 0xF1	// 9
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
					Send_To_Server(1, Cell_ID);		// 무게 Data PC로 전송
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
					Send_To_Server_1(Cell_ID, Rx3_Buff[cnt + 4]);	// 설정정보 PC로 전송
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
			RTC_T.OLD_D = RTC_T.Date;					// 현제 날짜 저장
			ee_write_byte (OLD_Date_Addr, RTC_T.OLD_D);		// 날짜 를 EEPROM 에 저장

			memset(Data_1, 0, 40);					// 측정 무게 Data  Clear
			CELL.Count = 0;						// 측정 무게 Count Clear
		}
	}
}

//=================================================================================================
void ADC_Check(void)								// ADC 체크 (로드셀 신호)
{
	AD_Channel_0 = 0;							// AD Channel 0 변수 초기화
	AD_Channel_1 = 0;							// AD Channel 1 변수 초기화
	AD_Channel_2 = 0;							// AD Channel 3 변수 초기화

	for(uint8_t i = 0; i < 50; i++)						// 50번 반복 측정
	{
		AD_Channel_0 += ADC_get_conversion(0);				// ADC 0 Conversion
		AD_Channel_1 += ADC_get_conversion(1);				// ADC 1 Conversion
		AD_Channel_2 += ADC_get_conversion(2);				// ADC 2 Conversion
	}

	AD_Channel_0 = AD_Channel_0 / 50;					// 50번 측정 후 평균
	AD_Channel_1 = AD_Channel_1 / 50;					// 50번 측정 후 평균
	AD_Channel_2 = AD_Channel_2 / 50;					// 50번 측정 후 평균

	ADC_Value_0 = AD_Channel_0 * 0.001221;					// 5V 나누기 12Bit : 5/4095 = 0.00122100...
	ADC_Value_1 = AD_Channel_1 * 0.001221;					// 5V 나누기 12Bit : 5/4095 = 0.00122100...
	ADC_Value_2 = AD_Channel_2 * 0.001221;					// 5V 나누기 12Bit : 5/4095 = 0.00122100...
}

//=================================================================================================
void Proportion_Set(void)							// 로드셀 비율 설정 및 초기화
{
	CELL.Proportion = (Cell_RO / 10000000 / (ADC_Value_0 + 0.48) * 128 / 0.5 * 8388608 / (Cell_Capa * 1000)) / Cell_CNT;
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

	for(uint8_t i = 0; i < 2; i++)						// 로드셀 0 값 찾기 : 2회 반복(튀는 값에 의한 Err 방지)
	{
		Cell_Zero_Set(1);
		Cell_Zero_Set(2);
		Cell_Zero_Set(3);
		_delay_ms(50);
	}
	Alarm();								// 알람(비프 3번)

	CELL.Zero_Buff = 0;							// 측정 값 Zero Buff 초기화

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
	printf("\r\n");
	_delay_ms(50);
	printf("---   Compiled : %s, %s   ---\r\n\r\n", __DATE__, __TIME__);	// Compiled : 날짜, 시간 표시
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
	uint8_t Time_Count = 1;							// 시간 표시 용 변수 설정
	uint8_t SECOND = 0;							// 이전 시간(초)용 변수 설정
	Read_eeprom();
//-------------------------------------------------------------------------------------------------
	DIGIT[0][3] = FND_OFF;							// Ver 3.1 표시
	DIGIT[0][2] = FND_4 + FND_Point;					
	DIGIT[0][1] = FND_1;						
	DIGIT[0][0] = FND_OFF;
	_delay_ms(1000);							// 1초 동안 표시	
	wdt_reset();								// Reset WT
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
//-------------------------------------------------------------------------------------------------
	while(1)
	{
		if(SET.Menu) {Seting_Mode();}					// SET Menu 가 설정되어 있으면 Seting_Mode 진입

		Check_Value();

		Receiv_PC();							// PC로 부터 데이터수신 확인

		Time_Check();							// 시간 체크(날짜변경 확인)

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
