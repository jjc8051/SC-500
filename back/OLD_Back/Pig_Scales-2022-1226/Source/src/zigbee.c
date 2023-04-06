//=================================================================================================
//		File  Name	:	zigbee.c
//		File  내용	:	zigbee 구동 펌웨어
//		작성  일자	:	2022년 07월 25일
//=================================================================================================
//		zigbee 관련 설정 및 동작 정의
//=================================================================================================
#include <zigbee.h>

//=================================================================================================
int CH[15] = {0x3043, 0x3044, 0x3045, 0x3046, 0x3130, 0x3131, 0x3132, 0x3133, 0x3134, 0x3135, 0x3136, 0x3137, 0x3138, 0x3139, 0x3140};
//지그비 채널{    0C,     0D,     0E,     0F,     10,     11,     12,     13,     14,     15,     16,     17,     18,     19,     1A}

//=================================================================================================
void ZigBee_Write_id(void)							// ZigBee PanID 저장
{
	uint8_t zbid_write = 0;							// zbid_write 변수 초기화

	do{
		switch(zbid_write)						// zbid_write
		{
			case 0:							// zbch_wite = 0 이면
				USART3_SendString("+++");			// USART3 에 +++ 전송
				_delay_ms(1000);				// ZigBee 최초 답변 시간 1초 Delay
			break;

			case 1:							// zbch_wite = 1 이면
				USART3_SendString("ATID");			// USART3 에 ATCH 전송
				USART3_Write(Pan_id_1000 + 0x30);		// USART3 에 Pan ID 2Byte 상위 전송
				USART3_Write(Pan_id_100 + 0x30);		// USART3 에 Pan ID 2Byte 하위 전송
				USART3_Write(Pan_id_10 + 0x30);			// USART3 에 Pan ID 1Byte 상위 전송
				USART3_Write(Pan_id_1 + 0x30);			// USART3 에 Pan ID 1Byte 하위 전송
				USART3_Write('\r');				// USART3 에 \r
			break;

			case 2:							// zbch_wite = 2 이면
				USART3_SendString("ATWR\r");			// USART3 에 ATWR\r 전송
			break;

			case 3:							// zbch_wite = 3 이면
				USART3_SendString("ATCN\r");			// USART3 에 ATCN\r 전송
			break;
		}
//-------------------------------------------------------------------------------------------------
		for(uint8_t cnt = 0; cnt < RX3_SIZE; cnt++)			// RX3_SIZE 만큼 반복
		{
			if(zbid_write == 0 && Rx3_Buff[cnt] == 'O' && Rx3_Buff[cnt + 1] == 'K' && Rx3_Buff[cnt + 2] == 0x0d)
			{							// 0+OK+CR
				zbid_write = 1;					// 답변 확인 후 다음으로 증가
				Beep(3);
				_delay_ms(50);					// ZigBee 처리 시간 후
				Rx3_Buff_Clear();				// Rx3 Buff Clear
				break;
			}

			if(zbid_write == 1 && Rx3_Buff[cnt] == 'O' && Rx3_Buff[cnt + 1] == 'K' && Rx3_Buff[cnt + 2] == 0x0d)
			{							// 1+OK+CR
				zbid_write = 2;					// 답변 확인 후 다음으로 증가
				Beep(3);
				_delay_ms(50);					// ZigBee 처리 시간 후
				Rx3_Buff_Clear();				// Rx3 Buff Clear
				break;
			}

			if(zbid_write == 2 && Rx3_Buff[cnt] == 'O' && Rx3_Buff[cnt + 1] == 'K' && Rx3_Buff[cnt + 2] == 0x0d)
			{							// 2+OK+CR
				zbid_write = 3;					// 답변 확인 후 다음으로 증가
				Beep(3);
				_delay_ms(50);					// ZigBee 처리 시간 후
				Rx3_Buff_Clear();				// Rx3 Buff Clear
				break;
			}

			if(zbid_write == 3 && Rx3_Buff[cnt] == 'O' && Rx3_Buff[cnt + 1] == 'K' && Rx3_Buff[cnt + 2] == 0x0d)
			{							// 3+OK+CR
				zbid_write = 5;					// 답변 확인 후 While()문 종료
				Beep(3);
				_delay_ms(50);					// ZigBee 처리 시간 후
				Rx3_Buff_Clear();				// Rx3 Buff Clear
				break;
			}
		}
		wdt_reset();							// 워치독 리셋
	}while(zbid_write != 5);						// zbid_write 가 5 일때 까지 반복
}

//=================================================================================================
void ZigBee_PanID_Write(void)							// ZigBee PanID 설정
{
	uint8_t digit_5, digit_4, digit_3, digit_2, digit_1;			// digit 변수 초기화

	FND_Value_Data(1, 2, 12);						// FND 0 ~ 2 FND_OFF

	digit_5 = ZB_id / 10000;						// ZigBee PanID 10000의 자리
	digit_4 = ZB_id % 10000 / 1000;						// ZigBee PanID  1000의 자리
	digit_3 = ZB_id % 1000 / 100;						// ZigBee PanID   100의 자리
	digit_2 = ZB_id % 100 / 10;						// ZigBee PanID    10의 자리
	digit_1 = ZB_id % 10;							// ZigBee PanID     1의 자리

	DIGIT[1][3] = fnd_tbl[digit_5];						// ZigBee PanID 10000의 자리 표시
	DIGIT[1][2] = fnd_tbl[digit_4];						// ZigBee PanID  1000의 자리 표시
	DIGIT[1][1] = fnd_tbl[digit_3];						// ZigBee PanID   100의 자리 표시
	DIGIT[1][0] = fnd_tbl[digit_2];						// ZigBee PanID    10의 자리 표시

	DIGIT[2][3] = fnd_tbl[digit_1];						// ZigBee PanID     1의 자리 표시

	while(SET.Menu)								// 메뉴모드 이면 반복
	{
		SET.Blink = 1;							// 자릿수 깜박임 설정

		switch(KEY.Count)						// 선택 Key 가 눌린횟수
		{
//-------------------------------------------------------------------------------------------------
			case 0:							// ZigBee PanID 10000의 자리
				DIGIT[2][3] = fnd_tbl[digit_1];			// ZigBee PanID     1의 자리 표시
				if(Blink) {DIGIT[1][3] = FND_OFF;}		// Blink = 1 이면 ZigBee PanID 10000의 자리 OFF
				else	  {DIGIT[1][3] = fnd_tbl[digit_5];}	// Blink = 0 이면 ZigBee PanID 10000의 자리 표시

				if(KEY.Buff)					// Key 입력이 있으면
				{
					if     (KEY.Key == 1 && digit_5 < 6) {digit_5++; KEY.Key = 0;}	// 업  버튼 이면 ZigBee PanID 10000의 자리 증가
					else if(KEY.Key == 3 && digit_5 > 0) {digit_5--; KEY.Key = 0;}	// 다운버튼 이면 ZigBee PanID 10000의 자리 감소
				}
			break;
//-------------------------------------------------------------------------------------------------
			case 1:							// ZigBee PanID 1000의 자리
				DIGIT[1][3] = fnd_tbl[digit_5];			// ZigBee PanID 10000의 자리 표시
				if(Blink) {DIGIT[1][2] = FND_OFF;}		// Blink = 1 이면 ZigBee PanID 1000의 자리 OFF
				else	  {DIGIT[1][2] = fnd_tbl[digit_4];}	// Blink = 0 이면 ZigBee PanID 1000의 자리 표시

				if(KEY.Buff)					// Key 입력이 있으면
				{
					if(digit_5 >= 6)							// ZigBee PanID 가 60000 과 같거나 크면
					{
						if     (KEY.Key == 1 && digit_4 < 5) {digit_4++; KEY.Key = 0;}	// 업  버튼 이면 ZigBee PanID 1000의 자리 5까지 증가
						else if(KEY.Key == 3 && digit_4 > 0) {digit_4--; KEY.Key = 0;}	// 다운버튼 이면 ZigBee PanID 1000의 자리 감소
					}
					else									// ZigBee PanID 가 60000 과 같거나 크지 않으면
					{
						if     (KEY.Key == 1 && digit_4 < 9) {digit_4++; KEY.Key = 0;}	// 업  버튼 이면 ZigBee PanID 1000의 자리 9까지 증가
						else if(KEY.Key == 3 && digit_4 > 0) {digit_4--; KEY.Key = 0;}	// 다운버튼 이면 ZigBee PanID 1000의 자리 0까지 감소
					}
				}
			break;
//-------------------------------------------------------------------------------------------------
			case 2:							// ZigBee PanID 100의 자리
				DIGIT[1][2] = fnd_tbl[digit_4];			// ZigBee PanID 1000의 자리 표시
				if(Blink) {DIGIT[1][1] = FND_OFF;}		// Blink = 1 이면 ZigBee PanID 100의 자리 OFF
				else	  {DIGIT[1][1] = fnd_tbl[digit_3];}	// Blink = 0 이면 ZigBee PanID 100의 자리 표시

				if(KEY.Buff)					// Key 입력이 있으면
				{
					if(digit_5 >= 6 && digit_4 >= 5)					// ZigBee PanID 가 65000 과 같거나 크면
					{
						if     (KEY.Key == 1 && digit_3 < 5) {digit_3++; KEY.Key = 0;}	// 업  버튼 이면 ZigBee PanID 100의 자리 5까지 증가
						else if(KEY.Key == 3 && digit_3 > 0) {digit_3--; KEY.Key = 0;}	// 다운버튼 이면 ZigBee PanID 100의 자리 감소
					}
					else									// ZigBee PanID 가 65000 과 같거나 크지 않으면
					{
						if     (KEY.Key == 1 && digit_3 < 9) {digit_3++; KEY.Key = 0;}	// 업  버튼 이면 ZigBee PanID 100의 자리 9까지 증가
						else if(KEY.Key == 3 && digit_3 > 0) {digit_3--; KEY.Key = 0;}	// 다운버튼 이면 ZigBee PanID 100의 자리 0까지 감소
					}
				}
			break;
//-------------------------------------------------------------------------------------------------
			case 3:							// ZigBee PanID 10의 자리
				DIGIT[1][1] = fnd_tbl[digit_3];			// ZigBee PanID 100의 자리 표시
				if(Blink) {DIGIT[1][0] = FND_OFF;}		// Blink = 1 이면 ZigBee PanID 10의 자리 OFF
				else	  {DIGIT[1][0] = fnd_tbl[digit_2];}	// Blink = 0 이면 ZigBee PanID 10의 자리 표시

				if(KEY.Buff)					// Key 입력이 있으면
				{
					if(digit_5 >= 6 && digit_4 >= 5 && digit_3 >= 5)			// ZigBee PanID 가 65500 과 같거나 크면
					{
						if     (KEY.Key == 1 && digit_2 < 3) {digit_2++; KEY.Key = 0;}	// 업  버튼 이면 ZigBee PanID 10의 자리 3까지 증가
						else if(KEY.Key == 3 && digit_2 > 0) {digit_2--; KEY.Key = 0;}	// 다운버튼 이면 ZigBee PanID 10의 자리 감소
					}
					else									// ZigBee PanID 가 65500 과 같거나 크지 않으면
					{
						if     (KEY.Key == 1 && digit_2 < 9) {digit_2++; KEY.Key = 0;}	// 업  버튼 이면 ZigBee PanID 10의 자리 9까지 증가
						else if(KEY.Key == 3 && digit_2 > 0) {digit_2--; KEY.Key = 0;}	// 다운버튼 이면 ZigBee PanID 10의 자리 0까지 감소
					}
				}
			break;
//-------------------------------------------------------------------------------------------------
			case 4:							// ZigBee PanID 1의 자리
				DIGIT[1][0] = fnd_tbl[digit_2];			// ZigBee PanID 10의 자리 표시
				if(Blink) {DIGIT[2][3] = FND_OFF;}		// Blink = 1 이면 ZigBee PanID 1의 자리 OFF
				else	  {DIGIT[2][3] = fnd_tbl[digit_1];}	// Blink = 0 이면 ZigBee PanID 1의 자리 표시

				if(KEY.Buff)					// Key 입력이 있으면
				{
					if(digit_5 >= 6 && digit_4 >= 5 && digit_3 >= 5 && digit_2 >= 3)	// ZigBee PanID 가 65530 과 같거나 크면
					{
						if     (KEY.Key == 1 && digit_1 < 5) {digit_1++; KEY.Key = 0;}	// 업  버튼 이면 ZigBee PanID 1의 자리 6까지 증가
						else if(KEY.Key == 3 && digit_1 > 0) {digit_1--; KEY.Key = 0;}	// 다운버튼 이면 ZigBee PanID 1의 자리 감소
					}
					else									// ZigBee PanID 가 65530 과 같거나 크지 않으면
					{
						if     (KEY.Key == 1 && digit_1 < 9) {digit_1++; KEY.Key = 0;}	// 업  버튼 이면 ZigBee PanID 1의 자리 9까지 증가
						else if(KEY.Key == 3 && digit_1 > 0) {digit_1--; KEY.Key = 0;}	// 다운버튼 이면 ZigBee PanID 1의 자리 0까지 감소
					}
				}
			break;
//-------------------------------------------------------------------------------------------------
			case 5:
				KEY.Count = 0;					// 반복을 위해 Count를 최저로 감소 시킴
			break;
		}
		wdt_reset();							// 워치독 리셋
	}
//-------------------------------------------------------------------------------------------------
	ZB_id = 0;								// ZigBee Pan_id 초기화
	ZB_id += (digit_5 * 10000);						// ZigBee Pan_id 10000의 자리
	ZB_id += (digit_4 * 1000);						// ZigBee Pan_id  1000의 자리
	ZB_id += (digit_3 * 100);						// ZigBee Pan_id   100의 자리
	ZB_id += (digit_2 * 10);						// ZigBee Pan_id    10의 자리
	ZB_id += (digit_1 * 1);							// ZigBee Pan_id     1의 자리

	Pan_id_1000 = (ZB_id / 4096);						// ZigBee Pan_id HEX Code 2Byte 상위
	Pan_id_100  = (ZB_id % 4096 / 256);					// ZigBee Pan_id HEX Code 2Byte 하위
	Pan_id_10   = (ZB_id % 4096 % 256 / 16);				// ZigBee Pan_id HEX Code 1Byte 상위
	Pan_id_1    = (ZB_id % 4096 % 256 % 16);				// ZigBee Pan_id HEX Code 1Byte 하위

	ZigBee_Write_id();							// ZigBee PanID 저장

	SET.Menu = 1;								// Menu 모드 유지
	SET.Select = 0;								// Select 버튼 초기화
	SET.Blink = 0;								// 자릿수 깜박임 초기화
	KEY.Count = 0;								// 선택 Key 가 눌린횟수 초기화
}

//=================================================================================================
void ZigBee_Write_ch(void)							// ZigBee 채널 저장
{
	uint8_t zbch_wite = 0;							// zbch_wite 변수 초기화

	do{
		switch(zbch_wite)						// zbch_wite
		{
			case 0:							// zbch_wite = 0 이면
				USART3_SendString("+++");			// USART3 에 +++ 전송
				_delay_ms(1000);				// ZigBee 최초 답변 시간 1초 Delay
			break;

			case 1:							// zbch_wite = 1 이면
				USART3_SendString("ATCH");			// USART3 에 ATCH 전송
				USART3_Write(CH[ZB_ch] / 256);			// USART3 에 채널 1Byte 상위 전송
				USART3_Write(CH[ZB_ch] % 256);			// USART3 에 채널 1Byte 하위 전송
				USART3_Write('\r');				// USART3 에 \r
			break;

			case 2:							// zbch_wite = 2 이면
				USART3_SendString("ATWR\r");			// USART3 에 ATWR\r 전송
			break;

			case 3:							// zbch_wite = 3 이면
				USART3_SendString("ATCN\r");			// USART3 에 ATCN\r 전송
			break;
		}
//-------------------------------------------------------------------------------------------------
		for(uint8_t cnt = 0; cnt < RX3_SIZE; cnt++)			// RX3_SIZE 만큼 반복
		{
			if(zbch_wite == 0 && Rx3_Buff[cnt] == 'O' && Rx3_Buff[cnt + 1] == 'K' && Rx3_Buff[cnt + 2] == 0x0d)
			{							// 0+OK+CR
				zbch_wite = 1;					// 답변 확인 후 다음으로 증가
				Beep(3);
				_delay_ms(50);					// ZigBee 처리 시간 후
				Rx3_Buff_Clear();				// Rx3 Buff Clear
				break;
			}

			if(zbch_wite == 1 && Rx3_Buff[cnt] == 'O' && Rx3_Buff[cnt + 1] == 'K' && Rx3_Buff[cnt + 2] == 0x0d)
			{							// 1+OK+CR
				zbch_wite = 2;					// 답변 확인 후 다음으로 증가
				Beep(3);
				_delay_ms(50);					// ZigBee 처리 시간 후
				Rx3_Buff_Clear();				// Rx3 Buff Clear
				break;
			}

			if(zbch_wite == 2 && Rx3_Buff[cnt] == 'O' && Rx3_Buff[cnt + 1] == 'K' && Rx3_Buff[cnt + 2] == 0x0d)
			{							// 2+OK+CR
				zbch_wite = 3;					// 답변 확인 후 다음으로 증가
				Beep(3);
				_delay_ms(50);					// ZigBee 처리 시간 후
				Rx3_Buff_Clear();				// Rx3 Buff Clear
				break;
			}

			if(zbch_wite == 3 && Rx3_Buff[cnt] == 'O' && Rx3_Buff[cnt + 1] == 'K' && Rx3_Buff[cnt + 2] == 0x0d)
			{							// 3+OK+CR
				zbch_wite = 5;					// 답변 확인 후 While()문 종료
				Beep(3);
				_delay_ms(50);					// ZigBee 처리 시간 후
				Rx3_Buff_Clear();				// Rx3 Buff Clear
				break;
			}
		}
		wdt_reset();							// 워치독 리셋
	}while(zbch_wite != 5);							// zbch_wite 가 5 일때 까지 반복
}

//=================================================================================================
void ZigBee_Channel_Write(void)							// ZigBee 채널 설정
{
	FND_Value_Data(2, 2, 12);						// FND 0 ~ 2 FND_OFF

	while(SET.Select && SET.Menu)						// 메뉴 와 선택 이 모두 1 이면 반복
	{
		DIGIT[1][1] = fnd_tbl[ZB_ch / 10];				// FND 2 에 지그비 체널 표시
		DIGIT[1][0] = fnd_tbl[ZB_ch % 10];

		if(KEY.Buff)							// Key 입력이 있으면
		{
			if     (KEY.Key == 1 && ZB_ch < 14) {ZB_ch++; KEY.Key = 0;}	// 업  버튼 이면 ZigBee 채널 14까지 증가
			else if(KEY.Key == 3 && ZB_ch > 0)  {ZB_ch--; KEY.Key = 0;}	// 다운버튼 이면 ZigBee 채널  0까지 감소
		}
		wdt_reset();							// 워치독 리셋
	}
	ZigBee_Write_ch();							// ZigBee 에 채널 저장
}

//=================================================================================================
void ZigBee_Read(void)								// ZigBee 설정값 읽기								
{
	uint8_t zbch_read = 0;							// zbch_read 변수 초기화

	do{
		switch(zbch_read)						// zbch_read
		{
			case 0:							// zbch_read = 0 이면
				USART3_SendString("+++");			// USART3 에 +++ 전송
				_delay_ms(1000);				// ZigBee 최초 답변 시간 1초 Delay
			break;

			case 1:							// zbch_read = 1 이면
				USART3_SendString("ATCH\r");			// USART3 에 ATCH 전송
			break;

			case 2:							// zbch_read = 2 이면
				USART3_SendString("ATID\r");			// USART3 에 ATWR 전송
			break;

			case 3:							// zbch_read = 3 이면
				USART3_SendString("ATCN\r");			// USART3 에 ATCN 전송
			break;
		}
//-------------------------------------------------------------------------------------------------
		for(uint8_t cnt = 0; cnt < RX3_SIZE; cnt++)			// RX3_SIZE 만큼 반복
		{
			if(zbch_read == 0 && Rx3_Buff[cnt] == 'O' && Rx3_Buff[cnt + 1] == 'K' && Rx3_Buff[cnt + 2] == 0x0d)
			{							// 0+OK+CR
				zbch_read = 1;					// 답변 확인 후 다음으로 증가
				Beep(3);
				_delay_ms(50);					// ZigBee 처리 시간 후
				Rx3_Buff_Clear();				// Rx3 Buff Clear
				break;
			}
//-------------------------------------------------------------------------------------------------
			if(zbch_read == 1 && Rx3_Buff[cnt] == 0x0d)
			{							// 1+CR
				switch(cnt)					// cnt
				{
					case 1:					// ZigBee channel 읽어오기. 한자리
						ZB_ch = Rx3_Buff[cnt-1] - 0x43;	// ZigBee channel 1의 자리
					break;

					case 2:					// ZigBee channel 읽어오기. 두자리
						if(Rx3_Buff[cnt-1] <= 0x39) {ZB_ch = (Rx3_Buff[cnt-2] - 0x30) * 10 + (Rx3_Buff[cnt -1] - 0x30) - 6;}	// HEX To DEC
						else			    {ZB_ch = (Rx3_Buff[cnt-2] - 0x30) * 10 + (Rx3_Buff[cnt -1] - 0x37) - 6;}	// HEX To DEC
					break;
				}
				zbch_read = 2;					// 답변 확인 후 다음으로 증가
				Beep(3);
				_delay_ms(50);					// ZigBee 처리 시간 후
				Rx3_Buff_Clear();				// Rx3 Buff Clear
				break;
			}
//-------------------------------------------------------------------------------------------------
			if(zbch_read == 2 && Rx3_Buff[cnt] == 0x0d)
			{							// 2+CR
				switch(cnt)					// cnt
				{
					case 1:						// ZigBee Pan ID 읽어오기. 1 자리
						Pan_id_1   = Rx3_Buff[cnt-1] - 0x30;	// ZigBee Pan_id 1의 자리
					break;

					case 2:						// ZigBee Pan ID 읽어오기. 2 자리
						Pan_id_10  = Rx3_Buff[cnt-2] - 0x30;	// ZigBee Pan_id 10의 자리
						Pan_id_1   = Rx3_Buff[cnt-1] - 0x30;	// ZigBee Pan_id  1의 자리
					break;

					case 3:						// ZigBee Pan ID 읽어오기. 3 자리
						Pan_id_100 = Rx3_Buff[cnt-3] - 0x30;	// ZigBee Pan_id 100의 자리
						Pan_id_10  = Rx3_Buff[cnt-2] - 0x30;	// ZigBee Pan_id  10의 자리
						Pan_id_1   = Rx3_Buff[cnt-1] - 0x30;	// ZigBee Pan_id   1의 자리
					break;

					case 4:						// ZigBee Pan ID 읽어오기. 4 자리
						Pan_id_1000 = Rx3_Buff[cnt-4] - 0x30;	// ZigBee Pan_id 1000의 자리
						Pan_id_100  = Rx3_Buff[cnt-3] - 0x30;	// ZigBee Pan_id  100의 자리
						Pan_id_10   = Rx3_Buff[cnt-2] - 0x30;	// ZigBee Pan_id   10의 자리
						Pan_id_1    = Rx3_Buff[cnt-1] - 0x30;	// ZigBee Pan_id    1의 자리
					break;
				}
				zbch_read = 3;					// 답변 확인 후 다음으로 증가
				Beep(3);
				_delay_ms(50);					// ZigBee 처리 시간 후
				Rx3_Buff_Clear();				// Rx3 Buff Clear
				break;
			}
//-------------------------------------------------------------------------------------------------
			if(zbch_read == 3 && Rx3_Buff[cnt] == 'O' && Rx3_Buff[cnt + 1] == 'K' && Rx3_Buff[cnt + 2] == 0x0d)
			{							// 3+OK+CR
				zbch_read = 5;					// 답변 확인 후 While()문 종료
				Beep(3);
				_delay_ms(50);					// ZigBee 처리 시간 후
				Rx3_Buff_Clear();				// Rx3 Buff Clear
				break;
			}
		}
		wdt_reset();							// 워치독 리셋
	}while(zbch_read != 5);							// zbch_read 가 5 일때 까지 반복
}

//=================================================================================================
void ZigBee_Set(void)								// ZigBee 채널 및 PanID 설정 모드
{
	uint8_t Count = 1;							// 카운트 변수 초기화
	SET.Select = 0;								// Select 버튼 초기화
	FND_Value_Data(0, 2, 12);						// FND 0 ~ 2 FND_OFF
	ZigBee_Read();								// ZigBee 설정값 읽기
//-------------------------------------------------------------------------------------------------
	while(SET.Menu)								// 메뉴가 1 이면 반복
	{
		if(KEY.Buff)							// Key 입력이 있으면
		{
			if     (KEY.Key == 1 && Count < 3) {Count++; KEY.Key = 0;}	// 업  버튼 이면 Count 증가
			else if(KEY.Key == 3 && Count > 0) {Count--; KEY.Key = 0;}	// 다운버튼 이면 Count 감소
		}
		DIGIT[2][0] = fnd_tbl[Count];					// FND 2.0 에 Count 표시

		switch(Count)							// Count
		{
			case 0:							// Count = 0 이면
				Count = 2;					// 반복을 위해 Count를 최고로 증가 시킴
			break;

			case 1:							// Count = 1 이면
				DIGIT[0][3] = FND_Z;				// FND 0 에 Z-ch 표시
				DIGIT[0][2] = FND_MINUS;
				DIGIT[0][1] = FND_c;
				DIGIT[0][0] = FND_h;

				DIGIT[1][3] = FND_OFF;				// FND 2 에 ZigBee 채널 표시
				DIGIT[1][2] = FND_OFF;
				DIGIT[1][1] = fnd_tbl[ZB_ch / 10];
				DIGIT[1][0] = fnd_tbl[ZB_ch % 10];

				DIGIT[2][3] = FND_OFF;				// FND 2 1~3 OFF
				DIGIT[2][2] = FND_OFF;
				DIGIT[2][1] = FND_OFF;

				if(SET.Select) {ZigBee_Channel_Write();}	// ZigBee 채널 설정
			break;

			case 2:							// Count = 2 이면
				DIGIT[0][3] = FND_Z;				// FND 0 에 Z-Id 표시
				DIGIT[0][2] = FND_MINUS;
				DIGIT[0][1] = FND_I;
				DIGIT[0][0] = FND_d;

				ZB_id = (Pan_id_1000 * 4096) + (Pan_id_100 * 256) + (Pan_id_10 * 16) + Pan_id_1;	// ZigBee PanID 계산
				DIGIT[1][3] = fnd_tbl[ZB_id / 10000];		// FND 1 에 ZigBee PanID 1000~10의 자리 표시
				DIGIT[1][2] = fnd_tbl[ZB_id % 10000 / 1000];
				DIGIT[1][1] = fnd_tbl[ZB_id % 1000 / 100];
				DIGIT[1][0] = fnd_tbl[ZB_id % 100 / 10];

				DIGIT[2][3] = fnd_tbl[ZB_id % 10];		// FND 1 에 ZigBee PanID 1의 자리 표시
				DIGIT[2][2] = FND_OFF;
				DIGIT[2][1] = FND_OFF;

				if(SET.Select) {ZigBee_PanID_Write();}		// ZigBee PanID 설정
			break;

			case 3:							// Count = 3 이면
				Count = 1;					// 반복을 위해 Count를 최저로 감소 시킴
			break;
		}
		wdt_reset();							// 워치독 리셋
	}
}

//=================================================================================================
//		END OF "zigbee.c"
//=================================================================================================
