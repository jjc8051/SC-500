//=================================================================================================
//		ProductName 	:	Pig_Scales
//=================================================================================================
//		File  Name	:	main.c
//		File  내용	:	AVR128DA64 MCU to BLDC Driver IC Board Test (PWM Control)
//		작성  일자	:	2022년 07월 10일
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
//	2022-0710	AVR128DA64 CPU 기본 설정 작업 시작
//			Fuses Set 정의
//			printf 소숫점 표시 처리
//
//	2022-0727	FND Display Test 완료
//			ZigBee Set Test 완료
//			GateWay Set Test 완료
//
//	2022-0729	로드셀 연결 Test 시작
//			입력 값이 너무 튀는 현상 발생
//			HX711 작동 타이밍이 중요
//
//	2022-0730	로드셀 튀는 Data 걸러네는 로직 구성 Test
//			최종출력 값 FND 에 Display
//
//	2022-0805	RTC 시간 적용 일차 변경시 측정횟수 초기화
//			RTC 설정 후 빠져나오질 못하고 워치독으로 리셋되는 현상발생
//			주기 적으로 RTC 를 읽어 들이면서 빠져 나오지 못하는 현상이 발생하는 것으로 판다
//			설정메뉴에 들어갈때 측정횟수를 EEPROM 에 저장 리셋 될때 읽어 와서 문제 해결
//
//	2022-0810	인디게이터 를 이용한 샘플 무게 측정기 펌웨어를 완료 후 이를 기본으로 하여
//			HX711 을 사용 무게를 측정하는 펌웨어를 제 작업 진행
//
//	2022-0811	HX711 제어 타이밍 조절 완료 (SCK 는 20uSec Delay 로 작동하고, 마지막에 150mSec Delay)
//			15번 Pin(RATE) 는 Hi 로 처리
//			INNA, INPA의 단자 저항은 0 오옴사용, 컨덴서 는 10uF 사용
//
//			배율=((RO_1+RO_2)÷(입력전압+0.5)×게인(128)÷0.5×2^23(8388067)÷((Capa1+Capa1)×1000)÷10000000)÷2)
//			마지막 나누기 2는 2개의 로드셀을 사용하여서 나누는것 임
//
//			입력 값을 배율로 나누어 출력되는 값에서 초기에 나오는 값을 빼주거나 더해주어,
//			로드셀 입력 값을 0으로 만들어 주는 작업 완료
//			3개의모듈 모두 정상 적인 값이 나오는것 확인
//
//			로드셀 셋팅 로직 Test 완료
//			케이트 웨이 셋팅 로직 Test 완료
//			지그비 셋팅 로직 Test 완료
//			입구체중 셋팅 로직 Test 완료
//			RTC 셋팅 로직 Test 완료
//			통신 프로토콜 Test 완료
//
//	2022-0812	각 로드셀의 ID를 개별 설정하고 이를 연동하여 응답하도록 수정
//			ID가 없는 로드셀은 OFF 되도록 처리(OFF 시 ID가 없으므로 통신 응답도 하지 않음)
//			1개~ 3개까지 순차 적으로 각 로드셀을 연결 Test 완료
//			통신 Data 가 정상적으로 올라가는지 확인
//			불필요 하거나 잘못된 곳 정리
//
//	2022-0816	RTC 최초 구동시 정상 시간이 아니면 초기 Data 저장 (22년 09월 20일 12시 00분 00초)
//			RTC Data 를 읽어서 년,월,일,시,분,초 Data 에 이상이 있으면 RTC를 초기화 하도록 처리
//
//	2022-0817	CPU 출력 PORT 제어 명령어 (cbi(), sbi()) sfr 수정
//
//	2022-0818	기본 제어 모듈별 정리
//
//	2022-0910	RTC 설정 후 저장이 1개씩 밖에 되지 않고 있어서 1개를 설정 후 Rest 하고 다른 부분 을 설정 
//			10Kg 이상 표시 되지 않는 증상 처리(로드셀의 이상으로 판단)
//
//	2022-0913	I2C 풀업저항 2K 오옴 에서 4.7K 오옴 으로 변경
//			RTC Data Read, Write 정상 작동
//
//	2022-0921	체중측정 표시방법 변경
//			기존방식 : 20kg보다 작고 50g 보다 큰 무게를 인식하면 다시 무게를 측정, +-30g 이내로 3번 인식되면 무게를 표시하고 무게 저장,
//				   무게는 계속 표시하지만, 0 값이 인식되지 않으면 무게는 저장하지 않음.
//			변경방식 : 20kg보다 작고 입구체중 보다 큰 무게를 인식하면 먼저 무게를 표시하고 다시무게를 측정, +-30g 이내로 3번 인식되면 무게를 저장 하고 알람,
//				   무게는 계속 표시하지만, 0 값이 인식되지 않으면 무게는 저장하지 않음.
//
//	2022-0926	각 함수의 기능에 대한 주석 처리 작업
//			주석 처러 중 필요 없거나 중첩되어진 루틴 정리
//	
//	2022-1004	MCU 부팅시 RTC 가 초기화 되어있지 않으면 정상 동작하지 않는 문제 해결
//			EEPROM Read 후 RTC 가 초기화 되어있지 않으면 RTC 초기화 후 MCU ReSet
//			RTC 설정 시 0년~99년, 1월~12월, 1일~31일, 0시~23시, 0분~59분, 0초~59초 까지 설정하도록 수정
//
//	2022-1129	10Kg 로드셀에 문제가 있어(너무 무거운 무게에 의한 로드셀 자체 파손) 100Kg 로드셀을 적용
//			로드셀의 단위 변경으로 인한 각 변수의 변수형에 문제 발생 각 변수 유형 확인 및 변경
//			무게 측정시 걸레값 인식율을 줄이기 위한 측정 시간 조절
//			CAPA, RO를 적용 후 오차를 보정할수 있도록 처리
//
//	2022-1201	로드셀의 갯수를 2개에서 4개로 변경 Test
//			로드셀의 갯수를 설정할수 있도록 처리 (2개 또는 4개)
//			로드셀의 갯수에 따라 에러 검출률을 조절하도록 처리(최대 무게 측정 단위 변경)
//			일정 횟수이상(100회)같은 무게(500g이상)가 인식 되는 경우 초기화 추가
//			일정 횟수이상(25회) 연속으로 무게가 측정 되는 경우 초기화 추가
//
//	2022-1206	부팅 시 버전을 FND 에 표시 하도록 처리
//
//	2022-1209	측정한 제한시간(2.5초 x 3개 Cell)이 지났으며, <====== 흔들리는 무게측정을 위한 조치
//			이전체중이 측정체중과 인증체중(50)을 더한 값과 같거나 작고 인증체중(50)을 뺀값과 같거나 크면서 이전체중과 같은경우가 3번이면 무게측정 OK
//			무게를 측정하고, 측정제한 시간 카운터 등 초기화
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
	OLD_Date = ee_read_byte(OLD_Date_Addr);					// 날짜 불러오기
	if(OLD_Date > 31 || OLD_Date < 0)					// 날짜가 31일보다 크거나 0보다 작으면
	{
		OLD_Date = 1;							// 날짜를 1로 초기화
		ee_write_byte(OLD_Date_Addr, OLD_Date);				// 날짜를 EEPROM 에 저장
	}
//-------------------------------------------------------------------------------------------------
//		1번, 2번, 3번 로드셀 ID
//-------------------------------------------------------------------------------------------------
	Cell_ID_1 = ee_read_byte(Cell_ID_1_Addr);				// 1번 로드셀 ID 설정값 불러오기
	if(Cell_ID_1 > 60 || Cell_ID_1 < 0)					// 1번 로드셀 ID 설정값이 60보다 크거나 0보다 작으면
	{
		Cell_ID_1 = 1;							// 1번 로드셀 ID 값을 1로 초기화
		ee_write_byte(Cell_ID_1_Addr, Cell_ID_1);			// 1번 로드셀 ID 값을 EEPROM 에 저장
	}

	Cell_ID_2 = ee_read_byte(Cell_ID_2_Addr);				// 2번 로드셀 ID 설정값 불러오기
	if(Cell_ID_2 > 60 || Cell_ID_2 < 0)					// 2번 로드셀 ID 설정값이 60보다 크거나 0보다 작으면
	{
		Cell_ID_2 = 0;							// 2번 로드셀 ID 값을 2로 초기화
		ee_write_byte(Cell_ID_2_Addr, Cell_ID_2);			// 2번 로드셀 ID 값을 EEPROM 에 저장
	}

	Cell_ID_3 = ee_read_byte(Cell_ID_3_Addr);				// 3번 로드셀 ID 설정값 불러오기
	if(Cell_ID_3 > 60 || Cell_ID_3 < 0)					// 3번 로드셀 ID 설정값이 60보다 크거나 0보다 작으면
	{
		Cell_ID_3 = 0;							// 3번 로드셀 ID 값을 3로 초기화
		ee_write_byte(Cell_ID_3_Addr, Cell_ID_3);			// 3번 로드셀 ID 값을 EEPROM 에 저장
	}
//-------------------------------------------------------------------------------------------------
//		1번, 2번, 3번 로드셀 ID
//-------------------------------------------------------------------------------------------------
	Cell_CNT_1 = ee_read_byte(Cell_CNT_1_Addr);				// 1번 로드셀 갯수 설정값 불러오기
	if(Cell_CNT_1 > 4 || Cell_CNT_1 < 1)					// 1번 로드셀 갯수 설정값이 8보다 크거나 2보다 작으면
	{
		Cell_CNT_1 = 2;							// 1번 로드셀 갯수 설정값을 2로 초기화
		ee_write_byte(Cell_CNT_1_Addr, Cell_CNT_1);			// 1번 로드셀 갯수 설정값을 EEPROM 에 저장
	}

	Cell_CNT_2 = ee_read_byte(Cell_CNT_2_Addr);				// 2번 로드셀 갯수 설정값 불러오기
	if(Cell_CNT_2 > 4 || Cell_CNT_2 < 1)					// 2번 로드셀 갯수 설정값이 8보다 크거나 2보다 작으면
	{
		Cell_CNT_2 = 2;							// 2번 로드셀 갯수 설정값을 2로 초기화
		ee_write_byte(Cell_CNT_2_Addr, Cell_CNT_2);			// 2번 로드셀 갯수 설정값을 EEPROM 에 저장
	}

	Cell_CNT_3 = ee_read_byte(Cell_CNT_3_Addr);				// 3번 로드셀 갯수 설정값 불러오기
	if(Cell_CNT_3 > 4 || Cell_CNT_3 < 1)					// 3번 로드셀 갯수 설정값이 8보다 크거나 2보다 작으면
	{
		Cell_CNT_3 = 2;							// 3번 로드셀 갯수 설정값을 2로 초기화
		ee_write_byte(Cell_CNT_3_Addr, Cell_CNT_3);			// 3번 로드셀 갯수 설정값을 EEPROM 에 저장
	}
//-------------------------------------------------------------------------------------------------
//		1번, 2번, 3번 로드셀 CAPA (용량)
//-------------------------------------------------------------------------------------------------
	Cell_Capa_1 = ee_read_word(Cell_Capa_1_Addr);				// 1번 로드셀 CAPA 설정값 불러오기
	if(Cell_Capa_1 > 1000 || Cell_Capa_1 < 100)				// 1번 로드셀 CAPA 설정값이 1000보다 크거나 100보다 작으면
	{
		Cell_Capa_1 = 100;						// 1번 로드셀 CAPA 값을 100kg로 초기화, 곱하기 1,000으로 계산 100kg = 100000g
		ee_write_word(Cell_Capa_1_Addr, Cell_Capa_1);			// 1번 로드셀 CAPA 값을 EEPROM 에 저장
	}

	Cell_Capa_2 = ee_read_word(Cell_Capa_2_Addr);				// 2번 로드셀 CAPA 설정값 불러오기
	if(Cell_Capa_2 > 1000 || Cell_Capa_2 < 100)				// 2번 로드셀 CAPA 설정값이 1000보다 크거나 100보다 작으면
	{
		Cell_Capa_2 = 100;						// 2번 로드셀 CAPA 값을 100kg로 초기화, 곱하기 1,000으로 계산 100kg = 100000g
		ee_write_word(Cell_Capa_2_Addr, Cell_Capa_2);			// 2번 로드셀 CAPA 값을 EEPROM 에 저장
	}

	Cell_Capa_3 = ee_read_word(Cell_Capa_3_Addr);				// 3번 로드셀 CAPA 설정값 불러오기
	if(Cell_Capa_3 > 1000 || Cell_Capa_3 < 100)				// 3번 로드셀 CAPA 설정값이 1000보다 크거나 100보다 작으면
	{
		Cell_Capa_3 = 100;						// 3번 로드셀 CAPA 값을 100kg로 초기화, 곱하기 1,000으로 계산 100kg = 100000g
		ee_write_word(Cell_Capa_3_Addr, Cell_Capa_3);			// 3번 로드셀 CAPA 값을 EEPROM 에 저장
	}
//-------------------------------------------------------------------------------------------------
//		1번, 2번, 3번 로드셀 RO (정격출력)
//-------------------------------------------------------------------------------------------------
	Cell_RO_1 = ee_read_dword(Cell_RO_1_Addr);				// 1번 로드셀 RO 설정값 불러오기
	if(Cell_RO_1 > 90000 || Cell_RO_1 < 10000)				// 1번 로드셀 RO 설정값이 9.000mV보다 크거나 3.000mV보다 작으면
	{
		Cell_RO_1 = 40000;						// 1번 로드셀 RO 값을 4.000mV로 초기화, 나누기 10,000으로 계산
		ee_write_dword(Cell_RO_1_Addr, Cell_RO_1);			// 1번 로드셀 RO 값을 EEPROM 에 저장
	}

	Cell_RO_2 = ee_read_dword(Cell_RO_2_Addr);				// 2번 로드셀 RO 설정값 불러오기
	if(Cell_RO_2 > 90000 || Cell_RO_2 < 10000)				// 2번 로드셀 RO 설정값이 9.000mV보다 크거나 3.000mV보다 작으면
	{
		Cell_RO_2 = 40000;						// 2번 로드셀 RO 값을  4.000mV로 초기화, 나누기 10,000으로 계산
		ee_write_dword(Cell_RO_2_Addr, Cell_RO_2);			// 2번 로드셀 RO 값을 EEPROM 에 저장
	}

	Cell_RO_3 = ee_read_dword(Cell_RO_3_Addr);				// 3번 로드셀 RO 설정값 불러오기
	if(Cell_RO_3 > 90000 || Cell_RO_3 < 10000)				// 3번 로드셀 RO 설정값이 9.000mV보다 크거나 3.000mV보다 작으면
	{
		Cell_RO_3 = 40000;						// 3번 로드셀 RO 값을 4.000mV로 초기화, 나누기 10,000으로 계산
		ee_write_dword(Cell_RO_3_Addr, Cell_RO_3);			// 3번 로드셀 RO 값을 EEPROM 에 저장
	}
//-------------------------------------------------------------------------------------------------
//		1번, 2번, 3번 로드셀 보정 값
//-------------------------------------------------------------------------------------------------
	Interval_1 = ee_read_word(Cell_Interval_1_Addr);			// 1번 로드셀 보정 값 불러오기
	if((Interval_1 > 99) || (Interval_1 < -99))				// 1번 로드셀 보정 값이 99보다 크거나 -99보다 작으면
	{
		Interval_1 = 0;							// 1번 로드셀 보정 값을 0 으로 초기화
		ee_write_word(Cell_Interval_1_Addr, Interval_1);		// 1번 로드셀 보정 값을 EEPROM 에 저장
	}

	Interval_2 = ee_read_word(Cell_Interval_2_Addr);			// 2번 로드셀 보정 값 불러오기
	if((Interval_2 > 99) || (Interval_2 < -99))				// 2번 로드셀 보정 값이 99보다 크거나 -99보다 작으면
	{
		Interval_2 = 0;							// 2번 로드셀 보정 값을 0 으로 초기화
		ee_write_word(Cell_Interval_2_Addr, Interval_2);		// 2번 로드셀 보정 값을 EEPROM 에 저장
	}

	Interval_3 = ee_read_word(Cell_Interval_3_Addr);			// 3번 로드셀 보정 값 불러오기
	if((Interval_3 > 99) || (Interval_3 < -99))				// 3번 로드셀 보정 값이 99보다 크거나 -99보다 작으면
	{
		Interval_3 = 0;							// 3번 로드셀 보정 값을 0 으로 초기화
		ee_write_word(Cell_Interval_3_Addr, Interval_3);		// 3번 로드셀 보정 값을 EEPROM 에 저장
	}
//-------------------------------------------------------------------------------------------------
//		1번, 2번, 3번 입구 체중
//-------------------------------------------------------------------------------------------------
	IN_Weight_1 = ee_read_word(Cell_IN_S_1_Addr);				// 1번 입구 체중 설정값 불러오기
	if(IN_Weight_1 > 10000 || IN_Weight_1 < 10)				// 1번 입구 체중 설정값이 10000g(10Kg)보다 크거나 10g보다 작으면
	{
		IN_Weight_1 = 500;						// 1번 입구 체중 값을 500으로 초기화 (500g)
		ee_write_word(Cell_IN_S_1_Addr, IN_Weight_1);			// 1번 입구 체중 값을 EEPROM 에 저장
	}

	IN_Weight_2 = ee_read_word(Cell_IN_S_2_Addr);				// 2번 입구 체중 설정값 불러오기
	if(IN_Weight_2 > 10000 || IN_Weight_2 < 10)				// 2번 입구 체중 설정값이 10000g(10Kg)보다 크거나 10g보다 작으면
	{
		IN_Weight_2 = 500;						// 2번 입구 체중 값을 500으로 초기화 (500g)
		ee_write_word(Cell_IN_S_2_Addr, IN_Weight_2);			// 2번 입구 체중 값을 EEPROM 에 저장
	}

	IN_Weight_3 = ee_read_word(Cell_IN_S_3_Addr);				// 3번 입구 체중 설정값 불러오기
	if(IN_Weight_3 > 10000 || IN_Weight_3 < 10)				// 3번 입구 체중 설정값이 10000g(10Kg)보다 크거나 10g보다 작으면
	{
		IN_Weight_3 = 500;						// 3번 입구 체중 값을 500으로 초기화 (500g)
		ee_write_word(Cell_IN_S_3_Addr, IN_Weight_3);			// 3번 입구 체중 값을 EEPROM 에 저장
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
void Check_Value(uint8_t Number)						// 로드셀 체중측정
{
	uint32_t Value = 0;							// Value 변수 초기화
	uint32_t Old_Value = 0;							// Old_Value 변수 초기화
	uint16_t Limit_Time = 0;							// Old_Value 변수 초기화

//-------------------------------------------------------------------------------------------------
	if(Number == 1)								// 1번 로드셀 이면
	{
		Value = CELL.Value_1 = Read_Load_Cell(1);			// 1번 로드셀 측정값
		Old_Value = CELL.Old_Value_1;					// 1번 로드셀 Old_Value 값
		Zero_Buff = CELL.Zero_Buff_1;					// 1번 로드셀 Zero_Buff 값
		OK_Count = OK_Count_1;						// 1번 로드셀 OK_Count 값
		Limit_Time = Timer.Check_Limit_Timer_1;				// 1번 로드셀 무게측정 제한 시간
	}
	else if(Number == 2)							// 2번 로드셀 이면
	{
		Value = CELL.Value_2 = Read_Load_Cell(2);			// 2번 로드셀 측정값
		Old_Value = CELL.Old_Value_2;					// 2번 로드셀 Old_Value 값
		Zero_Buff = CELL.Zero_Buff_2;					// 2번 로드셀 Zero_Buff 값
		OK_Count = OK_Count_2;						// 2번 로드셀 OK_Count 값
		Limit_Time = Timer.Check_Limit_Timer_2;				// 2번 로드셀 무게측정 제한 시간
	}
	else if(Number == 3)							// 3번 로드셀 이면
	{
		Value = CELL.Value_3 = Read_Load_Cell(3);			// 3번 로드셀 측정값
		Old_Value = CELL.Old_Value_3;					// 3번 로드셀 Old_Value 값
		Zero_Buff = CELL.Zero_Buff_3;					// 3번 로드셀 Zero_Buff 값
		OK_Count = OK_Count_3;						// 3번 로드셀 OK_Count 값
		Limit_Time = Timer.Check_Limit_Timer_3;				// 3번 로드셀 무게측정 제한 시간
	}
//-------------------------------------------------------------------------------------------------
	if(Value)								// 측정체중이 있으면
	{
		if((Limit_Time >= 500) & ((Old_Value <= Value + Admit_Weight) | (Old_Value >= Value - Admit_Weight) | (Old_Value == Value)))
		{								// 이전체중이 측정체중과 인증체중(50)을 더한값과 같거나 작고 인증체중(50)을 뺀값과 같거나 크면서 이전체중과 같으면
			OK_Count++;						// 측정체중 OK Count 증가
			if(OK_Count > 3)					// 측정체중 OK Count 가 5보다 크면
			{
				if(!Zero_Buff)					// Zero_Buff 가 0 이면
				{
					Zero_Buff = 1;				// Zero_Buff = 1

					if     (Number == 1) {CELL.Zero_Buff_1 = Zero_Buff;}	// 1번 로드셀 이면 Zero_Buff_1 저장
					else if(Number == 2) {CELL.Zero_Buff_2 = Zero_Buff;}	// 2번 로드셀 이면 Zero_Buff_2 저장
					else if(Number == 3) {CELL.Zero_Buff_3 = Zero_Buff;}	// 3번 로드셀 이면 Zero_Buff_3 저장

					for(uint8_t i = 20; i > 1; i--)				// 20 번 부터 1번까지 20번 반복
					{
						if     (Number == 1) {Data_1[i] = Data_1[i-1];}	// 1번 로드셀 이면 체중 Data_1 1칸씩 뒤로 밀기
						else if(Number == 2) {Data_2[i] = Data_2[i-1];}	// 2번 로드셀 이면 체중 Data_2 1칸씩 뒤로 밀기
						else if(Number == 3) {Data_3[i] = Data_3[i-1];}	// 3번 로드셀 이면 체중 Data_3 1칸씩 뒤로 밀기 
					}

					if     (Number == 1) {Data_1[1] = CELL.Value_1 = CELL.Value_1 + Interval_1; CELL.Count_1++; Cell_Desplay(Number); Alarm(); CELL.Zero_Buff_1 = 0; Timer.Check_Limit_Timer_1 = 0;}	
					else if(Number == 2) {Data_2[1] = CELL.Value_2 = CELL.Value_2 + Interval_2; CELL.Count_2++; Cell_Desplay(Number); Alarm(); CELL.Zero_Buff_2 = 0; Timer.Check_Limit_Timer_2 = 0;}
					else if(Number == 3) {Data_3[1] = CELL.Value_3 = CELL.Value_3 + Interval_3; CELL.Count_3++; Cell_Desplay(Number); Alarm(); CELL.Zero_Buff_3 = 0; Timer.Check_Limit_Timer_3 = 0;}
					// 1, 2, 3번 로드셀 이면 Data[1]에 측정체중에 오차 를 더하고 저장 측정 횟수 증가 체중표시, Zero_Buff 초기화
				}
				OK_Count = 0;					// 측정체중 OK Count 초기화
				printf("Value %ld \r\n", Value + Interval_1);
			}
			if     (Number == 1) {OK_Count_1 = OK_Count; CELL.Old_Value_1 = Old_Value; CELL.Value_1 = CELL.Value_1 + Interval_1; Cell_Desplay(Number);}		// 1번 로드셀 이면 OK_Count 저장
			else if(Number == 2) {OK_Count_2 = OK_Count; CELL.Old_Value_2 = Old_Value; CELL.Value_2 = CELL.Value_2 + Interval_2; Cell_Desplay(Number);}		// 2번 로드셀 이면 OK_Count 저장
			else if(Number == 3) {OK_Count_3 = OK_Count; CELL.Old_Value_3 = Old_Value; CELL.Value_3 = CELL.Value_3 + Interval_3; Cell_Desplay(Number);}		// 3번 로드셀 이면 OK_Count 저장
		}
		else	// 이전체중이 측정체중과 인증체중(50)을 더한값과 같거나 작고 인증체중(50)을 뺀값과 같거나 크면서 이전체중과 같으면 이 아니면
		{
			Old_Value = Value;					// 이전체중에 측정체중 저장
			if     (Number == 1) {CELL.Old_Value_1 = Old_Value; CELL.Value_1 = CELL.Value_1 + Interval_1; Cell_Desplay(Number); Zero_Count_1 = 0;}
			else if(Number == 2) {CELL.Old_Value_2 = Old_Value; CELL.Value_2 = CELL.Value_2 + Interval_2; Cell_Desplay(Number); Zero_Count_2 = 0;}
			else if(Number == 3) {CELL.Old_Value_3 = Old_Value; CELL.Value_3 = CELL.Value_3 + Interval_3; Cell_Desplay(Number); Zero_Count_3 = 0;}
			// 1, 2, 3번 로드셀 이면 이전측정체중 저장 측정체중에 오차 값을 더하고 체중표시, Zero_Count 초기화
		}
	}
	else 									// 측정체중이 없면
	{
		if(Number == 1)							// 1번 로드셀 이면
		{
			Zero_Count_1++;						// Zero_Count_1 증가
			if(Zero_Count_1 > 2) { Cell_Desplay(Number); CELL.Zero_Buff_1 = 0; Zero_Count_1 = 0; CELL.Old_Value_1 = 0;}
		}// Zero_Count_1 이 2보다 크면 측정체중 표시, Zero_Buff_1 초기화, Zero_Count_1 초기화, 이전측정체중_1 초기화, 측정체중 OK Count 초기화
		else if(Number == 2)						// 2번 로드셀 이면
		{
			Zero_Count_2++;						// Zero_Count_2 증가
			if(Zero_Count_2 > 2) {Cell_Desplay(Number); CELL.Zero_Buff_2 = 0; Zero_Count_2 = 0; CELL.Old_Value_2 = 0;}
		}	// Zero_Count_2 이 2보다 크면 측정체중 표시, Zero_Buff_2 초기화, Zero_Count_2 초기화, 이전측정체중_2 초기화, 측정체중 OK Count 초기화
		else if(Number == 3)						// 2번 로드셀 이면
		{
			Zero_Count_3++;						// Zero_Count_3 증가
			if(Zero_Count_3 > 2) {Cell_Desplay(Number); CELL.Zero_Buff_3 = 0; Zero_Count_3 = 0; CELL.Old_Value_3 = 0;}
		}	// Zero_Count_3 이 2보다 크면 측정체중 표시, Zero_Buff_3 초기화, Zero_Count_3 초기화, 이전측정체중_3 초기화, 측정체중 OK Count 초기화
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
			if     (CELL.Count_1 >= 20) {Count = CELL.Count_1 + i;}	// Count_1 이 20 보다 크거나 같으면 Count_1을 i 만큼 증가 하고
			else if(CELL.Count_1 >= i)  {Count = i;}		// Count_1 이  i 보다 크거나 같으면 Count_1에 i 를널고
			else if(CELL.Count_1 < i)   {Count = 0;}		// Count_1 이  i 보다 작으면 0 으로 초기화
			SerialData[cnt++] = Count / 256;			// 측정횟수 i 상위	// 10 ~ 89
			SerialData[cnt++] = Count % 256;			// 측정횟수 i 하위
			SerialData[cnt++] = Data_1[i] / 256;			// 측정무게 i 상위
			SerialData[cnt++] = Data_1[i] % 256;			// 측정무게 i 하위
		}
		else if(Number == 2)						// 2번 로드셀 이면
		{
			if     (CELL.Count_2 >= 20){Count = CELL.Count_2 + i;}	// Count_2 이 20 보다 크거나 같으면 Count_2을 i 만큼 증가 하고
			else if(CELL.Count_2 >= i) {Count = i;}			// Count_2 이  i 보다 크거나 같으면 Count_2에 i 를널고
			else if(CELL.Count_2 < i)  {Count = 0;}			// Count_2 이  i 보다 작으면 0 으로 초기화
			SerialData[cnt++] = Count / 256;			// 측정횟수 i 상위	// 10 ~ 89
			SerialData[cnt++] = Count % 256;			// 측정횟수 i 하위
			SerialData[cnt++] = Data_2[i] / 256;			// 측정무게 i 상위
			SerialData[cnt++] = Data_2[i] % 256;			// 측정무게 i 하위
		}
		else if(Number == 3)						// 3번 로드셀 이면
		{
			if     (CELL.Count_3 >= 20) {Count = CELL.Count_3 + i;}	// Count_3 이 20 보다 크거나 같으면 Count_3을 i 만큼 증가 하고
			else if(CELL.Count_3 >= i)  {Count = i;}		// Count_3 이  i 보다 크거나 같으면 Count_3에 i 를널고
			else if(CELL.Count_3 < i)   {Count = 0;}		// Count_3 이  i 보다 작으면 0 으로 초기화
			SerialData[cnt++] = Count / 256;			// 측정횟수 i 상위	// 10 ~ 89
			SerialData[cnt++] = Count % 256;			// 측정횟수 i 하위
			SerialData[cnt++] = Data_3[i] / 256;			// 측정무게 i 상위
			SerialData[cnt++] = Data_3[i] % 256;			// 측정무게 i 하위
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
		if(Rx3_Buff[cnt] == STX_Iontec && Rx3_Buff[cnt + 1] == PC2CTL && Rx3_Buff[cnt + 2] == 0x01 && Rx3_Buff[cnt + 4] == 0 && Rx3_Buff[cnt + 9] == ETX_Iontec)
		{//			STX			       PC2CTL			     선별기			실시간정보			ETX
			Rx_LED_ON;						// RX LED ON
			ChkSum = 0;						// ChkSum 초기화

			for(cnt1 = 1; cnt1 < 8; cnt1++) {ChkSum += Rx3_Buff[cnt + cnt1];}	// 체크썸 계산

			if(ChkSum == Rx3_Buff[cnt + 8])				// 체크썸 확인
			{
				if(Rx3_Buff[cnt + 3] == Cell_ID_1)		// ID 1번 이면
				{						// 실시간 정보
					Tx_LED_ON;				// TX LED ON
					Send_To_Server(1, Cell_ID_1);		// 무게 Data PC로 전송
					_delay_ms(2);
					Tx_LED_OFF;				// TX LED OFF
				}
				else if(Rx3_Buff[cnt + 3] == Cell_ID_2)		// ID 2번 이면
				{						// 실시간 정보
					Tx_LED_ON;				// TX LED ON
					Send_To_Server(2, Cell_ID_2);		// 무게 Data PC로 전송
					_delay_ms(2);
					Tx_LED_OFF;				// TX LED OFF
				}
				else if(Rx3_Buff[cnt + 3] == Cell_ID_3)		// ID 3번 이면
				{						// 실시간 정보
					Tx_LED_ON;				// TX LED ON
					Send_To_Server(3, Cell_ID_3);		// 무게 Data PC로 전송
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

				if(Rx3_Buff[cnt + 3] == Cell_ID_1)			// Cell ID 1번 이면
				{
					IN_Weight_1 = IN_S;				// 입구체중 값을 로드셀 1번에 적용
					ee_write_word(Cell_IN_S_1_Addr, IN_S);		// 입구체중 값을 EEPROM에 저장
					Tx_LED_ON;					// TX LED ON
					Send_To_Server_1(Cell_ID_1, Rx3_Buff[cnt + 4]);	// 설정정보 PC로 전송
					_delay_ms(2);
					Tx_LED_OFF;					// TX LED OFF
				}

				else if(Rx3_Buff[cnt + 3] == Cell_ID_2)			// Cell ID 2번 이면
				{
					IN_Weight_2 = IN_S;				// 입구체중 값을 로드셀 2번에 적용
					ee_write_word(Cell_IN_S_2_Addr, IN_S);		// 입구체중 값을 EEPROM에 저장
					Tx_LED_ON;					// TX LED ON
					Send_To_Server_1(Cell_ID_2, Rx3_Buff[cnt + 4]);	// 설정정보 PC로 전송
					_delay_ms(2);
					Tx_LED_OFF;					// TX LED OFF
				}

				else if(Rx3_Buff[cnt + 3] == Cell_ID_3)			// Cell ID 3번 이면
				{
					IN_Weight_3 = IN_S;				// 입구체중 값을 로드셀 3번에 적용
					ee_write_word(Cell_IN_S_3_Addr, IN_S);		// 입구체중 값을 EEPROM에 저장
					Tx_LED_ON;					// TX LED ON
					Send_To_Server_1(Cell_ID_3, Rx3_Buff[cnt + 4]);	// 설정정보 PC로 전송
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

		if(OLD_Date != RTC_T.Date)					// 날짜 Data 가 다르면
		{
			OLD_Date = RTC_T.Date;					// 현제 날짜 저장
			memset(Data_1, 0, 40);					// 측정 무게 Data  1 Clear
			CELL.Count_1 = 0;					// 측정 무게 Count 1 Clear
			memset(Data_2, 0, 40);					// 측정 무게 Data  2 Clear
			CELL.Count_2 = 0;					// 측정 무게 Count 2 Clear
			memset(Data_3, 0, 40);					// 측정 무게 Data  3 Clear
			CELL.Count_3 = 0;					// 측정 무게 Count 3 Clear
			ee_write_byte (OLD_Date_Addr, OLD_Date);		// 날짜 를 EEPROM 에 저장
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
	CELL.Proportion_1 = (Cell_RO_1 / 10000000 / (ADC_Value_0 + 0.48) * 128 / 0.5 * 8388608 / (Cell_Capa_1 * 1000)) / Cell_CNT_1;
	CELL.Proportion_2 = (Cell_RO_2 / 10000000 / (ADC_Value_1 + 0.48) * 128 / 0.5 * 8388608 / (Cell_Capa_2 * 1000)) / Cell_CNT_2;
	CELL.Proportion_3 = (Cell_RO_3 / 10000000 / (ADC_Value_2 + 0.48) * 128 / 0.5 * 8388608 / (Cell_Capa_3 * 1000)) / Cell_CNT_3;
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

	CELL.Zero_Buff_1 = 0;							// 1번 측정 값 Zero Buff 초기화
	CELL.Zero_Buff_2 = 0;							// 2번 측정 값 Zero Buff 초기화
	CELL.Zero_Buff_3 = 0;							// 3번 측정 값 Zero Buff 초기화
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
	uint8_t Time_Count = 5;							// 시간 표시 용 변수 설정
	uint8_t SECOND = 0;							// 이전 시간(초)용 변수 설정
	Read_eeprom();
//-------------------------------------------------------------------------------------------------
	DIGIT[0][3] = FND_OFF;							// Ver 1.0 표시
	DIGIT[0][2] = FND_1 + FND_Point;					
	DIGIT[0][1] = FND_0;						
	DIGIT[0][0] = FND_OFF;
	_delay_ms(1000);							// 1초 동안 표시	
	wdt_reset();								// Reset WDT
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
		if(Cell_ID_1) {Check_Value(1);}					// 1번 로드셀의 ID가 설정되어 있으면 체중측정 표시
		else	      {FND_Value_Data(0, 0, 12); _delay_ms(150);}	// FND 0 FND_OFF

		if(Cell_ID_2) {Check_Value(2);}					// 2번 로드셀의 ID가 설정되어 있으면 체중측정 표시
		else	      {FND_Value_Data(1, 1, 12); _delay_ms(150);}	// FND 1 FND_OFF

		if(Cell_ID_3) {Check_Value(3);}					// 3번 로드셀의 ID가 설정되어 있으면 체중측정 표시
		else	      {FND_Value_Data(2, 2, 12); _delay_ms(150);}	// FND 2 FND_OFF

		if(SET.Menu) {Seting_Mode();}					// SET Menu 가 설정되어 있으면 Seting_Mode 진입

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
