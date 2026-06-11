//=================================================================================================
//		File Name	:	main.h
//		File 내용	:	main 펌웨어 헤더
//=================================================================================================
#include <atmel_start.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>

//=================================================================================================
//		설정 재조정
//=================================================================================================
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |=  _BV(bit))
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
// #define tbi(sfr, bit) (_SFR_BYTE(sfr) ^= _BV(bit))

// #ifndef TYPE_REDEFINE
// #define TYPE_REDEFINE
typedef unsigned long DWORD;
typedef unsigned int  WORD;
typedef unsigned char BYTE;

typedef volatile unsigned long VDWORD;
typedef volatile unsigned int  VWORD;
typedef volatile unsigned char VBYTE;

//=================================================================================================
//		PORT A Control Bit
//=================================================================================================
// #define xxx_ON		sbi(PORTA_OUT, 0);		// PORTA_OUT |=  0x01
// #define xxx_OFF		cbi(PORTA_OUT, 0);		// PORTA_OUT &= ~0x01

#define Buzzer_ON		sbi(PORTA_OUT, 1);		// PORTA_OUT |=  0x02
#define Buzzer_OFF		cbi(PORTA_OUT, 1);		// PORTA_OUT &= ~0x02

// #define xxx_ON		sbi(PORTA_OUT, 2);		// PORTA_OUT |=  0x04
// #define xxx_OFF		cbi(PORTA_OUT, 2);		// PORTA_OUT &= ~0x04

// #define xxx_ON		sbi(PORTA_OUT, 3);		// PORTA_OUT |=  0x08
// #define xxx_OFF		cbi(PORTA_OUT, 3);		// PORTA_OUT &= ~0x08

// #define xxx_ON		sbi(PORTA_OUT, 4);		// PORTA_OUT |=  0x10
// #define xxx_OFF		cbi(PORTA_OUT, 4);		// PORTA_OUT &= ~0x10

// #define xxx_ON		sbi(PORTA_OUT, 5);		// PORTA_OUT |=  0x20
// #define xxx_OFF		cbi(PORTA_OUT, 5);		// PORTA_OUT &= ~0x20

// #define xxx_ON		sbi(PORTA_OUT, 6);		// PORTA_OUT |=  0x40
// #define xxx_OFF		cbi(PORTA_OUT, 6);		// PORTA_OUT &= ~0x40

// #define xxx_ON		sbi(PORTA_OUT, 7);		// PORTA_OUT |=  0x80
// #define xxx_OFF		cbi(PORTA_OUT, 7);		// PORTA_OUT &= ~0x80


// #define TOUCH_STATE		(PING & 0x08) >> 3
// #define MAGNET_STATE		(PING & 0x10) >> 4


//=================================================================================================
//		PORT B Control Bit
//=================================================================================================
// #define xxx_ON		sbi(PORTB_OUT, 0);		// PORTB_OUT |=  0x01
// #define xxx_OFF		cbi(PORTB_OUT, 0);		// PORTB_OUT &= ~0x01

// #define xxx_ON		sbi(PORTB_OUT, 1);		// PORTB_OUT |=  0x02
// #define xxx_OFF		cbi(PORTB_OUT, 1);		// PORTB_OUT &= ~0x02

#define Tx_LED_ON		sbi(PORTB_OUT, 2);		// PORTB_OUT |=  0x04
#define Tx_LED_OFF		cbi(PORTB_OUT, 2);		// PORTB_OUT &= ~0x04

#define Rx_LED_ON		sbi(PORTB_OUT, 3);		// PORTB_OUT |=  0x08
#define Rx_LED_OFF		cbi(PORTB_OUT, 3);		// PORTB_OUT &= ~0x08

// #define xxx_ON		sbi(PORTB_OUT, 4);		// PORTB_OUT |=  0x10
// #define xxx_OFF		cbi(PORTB_OUT, 4);		// PORTB_OUT &= ~0x10

// #define xxx_ON		sbi(PORTB_OUT, 5);		// PORTB_OUT |=  0x20
// #define xxx_OFF		cbi(PORTB_OUT, 5);		// PORTB_OUT &= ~0x20

// #define xxx_ON		sbi(PORTB_OUT, 6);		// PORTB_OUT |=  0x40
// #define xxx_OFF		cbi(PORTB_OUT, 6);		// PORTB_OUT &= ~0x40

// #define xxx_ON		sbi(PORTB_OUT, 7);		// PORTB_OUT |=  0x80
// #define xxx_OFF		cbi(PORTB_OUT, 7);		// PORTB_OUT &= ~0x80

//=================================================================================================
//		PORT C Control Bit
//=================================================================================================
// #define xxx_ON		sbi(PORTC_OUT, 0);		// PORTC_OUT |=  0x01
// #define xxx_OFF		cbi(PORTC_OUT, 0);		// PORTC_OUT &= ~0x01

// #define xxx_ON		sbi(PORTC_OUT, 1);		// PORTC_OUT |=  0x02
// #define xxx_OFF		cbi(PORTC_OUT, 1);		// PORTC_OUT &= ~0x02

#define SCK_HIGH_1		sbi(PORTC_OUT, 2);		// PORTC_OUT |=  0x04
#define SCK_LOW_1		cbi(PORTC_OUT, 2);		// PORTC_OUT &= ~0x04

// #define xxx_ON		sbi(PORTC_OUT, 3);		// PORTC_OUT |=  0x08
// #define xxx_OFF		cbi(PORTC_OUT, 3);		// PORTC_OUT &= ~0x08

// #define SCK_HIGH_2		sbi(PORTC_OUT, 4);		// PORTC_OUT |=  0x10
// #define SCK_LOW_2		cbi(PORTC_OUT, 4);		// PORTC_OUT &= ~0x10

// #define xxx_ON		sbi(PORTC_OUT, 5);		// PORTC_OUT |=  0x20
// #define xxx_OFF		cbi(PORTC_OUT, 5);		// PORTC_OUT &= ~0x20

// #define SCK_HIGH_3		sbi(PORTC_OUT, 6);		// PORTC_OUT |=  0x40
// #define SCK_LOW_3		cbi(PORTC_OUT, 6);		// PORTC_OUT &= ~0x40

// #define xxx_ON		sbi(PORTC_OUT, 7);		// PORTC_OUT |=  0x80
// #define xxx_OFF		cbi(PORTC_OUT, 7);		// PORTC_OUT &= ~0x80

//=================================================================================================
//		PORT D Control Bit
//=================================================================================================
// #define xxx_ON		sbi(PORTD_OUT, 0);		// PORTD_OUT |=  0x01
// #define xxx_OFF		cbi(PORTD_OUT, 0);		// PORTD_OUT &= ~0x01

// #define xxx_ON		sbi(PORTD_OUT, 1);		// PORTD_OUT |=  0x02
// #define xxx_OFF		cbi(PORTD_OUT, 1);		// PORTD_OUT &= ~0x02

// #define xxx_ON		sbi(PORTD_OUT, 2);		// PORTD_OUT |=  0x04
// #define xxx_OFF		cbi(PORTD_OUT, 2);		// PORTD_OUT &= ~0x04

// #define xxx_ON		sbi(PORTD_OUT, 3);		// PORTD_OUT |=  0x08
// #define xxx_OFF		cbi(PORTD_OUT, 3);		// PORTD_OUT &= ~0x08

// #define xxx_ON		sbi(PORTD_OUT, 4);		// PORTD_OUT |=  0x10
// #define xxx_OFF		cbi(PORTD_OUT, 4);		// PORTD_OUT &= ~0x10

// #define xxx_ON		sbi(PORTD_OUT, 5);		// PORTD_OUT |=  0x20
// #define xxx_OFF		cbi(PORTD_OUT, 5);		// PORTD_OUT &= ~0x20

// #define xxx_ON		sbi(PORTD_OUT, 6);		// PORTD_OUT |=  0x40
// #define xxx_OFF		cbi(PORTD_OUT, 6);		// PORTD_OUT &= ~0x40

// #define xxx_ON		sbi(PORTD_OUT, 7);		// PORTD_OUT |=  0x80
// #define xxx_OFF		cbi(PORTD_OUT, 7);		// PORTD_OUT &= ~0x80

//=================================================================================================
//		PORT E Control Bit
//=================================================================================================
// #define xxx_ON		sbi(PORTE_OUT, 0);		// PORTE_OUT |=  0x01
// #define xxx_OFF		cbi(PORTE_OUT, 0);		// PORTE_OUT &= ~0x01

// #define xxx_ON		sbi(PORTE_OUT, 1);		// PORTE_OUT |=  0x02
// #define xxx_OFF		cbi(PORTE_OUT, 1);		// PORTE_OUT &= ~0x02

// #define xxx_ON		sbi(PORTE_OUT, 2);		// PORTE_OUT |=  0x04
// #define xxx_OFF		cbi(PORTE_OUT, 2);		// PORTE_OUT &= ~0x04

// #define xxx_ON		sbi(PORTE_OUT, 3);		// PORTE_OUT |=  0x08
// #define xxx_OFF		cbi(PORTE_OUT, 3);		// PORTE_OUT &= ~0x08

// #define xxx_ON		sbi(PORTE_OUT, 4);		// PORTE_OUT |=  0x10
// #define xxx_OFF		cbi(PORTE_OUT, 4);		// PORTE_OUT &= ~0x10

// #define xxx_ON		sbi(PORTE_OUT, 5);		// PORTE_OUT |=  0x20
// #define xxx_OFF		cbi(PORTE_OUT, 5);		// PORTE_OUT &= ~0x20

// #define xxx_ON		sbi(PORTE_OUT, 6);		// PORTE_OUT |=  0x40
// #define xxx_OFF		cbi(PORTE_OUT, 6);		// PORTE_OUT &= ~0x40

// #define xxx_ON		sbi(PORTE_OUT, 7);		// PORTE_OUT |=  0x80
// #define xxx_OFF		cbi(PORTE_OUT, 7);		// PORTE_OUT &= ~0x80

//=================================================================================================
//		PORT F Control Bit
//=================================================================================================
// #define xxx_ON		sbi(PORTF_OUT, 0);		// PORTF_OUT |=  0x01
// #define xxx_OFF		cbi(PORTF_OUT, 0);		// PORTF_OUT &= ~0x01

// #define xxx_ON		sbi(PORTF_OUT, 1);		// PORTF_OUT |=  0x02
// #define xxx_OFF		cbi(PORTF_OUT, 1);		// PORTF_OUT &= ~0x02

// #define SDA_ON		sbi(PORTF_OUT, 2);		// PORTF_OUT |=  0x04
// #define SDA_OFF		cbi(PORTF_OUT, 2);		// PORTF_OUT &= ~0x04

// #define SCL_ON		sbi(PORTF_OUT, 3);		// PORTF_OUT |=  0x08
// #define SCL_OFF		cbi(PORTF_OUT, 3);		// PORTF_OUT &= ~0x08

// #define xxx_ON		sbi(PORTF_OUT, 4);		// PORTF_OUT |=  0x10
// #define xxx_OFF		cbi(PORTF_OUT, 4);		// PORTF_OUT &= ~0x10

// #define xxx_ON		sbi(PORTF_OUT, 5);		// PORTF_OUT |=  0x20
// #define xxx_OFF		cbi(PORTF_OUT, 5);		// PORTF_OUT &= ~0x20

// #define xxx_ON		sbi(PORTF_OUT, 6);		// PORTF_OUT |=  0x40
// #define xxx_OFF		cbi(PORTF_OUT, 6);		// PORTF_OUT &= ~0x40

// #define xxx_ON		sbi(PORTF_OUT, 7);		// PORTF_OUT |=  0x80
// #define xxx_OFF		cbi(PORTF_OUT, 7);		// PORTF_OUT &= ~0x80

//=================================================================================================
//		PORT G Control Bit
//=================================================================================================
#define HC595_LATCH_ON		sbi(PORTG_OUT, 0);		// PORTG_OUT |=  0x01
#define HC595_LATCH_OFF		cbi(PORTG_OUT, 0);		// PORTG_OUT &= ~0x01

#define HC595_SHIFT_ON		sbi(PORTG_OUT, 1);		// PORTG_OUT |=  0x02
#define HC595_SHIFT_OFF		cbi(PORTG_OUT, 1);		// PORTG_OUT &= ~0x02

#define HC595_DATA_ON		sbi(PORTG_OUT, 2);		// PORTG_OUT |=  0x04
#define HC595_DATA_OFF		cbi(PORTG_OUT, 2);		// PORTG_OUT &= ~0x04

// #define xxx_ON		sbi(PORTG_OUT, 3);		// PORTG_OUT |=  0x08
// #define xxx_OFF		cbi(PORTG_OUT, 3);		// PORTG_OUT &= ~0x08

#define DIGIT3_ON		sbi(PORTG_OUT, 4);		// PORTG_OUT |=  0x10
#define DIGIT3_OFF		cbi(PORTG_OUT, 4);		// PORTG_OUT &= ~0x10

#define DIGIT2_ON		sbi(PORTG_OUT, 5);		// PORTG_OUT |=  0x20
#define DIGIT2_OFF		cbi(PORTG_OUT, 5);		// PORTG_OUT &= ~0x20

#define DIGIT1_ON		sbi(PORTG_OUT, 6);		// PORTG_OUT |=  0x40
#define DIGIT1_OFF		cbi(PORTG_OUT, 6);		// PORTG_OUT &= ~0x40

#define DIGIT0_ON		sbi(PORTG_OUT, 7);		// PORTG_OUT |=  0x80
#define DIGIT0_OFF		cbi(PORTG_OUT, 7);		// PORTG_OUT &= ~0x80

//=================================================================================================
#define keytemp			~PORTA_IN			// Key IntPut Port C

#define DO1			~PORTC_IN & 0x08		// Key IntPut Port C & 00001000
// #define DO2			~PORTC_IN & 0x20		// Key IntPut Port C & 00100000
// #define DO3			~PORTC_IN & 0x80		// Key IntPut Port C & 10000000

//=================================================================================================
//		USART 사용 설정
//=================================================================================================
#define USART_SET						// USART 사용 설정
//-------------------------------------------------------------------------------------------------
#ifdef USART_SET
	uint8_t printf_port;					// USART 0 ~ 5 : printf 출력 설정
//-------------------------------------------------------------------------------------------------
//	USART 0 ~ 5 Port 설정 및 채널 0 ~ 1 설정
//	채널 0 ==> TX : Pin 0, RX : Pin 1 사용
//	채널 1 ==> TX : Pin 4, RX : Pin 5 사용
//	Usart0 채널 0 의 TX 는 외부 클럭과 연결 되어져 있어 외부클럭 사용시 사용할수 없음 Port_A0
//-------------------------------------------------------------------------------------------------
//	#define Usart_0		1				// Usart0 채널 : 1 => Port A
	#define Usart_1		0				// Usart1 채널 : 0 => Port C
//	#define Usart_2		0				// Usart2 채널 : 0 => Port F
	#define Usart_3		0				// Usart3 채널 : 0 => Port B
//	#define Usart_4		0				// Usart4 채널 : 0 => Port E
//	#define Usart_5		0				// Usart5 채널 : 0 => Port G
//-------------------------------------------------------------------------------------------------
//	#define STX_Normal	0x02
//	#define ETX_Normal	0x03

	#define STX_Iontec	0xF0
	#define ETX_Iontec	0xF1

	#define CTL2PC		0				// Controller To PC
	#define PC2CTL		1				// PC To Controller
//-------------------------------------------------------------------------------------------------
//		DEV_TYPE
//-------------------------------------------------------------------------------------------------
//	#define SORTING			1			// 선별기
//	#define GUNSA			2			// 군사
//	#define BUNMANSA		3			// 분만사
//	#define FEEDBIN			4			// 사료빈
//	#define WATER			5			// 음수
//	#define CLO2			6			// 환경
//	#define WEIGHT			7			// 사료 계량기
//	#define FAN			8			// 환기팬
//	#define WINCHI			9			// 임신사
//	#define BUNYO			10			// 습식급이기
//	#define SENSOR			11			// 센서
	#define SORTING_MOVE		21			// 이동형 선별기(포유자돈, 비육돈)
//-------------------------------------------------------------------------------------------------
	#define CD_STATUS		0			// 실시간상태 정보
//	#define CD_SET			1			// 설정정보
//	#define CD_SETRESP		2			// 설정응답
//	#define CD_STATEREQ		3			// 실시간상태 정보요청
//	#define CD_DEVIDCHG		4			// 장비ID변경
//	#define CD_WGTREQ		5			// 무게정보요청
#endif
//-------------------------------------------------------------------------------------------------
#ifdef Usart_0 							// Usart0 동작 설정
	#define RX0_SIZE	250				// Usart0 SIZE
	#define Usart0_BAUD	9600				// Usart0 통신 속도 설정
	uint8_t Rx0_Buff[RX0_SIZE];				// RXSIZE : 250
	uint8_t Rx0_Cnt;
	int USART0_Write(char data);
	void USART0_SendString(const char *str);
	void Rx0_Buff_Clear(void);
#endif
//-------------------------------------------------------------------------------------------------
#ifdef Usart_1							// Usart1 동작 설정
	#define RX1_SIZE	25				// Usart1 SIZE
	#define Usart1_BAUD	9600				// Usart1 통신 속도 설정
	uint8_t Rx1_Buff[RX1_SIZE];				// RXSIZE : 250
	uint8_t Rx1_Cnt;
	int USART1_Write(char data);
	void USART1_SendString(const char *str);
	void Rx1_Buff_Clear(void);
#endif
//-------------------------------------------------------------------------------------------------
#ifdef Usart_2							// Usart2 동작 설정
	#define RX2_SIZE	250				// Usart2 SIZE
	#define Usart2_BAUD	9600				// Usart2 통신 속도 설정
	uint8_t Rx2_Buff[RX2_SIZE];				// RXSIZE : 250
	uint8_t Rx2_Cnt;
	int USART2_Write(char data);
	void USART2_SendString(const char *str);
	void Rx2_Buff_Clear(void);
#endif
//-------------------------------------------------------------------------------------------------
#ifdef Usart_3							// Usart3 동작 설정
	#define RX3_SIZE	250				// Usart3 SIZE
	#define Usart3_BAUD	9600				// Usart3 통신 속도 설정
	uint8_t Rx3_Buff[RX3_SIZE];				// RXSIZE : 250
	uint8_t Rx3_Cnt;
	int USART3_Write(char data);
	void USART3_SendString(const char *str);
	void Rx3_Buff_Clear(void);
#endif
//-------------------------------------------------------------------------------------------------
#ifdef Usart_4							// Usart4 동작 설정
	#define RX4_SIZE	250				// Usart4 SIZE
	#define Usart4_BAUD	9600				// Usart4 통신 속도 설정
	uint8_t Rx4_Buff[RX4_SIZE];				// RXSIZE : 250
	uint8_t Rx4_Cnt;
	int USART4_Write(char data);
	void USART4_SendString(const char *str);
	void Rx4_Buff_Clear(void);
#endif
//-------------------------------------------------------------------------------------------------
#ifdef Usart_5							// Usart5 동작 설정
	#define RX5_SIZE	250				// Usart5 SIZE
	#define Usart5_BAUD	9600				// Usart5 통신 속도 설정
	uint8_t Rx5_Buff[RX5_SIZE];				// RXSIZE : 250
	uint8_t Rx5_Cnt;
	int USART5_Write(char data);
	void USART5_SendString(const char *str);
	void Rx5_Buff_Clear(void);
#endif

//=================================================================================================
//		EEPROM Data Address
//=================================================================================================
// #define Gateway_Gukbun_Addr	0				// 게이트웨이 국번 주소		// Byte   0
#define Gateway_GWID_Addr	1				// 게이트웨이 ID   주소		// Byte   1
#define OLD_Date_Addr		2				// 이전 날짜 주소			// Byte   2

#define Cell_ID_Addr		3				// 로드셀 ID 주소		// Byte   3
#define Cell_CNT_Addr		4				// 로드셀 갯수 설정 값 주소	// Byte   4

#define Cell_ROOM_Addr		5				// 방 번호 설정 값 주소		// Byte   5
#define Cell_STAL_Addr		6				// 스톨 번호 설정 값 주소		// Byte   6
#define Cell_AUTO_Addr		7				// 자동 모드 설정 값 주소		// Byte   7

#define Cell_Capa_Addr		8				// 로드셀 용량 값 주소		// Word   8,  9
#define Cell_IN_S_Addr		10				// 로드셀 입구체중 값 주소	// Word  10, 11
#define Cell_RO_Addr		12				// 로드셀 출력 값 주소		// DWord 12, 13, 14, 15

#define	Data_Count_Addr		16				// Data Count 값 주소		// Word  16, 17

#define MEASURE_AUTO		19				// 자동 측정			// Byte  19
#define MEASURE_COUNT		20				// 측정횟수			// Word  20, 21
#define ROOM_NUMBER		22				// 방번호			// Byte  22
#define STALL_NUMBER		23				// 스톨번호			// Byte  23
#define MEASURE_WEIGHT		24				// 측정무게			// Word  24, 25
#define MEASURE_YEAR		26				// 측정 년			// Byte  26
#define MEASURE_MONTH		27				// 측정 월			// Byte  27
#define MEASURE_DATE		28				// 측정 일			// Byte  28
#define MEASURE_HOUR		29				// 측정 시			// Byte  29
#define MEASURE_MIN		30				// 측정 분			// Byte  30
#define MEASURE_SEC		31				// 측정 초			// Byte  31
//-------------------------------------------------------------------------------------------------
#define MEASURE_LENGTH		13				// 측정 기록 길이

//=================================================================================================
//		Timer 사용 설정
//=================================================================================================
// #define Timer_0						// Timer 0 사용 설정
//-------------------------------------------------------------------------------------------------
// #ifdef Timer_0
// #endif
//=================================================================================================
#define Timer_1							// Timer 1 사용 설정
//-------------------------------------------------------------------------------------------------
#ifdef Timer_1
	#define FNS_TIME_1Sec	200
	#define REG_EXIT_TIME	FNS_TIME_1Sec * 10
	#define FND_EXIT_TIME	FNS_TIME_1Sec * 30

	uint8_t Blink_Timer;
	uint8_t Blink;

	uint8_t Beep_Timer;
#endif

//=================================================================================================
//		ADC 사용 설정
//=================================================================================================
// #define ADC_SET						// ADC 사용 설정=> Port D
//-------------------------------------------------------------------------------------------------
// #ifdef ADC_SET
// 	uint32_t AD_Channel;
// 	double ADC_Value;
// #endif

//=================================================================================================
//		PWM 사용 설정
//=================================================================================================
// #define PWM_SET						// PWM 사용 설정
//-------------------------------------------------------------------------------------------------
// #ifdef PWM_SET
// 	volatile uint8_t PWM_duty;
// #endif

//=================================================================================================
//		FND 표시 Font
//=================================================================================================
#define FND_0			0xFC				// 0b11111100
#define FND_1			0x60				// 0b01100000
#define FND_2			0xDA				// 0b11011010
#define FND_3			0xF2				// 0b11110010
#define FND_4			0x66				// 0b01100110
#define FND_5			0xB6				// 0b10110110
#define FND_6			0x3E				// 0b00111110
#define FND_7			0xE4				// 0b11100100
#define FND_8			0xFE				// 0b11111110
#define FND_9			0xE6				// 0b11100110
#define FND_MINUS		0x02				// 0b00000010
#define FND_Point		0x01				// 0b00000001
#define FND_OFF			0x00				// 0b00000000
// #define FND_ON		0xFF				// 0b11111111
//-------------------------------------------------------------------------------------------------
// #define FND_A		0xEE				// 0b11101110
#define FND_a			0xFA				// 0b11111010
#define FND_b			0x3E				// 0b00111110
#define FND_C			0x9C				// 0b10011100
#define FND_c			0x1A				// 0b00011010
#define FND_d			0x7A				// 0b01111010
// #define FND_E		0x9E				// 0b10011110
#define FND_e			0xDE				// 0b11011110
#define FND_F			0x8E				// 0b10001110
#define FND_G			0xBE				// 0b10111110
#define FND_g			0xF6				// 0b11110110
#define FND_h			0x2E				// 0b00101110
// #define FND_H		0x6E				// 0b01101110
#define FND_I			0x60				// 0b01100000
// #define FND_J		0x70				// 0b01110000
// #define FND_L		0x1C				// 0b00011100
#define FND_l			0x60				// 0b01100000
#define FND_N			0xEC				// 0b11101100
#define FND_n			0x2A				// 0b00101010
#define FND_O			0xFC				// 0b11111100
#define FND_o			0x3A				// 0b00111010
#define FND_P			0xCE				// 0b11001110
// #define FND_R		0xEE				// 0b11101110
#define FND_R			0x8C				// 0b10001100 : r
#define FND_r			0x0A				// 0b00001010
#define FND_S			0xB6				// 0b10110110
#define FND_t			0x1E				// 0b00011110
// #define FND_U		0x7C				// 0b01111100
#define FND_u			0x38				// 0b00111000
// #define FND_v		0x38				// 0b00111000
// #define FND_y		0x76				// 0b01110110
#define FND_Z			0xDA				// 0b11011010

//-------------------------------------------------------------------------------------------------
extern const uint8_t fnd_tbl[];
extern uint8_t DIGIT[3][4];

//=================================================================================================
#define FW_VER			47				// 펌웨어 버젼
#define Admit_Weight		5				// 인정체중 5x10g = 50g

//=================================================================================================
extern uint8_t Gukbun;							// 게이트웨이 국번 변수 선언
extern uint8_t Gwid;							// 게이트웨이 ID 변수 선언
extern uint8_t Auto_Mode;						// 자동모드 변수 선언

extern uint8_t OK_Count_1;						// OK Count 1
extern uint8_t OK_Count_2;						// OK Count 2
extern uint8_t Zero_Count;						// Zero Count 변수 선언
extern uint8_t Value_Count;						// Value Count 변수 선언

extern uint8_t Time_Count;						// 시간 표시 용 변수 설정
extern uint16_t Sec_Count;						// 초 저장 용 변수 설정
extern uint8_t SECOND;							// 이전 시간(초)용 변수 설정

extern uint16_t Data_Number;						// Data 번호
extern uint8_t Room_Number;						// 방 번호
extern uint8_t Stal_Number;						// 스톨 번호
extern uint16_t EepCnt;

//=================================================================================================
void MCU_Init(void);						// AVR128DA64 MCU Initialize
void Fnd_Display(void);						// FND Display
void FND_Value_Data(uint8_t start, uint8_t end, uint8_t value);	// FND 에 반복 되는 Data 표시
void FND_ALL_OFF(void);						// FND 전체 OFF
void Key_Scan(void);						// key 입력 체크
//-------------------------------------------------------------------------------------------------
void Read_eeprom(void);						// EEPROM Data 읽기
void Read_eeprom_Weight_Data(uint16_t cnt);			// EEPROM Weight Data 읽기
void eeprom_Data_erase(uint16_t cnt);				// EEPROM Data 지우기
void Alarm(void);						// 알람(비프 3번)
//-------------------------------------------------------------------------------------------------
void Seting_Mode(void);						// 셋팅모드
void Check_Value(void);						// 로드셀 체중측정
void Send_To_Server_0(void);					// PC로 데이터 전송 (1회 1개씩 전송 : 측정체중 Data)
void Send_To_Server_1(uint8_t Code);				// PC로 데이터 전송 (설정정보 답변)
void Receiv_PC(void);						// PC로 부터 데이터수신
void Time_Check(void);						// 시간 체크
void ADC_Check(void);						// ADC 체크 (로드셀 신호)
void Proportion_Set(void);					// 로드셀 비율 설정 및 초기화
void Var_Init(void);						// 변수 및 로드셀 초기화

//-------------------------------------------------------------------------------------------------
int std_putchar(char c,FILE *stream);				// printf Data 를 설정한 USART Port 로 전송
void wdt_reset(void);						// Reset WDT
//-------------------------------------------------------------------------------------------------
void Beep(uint16_t BeepTimer_1ms);

//=================================================================================================
//		END OF "main.h"
//=================================================================================================
