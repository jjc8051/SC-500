//=================================================================================================
//		ProductName 	:	Pig_Scales-2 V4.6 SC-500
//=================================================================================================
//		File  Name	:	main.c
//		File  내용	:	이동형 체중 측정기
//		작성  일자	:	2022년 12월 13일
//		수정  일자	:	2023년 04월 10일
//=================================================================================================
//		BOARD		:	Pig Scales Control Board
//		CPU		:	AVR128DA64 [ EXTCLK:14745600Hz, REG:32X8_Bit, SRAM:16K_Byte, EEPROM:512_Byte, FLASH:128K_Byte, PACKAGE:64_TQFP-10mm ]
//		CLOCK		:	14745600Hz (External Clock)
//		COMPILER	:	Microchip Studio (Version:7.0.2594)
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
//	2023-0106	EEPROM Data 최대 35개 까지 저장
//			부팅 완료 후 EEPROM의 저장 Data 를 불러와서 Data 초기화
//
//	2023-0215	150Kg 로드셀로 Test 한 결과
//			노이즈에 의한 튀는 현상도 너무 많고, 정상 적인 무게를 인식 하지 못하는 현상 발생
//			이를 해결 하기 위해 노이즈를 최대한 제거하기 위해 입력받는 부분에대한 소프트웨어 필터링 작업
//			펌웨어에 의한 필터링으로 어느 정도 안정된값을 받을수 있도록 처리
//
//	2023-0216	자동 모드 일때와 수동 모드일때 무게값 Display 방법 처리
//			배율계산시에서 입력 전압 부분을 4V로 고정하여 계산 하도록 변경
//			아날로그 그라운드 단자 (AGND) 는 System GND 와연결 (노이즈 현격하게 감소)
//
//	2023-0217	펌웨어 버젼 정리, 펌웨어 백업 폴더 정리
//
//	2023-0228	무게 측정 Data 를 버퍼에 저장 하고 저장한 값을 (Data + NewData)/2 로 연산하여 연속적으로 무게 Data 를 평균 내도록 펌웨어 수정
//			스케일 조정 하는 부분도 (Data + NewData)/2 값을 대입하고 0값 이 들어오는 부분 안정화 작업 처리
//			측정무게 안정화 Count 는 5번 에서 7번으로 늘려서 정확도를 약간 높임
//
//	2023-0303	측정무게 안정화 Count 를 최소로 줄여서 안정화 보다 측정 속도를 높이는 것을 선택
//			사용하지 않을경우 3분 후 FND OFF 하도록 처리
//
//	2023-0307	측정무게 안정화 카운터를 무게범위(+50g, -50g)일때는 카운터_1 은 1증가 카운터_2 는 3증가
//			무게범위(+250g, -250g) 이내 일때는 카운터_2 만 2증가
//			무게범위(+500g, -500g) 이내 일때는 카운터_2 만 1증가
//			하여 총 카운터_1은 3번, 카운터_2은 15번 안에 들어온 무게의 평균을 측정체중 으로인정
//
//	2023-0320	최종 무게측정 범위 확정 및 버젼 V4.5로 변경
//
//	2023-0405	스케일 조정을 위한 RO값을 5에서 10씩 변환하도록 조정
//			Gwid_Set(); 함수 외에 게이트웨이 불필요 함수 정리
//
//	2023-0410	저장 Data 초기화, Data 전송번호 동기화 작업
//			FND 꺼진 상태에서 통신 작업 진행, 	FND 꺼지는 시간 늘림  30분에서 30분으로...
//			펌웨어 버젼 표시 V4.6으로 변경
//
//	2026-0611	버그 수정 (코드 분석 및 전면 검토)
//			[loadcell.c] Cell_Zero_Set() do-while 무한루프 제거 → CELL.OFF_Set = Value 단순화
//			[tca.c]      TCA1 타이머 주기 수정 : PER 0x000E(1ms) → 0x0047(5ms)
//			[main.c]     Read_eeprom_Weight_Data() EEPROM 주소 오류 수정 : MEASURE_MIN → MEASURE_SEC
//			[main.c]     Receiv_PC() IN_Scales 파싱 오류 수정 : & → |, 인덱스 [8]→[9] 추가
//			[main.c]     Receiv_PC() 버퍼 범위 초과 수정 : cnt < RX3_SIZE → cnt < RX3_SIZE - 31
//			[main.c]     Read_eeprom() uint8_t < 0 항상 false 수정 : OLD_D/Cell_ID < 0 → == 0
//			[main.c]     Read_eeprom() Auto_Mode 범위 검사 수정 : > 2 || < 0 → > 2
//			[loadcell.c] Interval_Set() RO 상하한 조건 추가 (언더플로우 방지)
//			[loadcell.c] Cell_Data_Read() HX711 24bit 부호 확장 수정 : XOR → sign extension
//			[zigbee.c]   ZigBee AT 응답 파싱 버퍼 범위 수정 : cnt < RX3_SIZE → cnt < RX3_SIZE - 2
//			[main.c]     Proportion_Set() 정수 나누기 오류 수정 : Cell_RO → (double)Cell_RO
//			[main.c]     Check_Value() 비트 AND 수정 : & → &&
//			[loadcell.c] Interval_Set() 비트 AND 수정 : & → &&
//			[driver_isr.c] USART ISR 버퍼 오버플로우 수정 : 증가 후 범위 체크로 순서 변경
//			[main.c]     Time_Check() memset 크기 수정 : 300 → sizeof() 사용
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
	.CODESIZE = 0x00,							// All remaining Flash used as App code (������ �÷��ô� ��� �� �ڵ�� ����)
	.BOOTSIZE = 0x03							// 0x0300
};

//=================================================================================================
const uint8_t fnd_tbl[]={FND_0, FND_1, FND_2, FND_3, FND_4, FND_5, FND_6, FND_7, FND_8, FND_9, FND_MINUS, FND_Point, FND_OFF};
//FND ���� ���̺�	{    0,     1,     2,     3,     4,     5,     6,     7,     8,     9,         -,         .,     OFF}

//=================================================================================================
uint8_t DIGIT[3][4] = {{0}};

//=================================================================================================
uint8_t Gukbun;							// ����Ʈ���� ���� ���� ����
uint8_t Gwid;							// ����Ʈ���� ID ���� ����
uint8_t Auto_Mode;						// �ڵ���� ���� ����

uint8_t OK_Count_1;						// OK Count 1
uint8_t OK_Count_2;						// OK Count 2
uint8_t Zero_Count;						// Zero Count ���� ����
uint8_t Value_Count;						// Value Count ���� ����

uint8_t Time_Count;						// �ð� ǥ�� �� ���� ����
uint16_t Sec_Count;						// �� ���� �� ���� ����
uint8_t SECOND;							// ���� �ð�(��)�� ���� ����

uint16_t Data_Number;						// Data ��ȣ
uint8_t Room_Number;						// �� ��ȣ
uint8_t Stal_Number;						// ���� ��ȣ
uint16_t EepCnt;

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
	static uint8_t cnt = 0;							// cnt ���� �ʱ�ȭ

	for(int8_t j = 2; j >= 0; j--)						// FND ������ŭ (3ȸ �ݺ�)
	{
		for(uint8_t i = 0; i < 8; i++)					// FND 8bit data ����(����)
		{
			HC595_SHIFT_OFF;					// 8 X 1 bit Data Shift �� HC595 SHIFT OFF
			if((DIGIT[j][cnt] >> i) & 0x01) {HC595_DATA_ON;}	// DIGIT[FND��ȣ][DIGIT��ȣ] �� 1 �̸� HC595_DATA_ON
			else				{HC595_DATA_OFF;}	// DIGIT[FND��ȣ][DIGIT��ȣ] �� 0 �̸� HC595_DATA_OFF
			HC595_SHIFT_ON;						// 8 X 1 bit Data Shift �� HC595 SHIFT ON
		}
	}
	HC595_LATCH_ON;								// FND ���� X 8bit Data Shift ��
	HC595_LATCH_OFF;							// HC595 LATCH ON, OFF
//-------------------------------------------------------------------------------------------------
	switch(cnt)
	{
		case 0: DIGIT3_OFF; DIGIT2_OFF; DIGIT1_OFF; DIGIT0_ON;  break;	// DIGIT0 ON ������ OFF
		case 1: DIGIT3_OFF; DIGIT2_OFF; DIGIT1_ON;  DIGIT0_OFF; break;	// DIGIT1 ON ������ OFF
		case 2: DIGIT3_OFF; DIGIT2_ON;  DIGIT1_OFF; DIGIT0_OFF; break;	// DIGIT2 ON ������ OFF
		case 3: DIGIT3_ON;  DIGIT2_OFF; DIGIT1_OFF; DIGIT0_OFF; break;	// DIGIT3 ON ������ OFF
	}
	cnt++;									// ���� Digit �� ���� cnt ����
	if(cnt >= 4) {cnt = 0;}							// FND Digit ����ŭ (4ȸ) �ݺ� �� �ʱ�ȭ
}

//=================================================================================================
void FND_Value_Data(uint8_t start, uint8_t end, uint8_t value)			// FND �� �ݺ� �Ǵ� Data ǥ��
{
	for(uint8_t i = start; i <= end; i++)					// FND �� start ���� end ����
	{
		for(uint8_t j = 0; j <= 3; j++)					// FND DIGIT 0 ���� 3 ���� value Data ǥ��
		{
			DIGIT[i][j] = fnd_tbl[value];				// DIGIT[j] value Data ǥ��
		}
	}
}

//=================================================================================================
void FND_ALL_OFF(void)								// FND ��ü OFF
{
	for(uint8_t i = 0; i <= 2; i++)						// 0�� FND ���� 2�� FND ����
	{
		for(uint8_t j = 0; j <= 3; j++)					// FND DIGIT 0 ���� 3 ���� FND OFF
		{
			DIGIT[i][j] = FND_OFF;					// DIGIT[j] FND OFF
		}
	}
}

//=================================================================================================
//		key �� scan
//=================================================================================================
void Key_Scan(void)								// key �Է� üũ
{
	if(!KEY.Buff)								// Key �Է��� ��������
	{
		if(keytemp & 0x10) {Beep(10); KEY.Key = 1; KEY.Buff = 1; Timer.RegExitTimer = 0; Sec_Count = 0;}	// +(UP)  �� key �Է��� ������
		if(keytemp & 0x20) {Beep(10); KEY.Key = 2; KEY.Buff = 1; Timer.RegExitTimer = 0; Sec_Count = 0;}	// Select   key �Է��� ������
		if(keytemp & 0x40) {Beep(10); KEY.Key = 3; KEY.Buff = 1; Timer.RegExitTimer = 0; Sec_Count = 0;}	// -(Down)�� key �Է��� ������
		if(keytemp & 0x80) {Beep(10); KEY.Key = 4; KEY.Buff = 1; Timer.RegExitTimer = 0; Sec_Count = 0;}	// Menu     key �Է��� ������

		if(KEY.Key == 4)						// Menu key �Է��� ������
		{
			if(SET.Menu) {SET.Menu = 0;}				// �޴���� = 1 �̸� �޴���� ����
			else	     {SET.Menu = 1;}				// �޴���� = 0 �̸� �޴���� ����
		}

		if(SET.Menu && KEY.Key == 2 && !SET.Blink)			// �޴���� �̰� Blink = 0 �̰� ���� Key �Է��� ������
		{
			if(SET.Select) {SET.Select = 0;}			// ���ø�� = 1 �̸� ���ø�� ����
			else	       {SET.Select = 1;}			// ���ø�� = 0 �̸� ���ø�� ����
		}

		if(!SET.Menu && !SET.NumSET && KEY.Key == 2)			// �޴���� �� SET.NumSET �� 1�� �ƴϰ� ���� Key �Է��� ������
		{
			if(SET.Save) {SET.Save = 0;}				// ������ = 1 �̸� ������ ����
			else							// ������ = 0 �̸� ������ ����
			{
				SET.Save = 1;
				FND_Value_Data(2, 2, 10);			// FND 2 FND_MINUS ǥ��
			}
		}

		if(SET.Menu && KEY.Key == 2 && SET.Blink) {KEY.Count++;}	// �޴���� �̰� Blink = 1 �̰� ���� Key �Է��� ������ Key �� ����Ƚ�� ����

		if(!SET.Menu && KEY.Key == 1) {if(!SET.NumSET) {SET.NumSET = 1;}}	// �޴���尡 �ƴϰ� ���� Key �Է��� ������ NumSET��� = 1 �ƴϸ� NumSET��� ����

		if(KEY.Old == KEY.Key)						// Old Key ���� New Key ���� ������
		{
			if(KEY.In_Count >= 2) {Timer.Input_Key = 20;}		// �����Է��� 2���� ������ �����Է½ð� = 20
			else {Timer.Input_Key = 50; KEY.In_Count++;}		// �����Է��� 2���� ������ �����Է½ð� = 50, �����Է� ����
		}
		else								// Old Key ���� New Key ���� ���� ������
		{
			Timer.Input_Key = 50;					// �����Է� = 50
			KEY.Old = KEY.Key;					// Old Key �� New Key ����
			KEY.In_Count = 0;					// �����Է� �ʱ�ȭ
		}
	}
}

//=================================================================================================
void Read_eeprom(void)								// EEPROM Data �б�
{
//-------------------------------------------------------------------------------------------------
//		����Ʈ���� ID
//-------------------------------------------------------------------------------------------------
	Gwid = ee_read_byte(Gateway_GWID_Addr);					// ����Ʈ���� ID ������ �ҷ�����
	if(Gwid > 60 || Gwid == 0)						// ����Ʈ���� ID �������� 60���� ũ�ų� 0�� ������
	{
		Gwid = 1;							// ����Ʈ���� ID �������� 1�� �ʱ�ȭ
		ee_write_byte(Gateway_GWID_Addr, Gwid);				// ����Ʈ���� ID �������� EEPROM �� ����
	}
//-------------------------------------------------------------------------------------------------
//		����� ��¥ Date
//-------------------------------------------------------------------------------------------------
	RTC_T.OLD_D = ee_read_byte(OLD_Date_Addr);				// ��¥ ������ �ҷ�����
	if(RTC_T.OLD_D > 31 || RTC_T.OLD_D == 0)				// ��¥ �������� 31�Ϻ��� ũ�ų� 0�� ������
	{
		RTC_T.OLD_D = 1;						// ��¥ �������� 1�� �ʱ�ȭ
		ee_write_byte(OLD_Date_Addr, RTC_T.OLD_D);			// ��¥ �������� EEPROM �� ����
	}
//-------------------------------------------------------------------------------------------------
//		�ε弿 ID
//-------------------------------------------------------------------------------------------------
	Cell_ID = ee_read_byte(Cell_ID_Addr);					// �ε弿 ID ������ �ҷ�����
	if(Cell_ID > 60 || Cell_ID == 0)					// �ε弿 ID �������� 60���� ũ�ų� 0�� ������
	{
		Cell_ID = 1;							// �ε弿 ID �������� 1�� �ʱ�ȭ
		ee_write_byte(Cell_ID_Addr, Cell_ID);				// �ε弿 ID �������� EEPROM �� ����
	}
//-------------------------------------------------------------------------------------------------
//		�ε弿 ����
//-------------------------------------------------------------------------------------------------
	Cell_CNT = ee_read_byte(Cell_CNT_Addr);					// �ε弿 ���� ������ �ҷ�����
	if(Cell_CNT > 4 || Cell_CNT < 1)					// �ε弿 ���� �������� 8���� ũ�ų� 2���� ������
	{
		Cell_CNT = 2;							// �ε弿 ���� �������� 2�� �ʱ�ȭ
		ee_write_byte(Cell_CNT_Addr, Cell_CNT);				// �ε弿 ���� �������� EEPROM �� ����
	}
//-------------------------------------------------------------------------------------------------
//		�ε弿 CAPA (�뷮)
//-------------------------------------------------------------------------------------------------
	Cell_Capa = ee_read_word(Cell_Capa_Addr);				// �ε弿 CAPA ������ �ҷ�����
	if(Cell_Capa > 1000 || Cell_Capa < 100)					// �ε弿 CAPA �������� 1000���� ũ�ų� 100���� ������
	{
		Cell_Capa = 100;						// �ε弿 CAPA �������� 100kg�� �ʱ�ȭ, ���ϱ� 1,000���� ��� 100kg = 100000g
		ee_write_word(Cell_Capa_Addr, Cell_Capa);			// �ε弿 CAPA �������� EEPROM �� ����
	}
//-------------------------------------------------------------------------------------------------
//		�ε弿 RO (�������)
//-------------------------------------------------------------------------------------------------
	Cell_RO = ee_read_dword(Cell_RO_Addr);					// �ε弿 RO ������ �ҷ�����
	if(Cell_RO > 90000 || Cell_RO < 10000)					// �ε弿 RO �������� 9.000mV���� ũ�ų� 3.000mV���� ������
	{
		Cell_RO = 40000;						// �ε弿 RO �������� 4.000mV�� �ʱ�ȭ, ������ 10,000���� ���
		ee_write_dword(Cell_RO_Addr, Cell_RO);				// �ε弿 RO �������� EEPROM �� ����
	}
//-------------------------------------------------------------------------------------------------
//		�Ա� ü��
//-------------------------------------------------------------------------------------------------
	IN_Scales = ee_read_word(Cell_IN_S_Addr);				// �Ա�ü�� ������ �ҷ�����
	if(IN_Scales > 10000 || IN_Scales < 10)					// �Ա�ü�� �������� 10000g(10Kg)���� ũ�ų� 10g���� ������
	{
		IN_Scales = 500;						// �Ա�ü�� �������� 500���� �ʱ�ȭ (500g)
		ee_write_word(Cell_IN_S_Addr, IN_Scales);			// �Ա�ü�� �������� EEPROM �� ����
	}
//-------------------------------------------------------------------------------------------------
//		�� ��ȣ ��
//-------------------------------------------------------------------------------------------------
	Room_Number = ee_read_byte(Cell_ROOM_Addr);				// ���ȣ ������ �ҷ�����
	if(Room_Number > 100 || Room_Number <= 0)				// ���ȣ �������� 100���� ũ�ų� 0�� ���ų� ������
	{
		Room_Number = 1;						// ���ȣ �������� 1 ���� �ʱ�ȭ
		ee_write_byte(Cell_ROOM_Addr, Room_Number);			// ���ȣ �������� EEPROM �� ����
	}
//-------------------------------------------------------------------------------------------------
//		���� ��ȣ ��
//-------------------------------------------------------------------------------------------------
	Stal_Number = ee_read_byte(Cell_STAL_Addr);				// �����ȣ ���� �ҷ�����
	if(Stal_Number > 100 || Stal_Number <= 0)				// �����ȣ �������� 100���� ũ�ų� 0�� ���ų� ������
	{
		Stal_Number = 1;						// �����ȣ �������� 1 ���� �ʱ�ȭ
		ee_write_byte(Cell_STAL_Addr, Stal_Number);			// �����ȣ �������� EEPROM �� ����
	}
//-------------------------------------------------------------------------------------------------
//		�ڵ���� ���� ��
//-------------------------------------------------------------------------------------------------
	Auto_Mode = ee_read_byte(Cell_AUTO_Addr);				// �ڵ���� ������ �ҷ�����
	if(Auto_Mode > 2)							// �ڵ���� �������� 2���� ũ��
	{
		Auto_Mode = 0;							// �ڵ���� �������� 0 ���� �ʱ�ȭ
		ee_write_byte(Cell_AUTO_Addr, Auto_Mode);			// �ڵ���� �������� EEPROM �� ����
	}
//-------------------------------------------------------------------------------------------------
//		RTC �ʱ�ȭ
//-------------------------------------------------------------------------------------------------
	RTC_Time_Read();
	if(RTC_T.Sec > 59)							// �� ���� 59 ���� ũ��
	{
		RTC_Time_Write(RTC_SECOND, 0);					// RTC �� 00�� ����
		while(1);							// RTC �� �ʱ�ȭ ���� �ʾ� ������ CPU ReSet
	}
//-------------------------------------------------------------------------------------------------
	if(RTC_T.Year == 0)							// �� ���� 00 �̸�
	{
		RTC_Time_Write(RTC_YEAR,      23);				// RTC �� 23��   ����
		RTC_Time_Write(RTC_MONTH,      4);				// RTC �� 04��   ����
		RTC_Time_Write(RTC_DATE,      05);				// RTC �� 05��   ����
		RTC_Time_Write(RTC_DAYOFWEEK,  4);				// RTC �� 04���� ����
		RTC_Time_Write(RTC_HOUR,      12);				// RTC �� 12��   ����
		RTC_Time_Write(RTC_MINUTE,     0);				// RTC �� 00��   ����
		RTC_Time_Write(RTC_SECOND,     0);				// RTC �� 00��   ����
	}

//-------------------------------------------------------------------------------------------------
//		Data ��ȣ ��
//-------------------------------------------------------------------------------------------------
	EepCnt = ee_read_word(Data_Count_Addr);					// Data ��ȣ ������ �ҷ�����
	if(EepCnt > 300)							// Data��ȣ �������� 300���� ũ��
 	{
 		EepCnt = 0;
 		ee_write_word(Data_Count_Addr, Data_Number);			// Data ��ȣ �������� EEPROM �� ����
 	}
	Data_Number = EepCnt;
}

//=================================================================================================
void Read_eeprom_Weight_Data(uint16_t cnt)					// EEPROM Weight Data �б�
{
	SAVE.AutoSet[cnt] = ee_read_byte(MEASURE_AUTO   + (MEASURE_LENGTH * cnt));
	SAVE.Count  [cnt] = ee_read_word(MEASURE_COUNT  + (MEASURE_LENGTH * cnt));
	SAVE.Data   [cnt] = ee_read_word(MEASURE_WEIGHT + (MEASURE_LENGTH * cnt));
	SAVE.RoomNum[cnt] = ee_read_byte(ROOM_NUMBER    + (MEASURE_LENGTH * cnt));
	SAVE.StalNum[cnt] = ee_read_byte(STALL_NUMBER   + (MEASURE_LENGTH * cnt));

	SAVE.Year   [cnt] = ee_read_byte(MEASURE_YEAR   + (MEASURE_LENGTH * cnt));
	SAVE.Month  [cnt] = ee_read_byte(MEASURE_MONTH  + (MEASURE_LENGTH * cnt));
	SAVE.Date   [cnt] = ee_read_byte(MEASURE_DATE   + (MEASURE_LENGTH * cnt));
	SAVE.Hour   [cnt] = ee_read_byte(MEASURE_HOUR   + (MEASURE_LENGTH * cnt));
	SAVE.Min    [cnt] = ee_read_byte(MEASURE_MIN    + (MEASURE_LENGTH * cnt));
	SAVE.Sec    [cnt] = ee_read_byte(MEASURE_SEC    + (MEASURE_LENGTH * cnt));
}

//=================================================================================================
void eeprom_Data_erase(uint16_t cnt)						// EEPROM Weight Data �����
{

	ee_write_byte(MEASURE_AUTO   + (MEASURE_LENGTH * cnt), 0);
	ee_write_word(MEASURE_COUNT  + (MEASURE_LENGTH * cnt), 0);
	ee_write_word(MEASURE_WEIGHT + (MEASURE_LENGTH * cnt), 0);
	ee_write_byte(ROOM_NUMBER    + (MEASURE_LENGTH * cnt), 0);
	ee_write_byte(STALL_NUMBER   + (MEASURE_LENGTH * cnt), 0);
	ee_write_byte(MEASURE_YEAR   + (MEASURE_LENGTH * cnt), 0);
	ee_write_byte(MEASURE_MONTH  + (MEASURE_LENGTH * cnt), 0);
	ee_write_byte(MEASURE_DATE   + (MEASURE_LENGTH * cnt), 0);
	ee_write_byte(MEASURE_HOUR   + (MEASURE_LENGTH * cnt), 0);
	ee_write_byte(MEASURE_MIN    + (MEASURE_LENGTH * cnt), 0);
	ee_write_byte(MEASURE_SEC    + (MEASURE_LENGTH * cnt), 0);
}


//=================================================================================================
void Alarm(void)								// �˶�(���� 3��)
{
	Beep(15);								// 1mSec X 15 = 15mSec
	_delay_ms(120);								// Time Delay  120mSec
	Beep(15);								// 1mSec X 15 = 15mSec
	_delay_ms(120);								// Time Delay  120mSec
	Beep(15);								// 1mSec X 15 = 15mSec
}

//=================================================================================================
void Auto_Set(void)								// �ڵ����,������� ����
{
	while(SET.Menu && SET.Select)						// �޴��� 1 �̸� �ݺ�
	{
		if(KEY.Buff)							// Key �Է��� ������
		{
			if     (KEY.Key == 1) {Auto_Mode = 1;}
			else if(KEY.Key == 3) {Auto_Mode = 0;}
		}

		if(Auto_Mode)
		{
			if(Blink) {FND_Value_Data(1,1,12);}			// Blink = 1 �̸� FND 1 OFF
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
			if(Blink) {FND_Value_Data(1,1,12);}			// Blink = 1 �̸� FND 1 OFF
			else
			{
				DIGIT[1][3] = FND_OFF;
				DIGIT[1][2] = FND_O;
				DIGIT[1][1] = FND_F;
				DIGIT[1][0] = FND_F;
			}
		}
		wdt_reset();							// ��ġ�� ����
	}
	ee_write_byte(Cell_AUTO_Addr, Auto_Mode);				// �ڵ���� �������� EEPROM �� ����
}

//=================================================================================================
void Seting_Mode_0(void)							// ���ø�� 0
{
	uint8_t Count = 1;							// Count ���� �ʱ�ȭ
	SET.Select = 0;								// Select ��ư �ʱ�ȭ

	FND_Value_Data(0, 2, 12);						// FND 0 ~ 2 FND_OFF

	while(SET.Menu)								// Seting_Mode �̸� ���� �ݺ�
	{
		if(KEY.Buff)							// Key �Է��� ������
		{
			if     (KEY.Key == 1 && Count < 6) {Count++; KEY.Key = 0;}	// ��  ��ư �̸� Count ����
			else if(KEY.Key == 3 && Count > 0) {Count--; KEY.Key = 0;}	// �ٿ��ư �̸� Count ����
		}
		DIGIT[2][3] = FND_OFF;
		DIGIT[2][2] = FND_OFF;
		DIGIT[2][1] = FND_OFF;
		DIGIT[2][0] = fnd_tbl[Count];					// FND 2.0 �� Count ǥ��
//-------------------------------------------------------------------------------------------------
		switch(Count)							// Count
		{
			case 0:							// Count = 0 �̸�
				Count = 5;					// �ݺ��� ���� Count �� �ְ��� ���� ��Ŵ
			break;

			case 1:							// Count = 1 �̸�
				DIGIT[0][3] = FND_a;				// cell ǥ��
				DIGIT[0][2] = FND_u;
				DIGIT[0][1] = FND_t;
				DIGIT[0][0] = FND_o;

				DIGIT[1][3] = FND_OFF;				// Set ǥ��
				DIGIT[1][2] = FND_S;
				DIGIT[1][1] = FND_e;
				DIGIT[1][0] = FND_t;

				if(SET.Select) {Auto_Set();}			// �ڵ����,������� ����
			break;

			case 2:							// Count = 2 �̸�
				DIGIT[0][3] = FND_c;				// cell ǥ��
				DIGIT[0][2] = FND_e;
				DIGIT[0][1] = FND_l;
				DIGIT[0][0] = FND_l;

				DIGIT[1][3] = FND_OFF;				// Set ǥ��
				DIGIT[1][2] = FND_S;
				DIGIT[1][1] = FND_e;
				DIGIT[1][0] = FND_t;

				if(SET.Select) {Loadcell_Set();}		// �ε弿 ���� ���
			break;

			case 3:							// Count = 3 �̸�
				DIGIT[0][3] = FND_G;						// FND 0 �� G-Id (����)ǥ��
				DIGIT[0][2] = FND_MINUS;
				DIGIT[0][1] = FND_I;
				DIGIT[0][0] = FND_d;

				DIGIT[1][3] = FND_OFF;				// Set ǥ��
				DIGIT[1][2] = FND_S;
				DIGIT[1][1] = FND_e;
				DIGIT[1][0] = FND_t;

				if(SET.Select) {Gwid_Set();}			// ����Ʈ ���� ����
			break;

			case 4:							// Count = 4 �̸�
				DIGIT[0][3] = FND_OFF;				// Zbe ǥ��
				DIGIT[0][2] = FND_Z;
				DIGIT[0][1] = FND_b;
				DIGIT[0][0] = FND_e;

				DIGIT[1][3] = FND_OFF;				// Set ǥ��
				DIGIT[1][2] = FND_S;
				DIGIT[1][1] = FND_e;
				DIGIT[1][0] = FND_t;

				if(SET.Select) {ZigBee_Set();}			// ���׺� ���� ���
			break;

			case 5:							// Count = 5 �̸�
				DIGIT[0][3] = FND_OFF;				// rtc ǥ��
				DIGIT[0][2] = FND_r;
				DIGIT[0][1] = FND_t;
				DIGIT[0][0] = FND_c;

				DIGIT[1][3] = FND_OFF;				// Set ǥ��
				DIGIT[1][2] = FND_S;
				DIGIT[1][1] = FND_e;
				DIGIT[1][0] = FND_t;

				if(SET.Select) {RTC_Set();}			// RTC ���� ���
			break;

			case 6:							// Count = 6 �̸�
				Count = 1;					// �ݺ��� ���� Count �� ������ ���� ��Ŵ
			break;
		}
		wdt_reset();							// ��ġ�� ����
	}
	Var_Init();								// ���� �ʱ�ȭ
}

//=================================================================================================
void Seting_Mode_1(void)							// ���ø�� 1
{
	_delay_ms(500);								// ��� ���� �� ���� ���� ������ ���� ������

	while(SET.NumSET)							// �޴� �� ���� �� ��� 1 �̸� �ݺ�
	{
		if(SET.NumSET == 1)
		{
			if(KEY.Buff)						// Key �Է��� ������
			{
				if     (KEY.Key == 1 && Room_Number < 100) {Room_Number++; KEY.Key = 0;}		// ��  ��ư �̸� ���� ������ ����
				else if(KEY.Key == 3 && Room_Number > 1)   {Room_Number--; KEY.Key = 0;}		// �ٿ��ư �̸� ���� ������ ����
				else if(KEY.Key == 2)			   {SET.NumSET++; SET.Save = 0; KEY.Key = 0;}	// ���ù�ư �̸� SET.Room �ʱ�ȭ
			}

			if(Blink) {FND_Value_Data(1,1,12);}			// Blink = 1 �̸� FND 1 OFF
			else							// Blink = 0 �̸� FND 1 �� �� ��ȣ ǥ��
			{
				DIGIT[1][3] = FND_OFF;				// FND 1 �� �� ��ȣ ǥ��
				DIGIT[1][2] = fnd_tbl[Room_Number / 100];
				DIGIT[1][1] = fnd_tbl[Room_Number % 100 / 10];
				DIGIT[1][0] = fnd_tbl[Room_Number % 10];
			}
			DIGIT[2][3] = FND_OFF;					// FND 2 �� ���� ��ȣ ǥ��
			DIGIT[2][2] = fnd_tbl[Stal_Number / 100];
			DIGIT[2][1] = fnd_tbl[Stal_Number % 100 / 10];
			DIGIT[2][0] = fnd_tbl[Stal_Number % 10];
		}
		else if(SET.NumSET == 2)
		{
			if(KEY.Buff)						// Key �Է��� ������
			{
				if     (KEY.Key == 1 && Stal_Number < 100) {Stal_Number++; KEY.Key = 0;}		// ��  ��ư �̸� ���� ������ ����
				else if(KEY.Key == 3 && Stal_Number > 1)   {Stal_Number--; KEY.Key = 0;}		// �ٿ��ư �̸� ���� ������ ����
				else if(KEY.Key == 2)			   {SET.NumSET = 0; SET.Save = 0; KEY.Key = 0;}	// ���ù�ư �̸� SET.Room �ʱ�ȭ
			}

			if(Blink) {FND_Value_Data(2,2,12);}			// Blink = 1 �̸� FND 2 OFF
			else							// Blink = 0 �̸� FND 2�� ���� ��ȣ ǥ��
			{
				DIGIT[2][3] = FND_OFF;				// FND 2 �� ���� ��ȣ ǥ��
				DIGIT[2][2] = fnd_tbl[Stal_Number / 100];
				DIGIT[2][1] = fnd_tbl[Stal_Number % 100 / 10];
				DIGIT[2][0] = fnd_tbl[Stal_Number % 10];
			}
			DIGIT[1][3] = FND_OFF;					// FND 1 �� �� ��ȣ ǥ��
			DIGIT[1][2] = fnd_tbl[Room_Number / 100];
			DIGIT[1][1] = fnd_tbl[Room_Number % 100 / 10];
			DIGIT[1][0] = fnd_tbl[Room_Number % 10];
		}
		wdt_reset();							// ��ġ�� ����
	}
	SET.NumSET = 0;
	ee_write_byte(Cell_ROOM_Addr, Room_Number);
	ee_write_byte(Cell_STAL_Addr, Stal_Number);
	Alarm();
}

//=================================================================================================
void Room_Desplay(void)								// �� ��ȣ ǥ��
{
	DIGIT[1][3] = FND_OFF;							// FND 1 �� �� ��ȣ ǥ��
	DIGIT[1][2] = fnd_tbl[Room_Number / 100];
	DIGIT[1][1] = fnd_tbl[Room_Number % 100 / 10];
	DIGIT[1][0] = fnd_tbl[Room_Number % 10];
}

//=================================================================================================
void Stal_Desplay(void)								// ���� ��ȣ ǥ��
{
	DIGIT[2][3] = FND_OFF;							// FND 2 �� ���� ��ȣ ǥ��
	DIGIT[2][2] = fnd_tbl[Stal_Number / 100];
	DIGIT[2][1] = fnd_tbl[Stal_Number % 100 / 10];
	DIGIT[2][0] = fnd_tbl[Stal_Number % 10];
}

//=================================================================================================
void Check_Value(void)								// �ε弿 ü������
{
	uint32_t Value = Read_Load_Cell();					// �ε弿 ������ �б�
	uint8_t cnt_1 = 3;							// OK_Count ����
	uint8_t cnt_2 = 12;							// OK_Count ����
//-------------------------------------------------------------------------------------------------
	if(Value)								// ����ü���� ������
	{
		Sec_Count = 0;

		if((OK_Count_1 >= cnt_1) || (OK_Count_2 >= cnt_2))		// ����ü�� OK Count ���� ũ��
		{
			FND_Value_Data(1, 1, 10);				// FND 1 FND_MINUS ǥ��
			if((SET.Save || Auto_Mode) && !CELL.Zero_Buff)		// ���� Key �� ���Ȱų� �ڵ���� �̸鼭 CELL.Zero_Buff �� 0�̸�
			{
				if(Data_Number < 299) {Data_Number++;}		// ����Ƚ�� �� 0 ~ 299(300��)���� ������ ����Ƚ�� ����
				else		      {Data_Number = 0;}	// �ƴϸ� ����Ƚ�� �ʱ�ȭ
//-------------------------------------------------------------------------------------------------
				SAVE.AutoSet[Data_Number] = Auto_Mode;
				SAVE.Count  [Data_Number] = Data_Number;
				SAVE.Data   [Data_Number] = CELL.Value = CELL.Old_Value;
				SAVE.RoomNum[Data_Number] = Room_Number;
				SAVE.StalNum[Data_Number] = Stal_Number;
				SAVE.Year   [Data_Number] = RTC_Single_Read(RTC_YEAR);		// YEAR
				SAVE.Month  [Data_Number] = RTC_Single_Read(RTC_MONTH);		// MONTH
				SAVE.Date   [Data_Number] = RTC_Single_Read(RTC_DATE);		// DATE
				SAVE.Hour   [Data_Number] = RTC_Single_Read(RTC_HOUR);		// HOUR
				SAVE.Min    [Data_Number] = RTC_Single_Read(RTC_MINUTE);	// MINUTE
				SAVE.Sec    [Data_Number] = RTC_Single_Read(RTC_SECOND);	// SECOND

				if(EepCnt < 35) {EepCnt++;}			// EEPROM ����Ƚ�� �� 0 ~ 34(35��)���� ������ ����Ƚ�� ����
				else		{EepCnt = 0;}			// �ƴϸ� EEPROM ����Ƚ�� �ʱ�ȭ
//-------------------------------------------------------------------------------------------------
				ee_write_byte(MEASURE_AUTO   + (MEASURE_LENGTH * EepCnt), Auto_Mode);
				ee_write_word(MEASURE_COUNT  + (MEASURE_LENGTH * EepCnt), Data_Number);
				ee_write_word(MEASURE_WEIGHT + (MEASURE_LENGTH * EepCnt), CELL.Value = CELL.Old_Value);
				ee_write_byte(ROOM_NUMBER    + (MEASURE_LENGTH * EepCnt), Room_Number);
				ee_write_byte(STALL_NUMBER   + (MEASURE_LENGTH * EepCnt), Stal_Number);
				ee_write_byte(MEASURE_YEAR   + (MEASURE_LENGTH * EepCnt), RTC_Single_Read(RTC_YEAR));
				ee_write_byte(MEASURE_MONTH  + (MEASURE_LENGTH * EepCnt), RTC_Single_Read(RTC_MONTH));
				ee_write_byte(MEASURE_DATE   + (MEASURE_LENGTH * EepCnt), RTC_Single_Read(RTC_DATE));
				ee_write_byte(MEASURE_HOUR   + (MEASURE_LENGTH * EepCnt), RTC_Single_Read(RTC_HOUR));
				ee_write_byte(MEASURE_MIN    + (MEASURE_LENGTH * EepCnt), RTC_Single_Read(RTC_MINUTE));
				ee_write_byte(MEASURE_SEC    + (MEASURE_LENGTH * EepCnt), RTC_Single_Read(RTC_SECOND));
				ee_write_word(Data_Count_Addr, EepCnt);

//-------------------------------------------------------------------------------------------------
				CELL.Zero_Buff = 1;
				SET.Save = 0;
				Alarm();
				printf("Value %ld, %d, %d \r\n", CELL.Old_Value, OK_Count_1, OK_Count_2);
				Cell_Desplay();					// �ε弿 ���� ǥ��
			}
			Stal_Desplay();						// ���� ��ȣ ǥ��
		}
//-------------------------------------------------------------------------------------------------
		else if((CELL.Old_Value <= Value + Admit_Weight) && (CELL.Old_Value >= Value - Admit_Weight))
		{								// ����ü���� ����ü�߰� ����ü��(5x10g)�� ���Ѱ��� ���ų� �۰� ����ü��(5x10g)�� ������ ���ų� ũ��
			if(SET.Save || Auto_Mode)				// �ڵ� ����϶� ����ü�� ��ȭ�� ������
			{
				OK_Count_1 += 1;				// OK Count_1 1 ����
				OK_Count_2 += 3;				// OK Count_2 3 ����
			}
			CELL.Old_Value = (CELL.Old_Value + Value)/2;		// ����ü�߿� ����ü�� ����
			CELL.Value = CELL.Old_Value;				// ����ü���� ����ǥ�� ü�� �� ����
			CELL.Over_Value = CELL.Old_Value;			// ����ü���� ���� �� ü�߰��� ����
		}
 		else if((CELL.Over_Value <= Value + (Admit_Weight * 5)) && (CELL.Over_Value >= Value - (Admit_Weight * 5)))
		{								// ����ü���� ����ü�߰� ����ü��((5x5)x10g)�� ���Ѱ��� ���ų� �۰� ����ü���� ������ ���ų� ũ��
			if(SET.Save || Auto_Mode) {OK_Count_2 += 2;}		// �ڵ� ����϶� ����ü�� OK Count 2 = 2 ����
			CELL.Old_Value = (CELL.Over_Value + Value)/2;		// ����ü�߿� ����ü�� ����
			CELL.Value = CELL.Old_Value;				// ����ü���� ����ǥ�� ü�� �� ����
			CELL.Over_Value = CELL.Old_Value;			// ����ü���� ���� �� ü�߰��� ����
		}
		else if((CELL.Over_Value <= Value + (Admit_Weight * 10)) && (CELL.Over_Value >= Value - (Admit_Weight * 10)))
		{								// ����ü���� ����ü�߰� ����ü��((5x10)x10g)�� ���Ѱ��� ���ų� �۰� ����ü���� ������ ���ų� ũ��
			if(SET.Save || Auto_Mode) {OK_Count_2 += 1;}		// �ڵ� ����϶� ����ü�� OK Count 2 ����
			CELL.Old_Value = (CELL.Over_Value + Value)/2;		// ����ü�߿� ����ü�� ����
			CELL.Value = CELL.Old_Value;				// ����ü���� ����ǥ�� ü�� �� ����
			CELL.Over_Value = CELL.Old_Value;			// ����ü���� ���� �� ü�߰��� ����
		}
		else
		{
			CELL.Value = CELL.Over_Value = Value;			// ����ü���� ���� �� ü�߰��� ����
			OK_Count_1 = 0;						// ü�� ��ȭ�� ũ�� OK_Count 1 �ʱ�ȭ
		}
		Room_Desplay();							// �� ��ȣ ǥ��
		Zero_Count = 0;
		if(!CELL.Zero_Buff) {Cell_Desplay();}				// �ε弿 ���� ǥ��
	}
	else									// ����ü���� ����
	{
		Zero_Count++;							// Zero_Count ����
		if(Zero_Count > 5)						// Zero_Count �� 2���� ũ��
		{
			CELL.Old_Value = 0;					// ��������ü�� �ʱ�ȭ
			CELL.Over_Value = 0;					// ��������ü�� �ʱ�ȭ
			CELL.Value = Value;
			CELL.Zero_Buff = 0;					// CELL.Zero_Buff �ʱ�ȭ
			Zero_Count = 0;						// Zero_Count �ʱ�ȭ
			OK_Count_1 = 0;						// OK_Count 1 �ʱ�ȭ
			OK_Count_2 = 0;						// OK_Count 2 �ʱ�ȭ
			if(!SET.Save) {Stal_Desplay();}				// Save ��尡 �ƴϸ� ���� ��ȣ ǥ��
			Room_Desplay();						// �� ��ȣ ǥ��
			Cell_Desplay();						// ����ü�� ǥ��
		}
	}
}

//=================================================================================================
//		PC�� ������ ���� (20�� ����ü�� Data)
//=================================================================================================
void Send_To_Server_0(void)							// PC�� ������ ���� (1ȸ 1���� ���� : ����ü�� Data)
{
	uint8_t cnt = 0;							// cnt ���� �ʱ�ȭ
	uint8_t ChkSum = 0;							// ChkSum ���� �ʱ�ȭ
	char SerialData[30] = {0};						// SerialData[30] ���� ����
	static uint16_t CommCnt	= 0;

	memset(SerialData, 0x00, sizeof(SerialData));				// ���� �ʱ�ȭ

	if(CommCnt < Data_Number) {CommCnt++;}
	else			  {CommCnt=0;}
//-------------------------------------------------------------------------------------------------
//		Serial Protocol Start
//-------------------------------------------------------------------------------------------------
	SerialData[cnt++] = STX_Iontec;						//  0 : STX_Iontec : 0xF0
	SerialData[cnt++] = CTL2PC;						//  1 : �����͹���
	SerialData[cnt++] = SORTING_MOVE;					//  2 : 21 ������ : �̵��� ������(�����ڵ�, ������)
	SerialData[cnt++] = Cell_ID;						//  3 : ���id (Cell_ID)
	SerialData[cnt++] = CD_STATUS;						//  4 : 0 : �����ڵ� (�ǽð����� ����)

	SerialData[cnt++] = FW_VER;						//  5 : Protocol Version 41
	SerialData[cnt++] = Gwid;						//  6 : GWID
	SerialData[cnt++] = 0x00;						//  7 : ��ü����
	SerialData[cnt++] = SAVE.AutoSet[CommCnt];				//  8 : �ڵ� 1 / ���� 0
	SerialData[cnt++] = 0x02;						//  9 : ���� �Ҽ��� ���:1, ���� �Ҽ��� �̻��:0, 10g����:2

	SerialData[cnt++] = SAVE.Count[CommCnt] / 256;				// 10 : ����Ƚ�� ����
	SerialData[cnt++] = SAVE.Count[CommCnt] % 256;				// 11 : ����Ƚ�� ����
	SerialData[cnt++] = SAVE.Data[CommCnt] / 256;				// 12 : �������� [CommCnt] ����
	SerialData[cnt++] = SAVE.Data[CommCnt] % 256;				// 13 : �������� [CommCnt] ����

	SerialData[cnt++] = SAVE.RoomNum[CommCnt];				// 14 : �� ��ȣ
	SerialData[cnt++] = SAVE.StalNum[CommCnt];				// 15 : ���� ��ȣ

	SerialData[cnt++] = SAVE.Year[CommCnt];					// 16 : ���� �� [CommCnt]
	SerialData[cnt++] = SAVE.Month[CommCnt];				// 17 : ���� �� [CommCnt]
	SerialData[cnt++] = SAVE.Date[CommCnt];					// 18 : ���� �� [CommCnt]
	SerialData[cnt++] = SAVE.Hour[CommCnt];					// 19 : ���� �� [CommCnt]
	SerialData[cnt++] = SAVE.Min[CommCnt];					// 20 : ���� �� [CommCnt]
	SerialData[cnt++] = SAVE.Sec[CommCnt];					// 21 : ���� �� [CommCnt]

	SerialData[cnt++] = CommCnt / 256;					// 22 : Data ���� ����
	SerialData[cnt++] = CommCnt % 256;					// 23 : Data ���� ����

	SerialData[cnt++] = 0x00;						// 24 : ����
	SerialData[cnt++] = 0x00;						// 25 : ����
	SerialData[cnt++] = 0x00;						// 26 : ����
	SerialData[cnt++] = 0x00;						// 27 : ����

	for(uint8_t icnt2 = 1; icnt2 < cnt; icnt2++) {ChkSum += SerialData[icnt2];}	// üũ�� ���
	SerialData[cnt++] = ChkSum;						// 28 : üũ��
	SerialData[cnt++] = ETX_Iontec;						// 29 : ETX_Iontec : 0xF1
//-------------------------------------------------------------------------------------------------
//		Serial Protocol End
//-------------------------------------------------------------------------------------------------
	for(uint8_t j = 0; j < cnt; j++) {USART3_Write(SerialData[j]);}		// SerialData ����ŭ �۽�
}

//=================================================================================================
//		PC�� ������ ���� (�������� �亯)
//=================================================================================================
void Send_To_Server_1(uint8_t Code)						// PC�� ������ ���� (�������� �亯)
{
	uint8_t cnt = 0;							// cnt ���� �ʱ�ȭ
	uint8_t ChkSum = 0;							// ChkSum ���� �ʱ�ȭ
	char SerialData[10] = {};						// SerialData[10] ���� ����
	memset(SerialData, 0x00, sizeof(SerialData));				// ���� �ʱ�ȭ
//-------------------------------------------------------------------------------------------------
//		Serial Protocol Start
//-------------------------------------------------------------------------------------------------
	SerialData[cnt++] = STX_Iontec;						// 0 : STX_Iontec : 0xF0
	SerialData[cnt++] = CTL2PC;						// 1 : �����͹���
	SerialData[cnt++] = SORTING_MOVE;					// 2 : 21 ������ : �̵��� ������(�����ڵ�, ������)
	SerialData[cnt++] = Cell_ID;						// 3 : ���id (Cell_ID)
	SerialData[cnt++] = Code;						// 4 : �����ڵ�

	SerialData[cnt++] = FW_VER;						// 5 : Protocol Version
	SerialData[cnt++] = Gwid;						// 6 : GWID
	SerialData[cnt++] = 0x00;						// 7 : ����
	for(uint8_t icnt2 = 1; icnt2 < cnt; icnt2++) {ChkSum += SerialData[icnt2];}	// üũ�� ���
	SerialData[cnt++] = ChkSum;						// 8 : üũ��
	SerialData[cnt++] = ETX_Iontec;						// 9 : ETX_Iontec : 0xF1
//-------------------------------------------------------------------------------------------------
//		Serial Protocol End
//-------------------------------------------------------------------------------------------------
	for(uint8_t j = 0; j < cnt; j++) {USART3_Write(SerialData[j]);}		// SerialData ����ŭ �۽�
}

//=================================================================================================
//		PC�� ���� �����ͼ���
//=================================================================================================
void Receiv_PC(void)								// PC�� ���� �����ͼ���
{
	uint8_t cnt1;								// cnt1 ���� �ʱ�ȭ
	uint8_t ChkSum = 0;							// ChkSum ���� �ʱ�ȭ
	uint16_t IN_S;								// �Ա� ü�� ����

	for(uint8_t cnt = 0; cnt < RX3_SIZE - 31; cnt++)			// RX3_SIZE ��ŭ �ݺ�
	{
		if(Rx3_Buff[cnt] == STX_Iontec && Rx3_Buff[cnt + 1] == PC2CTL && Rx3_Buff[cnt + 2] == 0x01 && Rx3_Buff[cnt + 4] == 0 && Rx3_Buff[cnt + 9] == ETX_Iontec)
		{//			STX			       PC2CTL			     ������			�ǽð�����			ETX
			Rx_LED_ON;						// RX LED ON
			ChkSum = 0;						// ChkSum �ʱ�ȭ

			for(cnt1 = 1; cnt1 < 8; cnt1++) {ChkSum += Rx3_Buff[cnt + cnt1];}	// üũ�� ���

			if(ChkSum == Rx3_Buff[cnt + 8])				// üũ�� Ȯ��
			{
				if(Rx3_Buff[cnt + 3] == Cell_ID)		// ID 1�� �̸�
				{						// �ǽð� ����
					Tx_LED_ON;				// TX LED ON
					Send_To_Server_0();			// ���� Data PC�� ����
					_delay_ms(2);
					Tx_LED_OFF;				// TX LED OFF
				}
			}
			Rx3_Buff_Clear();					// Rx3 Buff Clear
			Rx_LED_OFF;						// RX LED OFF
			break;							// for �� ���� ������
		}

		else if(Rx3_Buff[cnt] == STX_Iontec && Rx3_Buff[cnt + 1] == PC2CTL && Rx3_Buff[cnt + 4] == 9 && Rx3_Buff[cnt + 15] == ETX_Iontec)
		{//			    STX				    PC2CTL		    �ð���������			        ETX
			Rx_LED_ON;						// RX LED ON
			ChkSum = 0;						// ChkSum �ʱ�ȭ

			for(cnt1 = 1; cnt1 < 14; cnt1++) {ChkSum += Rx3_Buff[cnt + cnt1];}	// üũ�� ���

			if(ChkSum == Rx3_Buff[cnt + 14])			// üũ�� Ȯ��
			{
				RTC_Time_Write(RTC_YEAR,  ((Rx3_Buff[cnt+ 8]&0xF0)>>4)*10+(Rx3_Buff[cnt+ 8]&0x0F));	// RTC �� �� ����
				RTC_Time_Write(RTC_MONTH, ((Rx3_Buff[cnt+ 9]&0xF0)>>4)*10+(Rx3_Buff[cnt+ 9]&0x0F));	// RTC �� �� ����
				RTC_Time_Write(RTC_DATE,  ((Rx3_Buff[cnt+10]&0xF0)>>4)*10+(Rx3_Buff[cnt+10]&0x0F));	// RTC �� �� ����
				RTC_Time_Write(RTC_HOUR,  ((Rx3_Buff[cnt+11]&0xF0)>>4)*10+(Rx3_Buff[cnt+11]&0x0F));	// RTC �� �� ����
				RTC_Time_Write(RTC_MINUTE,((Rx3_Buff[cnt+12]&0xF0)>>4)*10+(Rx3_Buff[cnt+12]&0x0F));	// RTC �� �� ����
				RTC_Time_Write(RTC_SECOND,((Rx3_Buff[cnt+13]&0xF0)>>4)*10+(Rx3_Buff[cnt+13]&0x0F));	// RTC �� �� ����
			}
			Rx3_Buff_Clear();					// Rx3 Buff Clear
			Rx_LED_OFF;						// RX LED OFF
			break;							// for �� ���� ������
		}

		else if(Rx3_Buff[cnt] == STX_Iontec && Rx3_Buff[cnt + 1] == PC2CTL && Rx3_Buff[cnt + 2] == 0x01 && Rx3_Buff[cnt + 4] == 7 && Rx3_Buff[cnt + 31] == ETX_Iontec)
		{//			     STX			    PC2CTL			  ������		��������(�Ա�ü��)		       ETX
			Rx_LED_ON;							// RX LED ON
			ChkSum = 0;							// ChkSum �ʱ�ȭ

			for(cnt1 = 1; cnt1 < 30; cnt1++) {ChkSum += Rx3_Buff[cnt + cnt1];}	// üũ�� ���

			if(ChkSum == Rx3_Buff[cnt + 30])				// üũ�� Ȯ��
			{
//				IN_S = Rx3_Buff[cnt + 8] << 8 & Rx3_Buff[cnt + 8];	// �Ա�ü�� �� ��ȯ
				IN_S = (Rx3_Buff[cnt + 8] << 8) | Rx3_Buff[cnt + 9];	// �Ա�ü�� �� ��ȯ
				if(Rx3_Buff[cnt + 3] == Cell_ID)			// Cell ID 1�� �̸�
				{
					IN_Scales = IN_S;				// �Ա�ü�� ���� �ε弿 1���� ����
					ee_write_word(Cell_IN_S_Addr, IN_S);		// �Ա�ü�� ���� EEPROM�� ����
					Tx_LED_ON;					// TX LED ON
					Send_To_Server_1(Rx3_Buff[cnt + 4]);		// �������� PC�� ����
					_delay_ms(2);
					Tx_LED_OFF;					// TX LED OFF
				}
			}
		}
	}
}

//=================================================================================================
void Time_Check(void)								// �ð� üũ
{
	if(Timer.RTC_Timer >= 1000)						// 5mSec X 1000 = 5Sec �̻��� �Ǹ� ����
	{
		Timer.RTC_Timer = 0;						// RTC_Timer �ʱ�ȭ (Ÿ�̸�1 ���� 5mSec ���� ����)
		RTC_Time_Read();						// RTC���� �ð� �о� ����

		if(RTC_T.OLD_D != RTC_T.Date)					// ��¥ Data �� �ٸ���
		{
			RTC_T.OLD_D = RTC_T.Date;				// ���� ��¥ ����
			ee_write_byte (OLD_Date_Addr, RTC_T.OLD_D);		// ��¥ �� EEPROM �� ����

			memset(SAVE.AutoSet, 0x00, sizeof(SAVE.AutoSet));	// �ڵ����� ���� Clear
			memset(SAVE.Count,   0x00, sizeof(SAVE.Count));		// ����Ƚ�� ���� Clear
			memset(SAVE.Data,    0x00, sizeof(SAVE.Data));		// �������� ���� Clear
			memset(SAVE.RoomNum, 0x00, sizeof(SAVE.RoomNum));	// ���ȣ ���� Clear
			memset(SAVE.StalNum, 0x00, sizeof(SAVE.StalNum));	// �����ȣ ���� Clear
			memset(SAVE.Year,    0x00, sizeof(SAVE.Year));		// �� ���� Clear
			memset(SAVE.Month,   0x00, sizeof(SAVE.Month));		// �� ���� Clear
			memset(SAVE.Date,    0x00, sizeof(SAVE.Date));		// �� ���� Clear
			memset(SAVE.Hour,    0x00, sizeof(SAVE.Hour));		// �� ���� Clear
			memset(SAVE.Min,     0x00, sizeof(SAVE.Min));		// �� ���� Clear
			memset(SAVE.Sec,     0x00, sizeof(SAVE.Sec));		// �� ���� Clear

			Data_Number = 0;					// ���� ���� Count Clear
			EepCnt = 0;						// EEPROM COUNT Clear

			for(uint8_t i = 0; i < 35; i++)				// Eeprom ���� ������ŭ �ݺ�
			{
				eeprom_Data_erase(i);				// Eeprom Data �����
			}
		}
		Sec_Count++;							// Sec_Count ����
		if(Sec_Count > 50) {Sec_Count = 50;}				// Sec_Count �� 10 ���� ũ�� 10 ���� ����
	}
}

//=================================================================================================
void Proportion_Set(void)							// �ε弿 ���� ���� �� �ʱ�ȭ
{
//	CELL.Proportion = (Cell_RO / 10000000 / 4 * 128 / 0.5 * 8388608 / (Cell_Capa * 1000)) / Cell_CNT;
//	�ε弿 ���� = (RO ���� / 10000000(mV) / ADC �Է����� X ���� / 0.5 * ADC Bit/ (CAPA ���� X 1000)) / �ε弿����

	CELL.Proportion = ((double)Cell_RO / 10000000 / 4 * 128 / 0.5 * 8388608 / (Cell_Capa * 1000)) / Cell_CNT;
//	�ε弿 ���� =      (RO ���� / 10000000(mV) / ADC �Է����� X ���� / 0.5 * ADC Bit/ (CAPA ���� X 1000)) / �ε弿����

}

//=================================================================================================
void Var_Init(void)								// ���� �� �ε弿 �ʱ�ȭ
{
	FND_Value_Data(0, 2, 10);						// FND 0 ~ 2 FND_MINUS

	Read_eeprom();								// EEPROM Data �б�
	_delay_ms(1);

	if(RTC_T.OLD_D != RTC_T.Date)						// ��¥ Data �� �ٸ���
	{
		RTC_T.OLD_D = RTC_T.Date;					// ���� ��¥ ����
		ee_write_byte (OLD_Date_Addr, RTC_T.OLD_D);			// ��¥ �� EEPROM �� ����

		for(uint8_t i = 0; i < 35; i++)					// Eeprom ���� ������ŭ �ݺ�
		{
			eeprom_Data_erase(i);					// Eeprom Data �����
		}
		Data_Number = 0;						// ���� ���� Count Clear
		EepCnt = 0;							// EEPROM COUNT Clear
	}
	else
	{
		for(uint8_t i = 0; i < 35; i++)					// Eeprom ���� ������ŭ �ݺ�
		{
			Read_eeprom_Weight_Data(i);				// Eeprom Data �ҷ�����
		}
	}

	Proportion_Set();							// �ε弿 ���� ���� �� �ʱ�ȭ
	_delay_ms(1);

	Cell_Zero_Set();							// �ε弿 0 �� ã�� 2ȸ �ݺ�
	_delay_ms(1);
	Cell_Zero_Set();
	Alarm();								// �˶�(���� 3��)

	CELL.Zero_Buff = 0;							// ���� �� Zero Buff �ʱ�ȭ
	Sec_Count = 0;								// �����ð�(30��) ��ư �Է� ������ FND OFF Timer �ʱ�ȭ
	SET.Menu = 0;								// Menu   ��� �ʱ�ȭ
	SET.Select = 0;								// Select ��� �ʱ�ȭ
	SET.Blink = 0;								// �ڸ��� ������ �ʱ�ȭ
	SET.NumSET = 0;
	KEY.Count = 0;								// Key �� ����Ƚ�� �ʱ�ȭ
	OK_Count_1 = 0;								// OK_Count �ʱ�ȭ
	OK_Count_2 = 0;								// OK_Count �ʱ�ȭ

	FND_Value_Data(0, 2, 12);						// FND 0 ~ 2 FND_MINUS
}

//=================================================================================================
int main(void)
{
	atmel_start_init();
	MCU_Init();								// MCU Port �ʱ�ȭ
//-------------------------------------------------------------------------------------------------
#ifdef USART_SET								// USART ��뼳��
	fdevopen(std_putchar, NULL);						// printf �Լ���� ����
	printf_port = 1;							// Usart Port 1 ���
	_delay_ms(50);								// ��� Port ����ȭ Time

	printf("\r\n");
	printf("//=================================================//\r\n");
	printf("//===  Product Name : Pig Scales 2 V%d.%d(SC-500) ===//\r\n",FW_VER / 10, FW_VER % 10);	// ProductName
	printf("//===  File Content : Moving Weight Scales      ===//\r\n");				// File ���� : �̵��� ü�� ������
	printf("//===  Compiled     : %s, %s     ===//\r\n", __DATE__, __TIME__);			// Compiled : ��¥, �ð� ǥ��
	printf("//=================================================//\r\n");
	printf("\r\n");
#endif
//-------------------------------------------------------------------------------------------------
// #ifdef ADC_SET								// ADC ��뼳��
// 	ADC_init();								// ADC �ʱ�ȭ �� ����
// 	VREF_ADC_init();
// 	ADC_is_conversion_done();
// #endif
//-------------------------------------------------------------------------------------------------
// #ifdef PWM_SET								// PWM ��뼳��
// 	PWM_0_enable();								// PWM 0 �ʱ�ȭ �� ����
// 	PWM_0_enable_output_ch0();
// #endif
//-------------------------------------------------------------------------------------------------
	sei();									// ��ü ���ͷ��� ����
	Alarm();								// �˶�(���� 3��)
	Rx_LED_OFF;								// RX LED OFF
	Tx_LED_OFF;								// TX LED OFF
//-------------------------------------------------------------------------------------------------
	DIGIT[0][3] = FND_OFF;							// �߿��� ���� ǥ��
	DIGIT[0][2] = fnd_tbl[FW_VER / 10] + FND_Point;
	DIGIT[0][1] = fnd_tbl[FW_VER % 10];
	DIGIT[0][0] = FND_OFF;
	_delay_ms(1000);							// 1�� ���� ǥ��
	wdt_reset();								// Reset WT

	Time_Count = 5;								// �ð� ǥ�� �� ���� ����
	Sec_Count = 0;
	SECOND = 0;								// ���� �ð�(��)�� ���� ����
	Read_eeprom();
//-------------------------------------------------------------------------------------------------
	while(Time_Count)							// Time_Count �� 0�� �ƴϸ� �ݺ�(5�ʵ��� �ð�ǥ��)
	{
		RTC_Time_Read();						// RTC Data �б�

		DIGIT[0][3] = fnd_tbl[RTC_T.Year / 10];				// �� ǥ��
		DIGIT[0][2] = fnd_tbl[RTC_T.Year % 10];

		DIGIT[0][1] = fnd_tbl[RTC_T.Month / 10];			// �� ǥ��
		DIGIT[0][0] = fnd_tbl[RTC_T.Month % 10];

		DIGIT[1][3] = fnd_tbl[RTC_T.Date / 10];				// �� ǥ��
		DIGIT[1][2] = fnd_tbl[RTC_T.Date % 10];

		DIGIT[1][1] = fnd_tbl[RTC_T.Hour / 10];				// �� ǥ��
		DIGIT[1][0] = fnd_tbl[RTC_T.Hour % 10];

		DIGIT[2][3] = fnd_tbl[RTC_T.Min / 10];				// �� ǥ��
		DIGIT[2][2] = fnd_tbl[RTC_T.Min % 10];

		DIGIT[2][1] = fnd_tbl[RTC_T.Sec / 10];				// �� ǥ��
		DIGIT[2][0] = fnd_tbl[RTC_T.Sec % 10];

		if(SECOND != RTC_T.Sec)						// ���� �ð�(��)�� ����ð�(��)�� ���� ������
		{
			SECOND = RTC_T.Sec;					// ���� �ð�(��)�� ����ð�(��) ����
			Beep(10);						// ����(1�ʿ� 1����)
			wdt_reset();						// Reset WDT
			Time_Count--;						// Time_Count ����
		}
	}
	Var_Init();								// ���� �� �ε弿 �ʱ�ȭ
	wdt_reset();								// Reset WDT

//-------------------------------------------------------------------------------------------------
	while(1)
	{
 		if(Sec_Count < 360) {Check_Value();}				// 5Sec x 360 = 1800Sec(30��) ���� ũ�� FND ALL OFF
		else
		{
			while(!KEY.Buff)
			{
				FND_ALL_OFF();					// FND ����
  				Receiv_PC();					// PC�� ���� �����ͼ��� Ȯ��
  				Time_Check();					// �ð� üũ(��¥���� Ȯ��)
  				_delay_ms(50);					// ���� ������ �������� Delay
				wdt_reset();					// Reset WDT
			}
			KEY.Buff = 0;
			Var_Init();						// ���� �� �ε弿 �ʱ�ȭ
		}

		if(SET.Menu)   {Seting_Mode_0();}				// SET Menu   �� �����Ǿ� ������ Seting_Mode_0 ����
		if(SET.NumSET) {Seting_Mode_1();}				// SET.NumSET �� �����Ǿ� ������ Seting_Mode_1 ����
		Receiv_PC();							// PC�� ���� �����ͼ��� Ȯ��
		Time_Check();							// �ð� üũ(��¥���� Ȯ��)
		_delay_ms(50);							// ���� ������ �������� Delay
		wdt_reset();							// Reset WDT
	}
}

//=================================================================================================
#ifdef USART_SET
int std_putchar(char c,FILE *stream)						// printf data �� ������ USART Port �� ����
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
