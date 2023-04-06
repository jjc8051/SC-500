//=================================================================================================
//		File  Name	:	RTC.C
//		File  내용	:	ATmega128A RTC 설정 펌웨어
//=================================================================================================
//		ATmega128A Interrupt 관련 설정 및 동작 정의
//=================================================================================================
#include <rtc.h>

//=================================================================================================
void RTC_Set(void)								// RTC 설정 모드
{
	SET.Select = 0;								// Select 버튼 초기화

	FND_Value_Data(0, 2, 12);						// FND 0 ~ 2 FND_OFF

	RTC_Time_Read();							// RTC Data 읽기

	DIGIT[0][3] = fnd_tbl[RTC_T.Year / 10];					// 년 표시
	DIGIT[0][2] = fnd_tbl[RTC_T.Year % 10];

	DIGIT[0][1] = fnd_tbl[RTC_T.Month / 10];				// 월 표시
	DIGIT[0][0] = fnd_tbl[RTC_T.Month % 10];

	DIGIT[1][3] = fnd_tbl[RTC_T.Date / 10];					// 일 표시
	DIGIT[1][2] = fnd_tbl[RTC_T.Date % 10];

	DIGIT[1][1] = fnd_tbl[RTC_T.Hour / 10];					// 시 표시
	DIGIT[1][0] = fnd_tbl[RTC_T.Hour % 10];

	DIGIT[2][3] = fnd_tbl[RTC_T.Min / 10];					// 분 표시
	DIGIT[2][2] = fnd_tbl[RTC_T.Min % 10];

	DIGIT[2][1] = fnd_tbl[RTC_T.Sec / 10];					// 초 표시
	DIGIT[2][0] = fnd_tbl[RTC_T.Sec % 10];

	while(SET.Menu)								// 메뉴모드 이면 반복
	{
		SET.Blink = 1;							// 자릿수 깜박임 설정

		switch(KEY.Count)						// 선택 Key 가 눌린횟수
		{
			case 0:							// 년 표시 및 설정
				DIGIT[2][1] = fnd_tbl[RTC_T.Sec / 10];		// 초 표시
				DIGIT[2][0] = fnd_tbl[RTC_T.Sec % 10];
				if(Blink)					// Blink = 1 이면 
				{
					DIGIT[0][3] = FND_OFF;			// 로드셀 Year 자리 OFF
					DIGIT[0][2] = FND_OFF;
				}
				else						// Blink = 0 이면
				{
					DIGIT[0][3] = fnd_tbl[RTC_T.Year / 10];	// 로드셀 Year 자리 표시
					DIGIT[0][2] = fnd_tbl[RTC_T.Year % 10];
				}

				if(KEY.Buff)					// Key 입력이 있으면
				{
					if     (KEY.Key == 1 && RTC_T.Year < 99) {RTC_T.Year ++; KEY.Key = 0;}	// 업  버튼 이면 Year 증가
					else if(KEY.Key == 3 && RTC_T.Year >  0) {RTC_T.Year --; KEY.Key = 0;}	// 다운버튼 이면 Year 감소
				}
			break;

			case 1:							// 월 표시 및 설정
				DIGIT[0][3] = fnd_tbl[RTC_T.Year / 10];		// 년 표시
				DIGIT[0][2] = fnd_tbl[RTC_T.Year % 10];
				if(Blink)					// Blink = 1 이면
				{
					DIGIT[0][1] = FND_OFF;			// 로드셀 Month 자리 OFF
					DIGIT[0][0] = FND_OFF;
				}
				else						// Blink = 0 이면
				{
					DIGIT[0][1] = fnd_tbl[RTC_T.Month / 10];// 로드셀 Month 자리 표시
					DIGIT[0][0] = fnd_tbl[RTC_T.Month % 10];
				}

				if(KEY.Buff)					// Key 입력이 있으면
				{
					if     (KEY.Key == 1 && RTC_T.Month < 12) {RTC_T.Month ++; KEY.Key = 0;}// 업  버튼 이면 Month 증가
					else if(KEY.Key == 3 && RTC_T.Month >  1) {RTC_T.Month --; KEY.Key = 0;}// 다운버튼 이면 Month 감소
				}
			break;

			case 2:							// 일 표시 및 설정
				DIGIT[0][1] = fnd_tbl[RTC_T.Month / 10];	// 월 표시
				DIGIT[0][0] = fnd_tbl[RTC_T.Month % 10];	
				if(Blink)					// Blink = 1 이면
				{
					DIGIT[1][3] = FND_OFF;			// 로드셀 Date 자리 OFF
					DIGIT[1][2] = FND_OFF;
				}
				else						// Blink = 0 이면
				{
					DIGIT[1][3] = fnd_tbl[RTC_T.Date / 10];	// 로드셀 Date 자리 표시
					DIGIT[1][2] = fnd_tbl[RTC_T.Date % 10];
				}

				if(KEY.Buff)					// Key 입력이 있으면
				{
					if     (KEY.Key == 1 && RTC_T.Date < 31) {RTC_T.Date ++; KEY.Key = 0;}	// 업  버튼 이면 Date 증가
					else if(KEY.Key == 3 && RTC_T.Date >  1) {RTC_T.Date --; KEY.Key = 0;}	// 다운버튼 이면 Date 감소
				}
			break;

			case 3:							// 시 표시 및 설정
				DIGIT[1][3] = fnd_tbl[RTC_T.Date / 10];		// 일 표시
				DIGIT[1][2] = fnd_tbl[RTC_T.Date % 10];
				if(Blink)					// Blink = 1 이면
				{
					DIGIT[1][1] = FND_OFF;			// 로드셀 Hour 자리 OFF
					DIGIT[1][0] = FND_OFF;
				}
				else						// Blink = 0 이면
				{
					DIGIT[1][1] = fnd_tbl[RTC_T.Hour / 10];	// 로드셀 Hour 자리 표시
					DIGIT[1][0] = fnd_tbl[RTC_T.Hour % 10];
				}

				if(KEY.Buff)					// Key 입력이 있으면
				{
					if     (KEY.Key == 1 && RTC_T.Hour < 23) {RTC_T.Hour ++; KEY.Key = 0;}	// 업  버튼 이면 Hour 증가
					else if(KEY.Key == 3 && RTC_T.Hour >  0) {RTC_T.Hour --; KEY.Key = 0;}	// 다운버튼 이면 Hour 감소
				}
			break;

			case 4:							// 분 표시 및 설정
				DIGIT[1][1] = fnd_tbl[RTC_T.Hour / 10];		// 시 표시
				DIGIT[1][0] = fnd_tbl[RTC_T.Hour % 10];
				if(Blink)					// Blink = 1 이면
				{
					DIGIT[2][3] = FND_OFF;			// 로드셀 Min 자리 OFF
					DIGIT[2][2] = FND_OFF;
				}
				else						// Blink = 0 이면
				{
					DIGIT[2][3] = fnd_tbl[RTC_T.Min / 10];	// 로드셀 Min 자리 표시
					DIGIT[2][2] = fnd_tbl[RTC_T.Min % 10];
				}

				if(KEY.Buff)					// Key 입력이 있으면
				{
					if     (KEY.Key == 1 && RTC_T.Min < 59) {RTC_T.Min ++; KEY.Key = 0;}	// 업  버튼 이면 Min 증가
					else if(KEY.Key == 3 && RTC_T.Min >  0) {RTC_T.Min --; KEY.Key = 0;}	// 다운버튼 이면 Min 감소
				}
			break;

			case 5:							// 로드셀 RO 10의 자리
				DIGIT[2][3] = fnd_tbl[RTC_T.Min / 10];		// 분 표시
				DIGIT[2][2] = fnd_tbl[RTC_T.Min % 10];
				if(Blink)					// Blink = 1 이면
				{
					DIGIT[2][1] = FND_OFF;			// 로드셀 Sec 자리 OFF
					DIGIT[2][0] = FND_OFF;
				}
				else						// Blink = 0 이면
				{
					DIGIT[2][1] = fnd_tbl[RTC_T.Sec / 10];	// 로드셀 Sec 자리 표시
					DIGIT[2][0] = fnd_tbl[RTC_T.Sec % 10];
				}

				if(KEY.Buff)					// Key 입력이 있으면
				{
					if     (KEY.Key == 1 && RTC_T.Sec < 59) {RTC_T.Sec++; KEY.Key = 0;}	// 업  버튼 이면 Sec 증가
					else if(KEY.Key == 3 && RTC_T.Sec >  0) {RTC_T.Sec--; KEY.Key = 0;}	// 다운버튼 이면 Sec 감소
				}
			break;

			case 6:
				KEY.Count = 0;					// 반복을 위해 Count를 최저로 감소 시킴
			break;
		}
		wdt_reset();							// 워치독 리셋
	}
	RTC_Time_Write(RTC_YEAR,   RTC_T.Year);					// RTC 에 년 저장
	RTC_Time_Write(RTC_MONTH,  RTC_T.Month);				// RTC 에 월 저장
	RTC_Time_Write(RTC_DATE,   RTC_T.Date);					// RTC 에 일 저장
	RTC_Time_Write(RTC_HOUR,   RTC_T.Hour);					// RTC 에 시 저장
	RTC_Time_Write(RTC_MINUTE, RTC_T.Min);					// RTC 에 분 저장
	RTC_Time_Write(RTC_SECOND, RTC_T.Sec);					// RTC 에 초 저장
}

//=================================================================================================
uint8_t RTC_Single_Read(uint8_t reg)						// RTC 1Byte Data 읽기
{
	uint8_t data	= 0;							// data 변수 초기화
	uint8_t retData = 0;							// retData 변수 초기화

	data = I2C_0_read1ByteRegister(RTC_ADDRESS, reg);			// 1Byte Data 읽기
	retData = ((data >> 4) * 10) + (data & 0x0F);				// 1010의 자리, 1의 자리 변환

	return retData;								// 리턴 retData
}

//=================================================================================================
void RTC_Time_Read(void)							// RTC Data 읽기
{
	RTC_T.Year  = RTC_Single_Read(RTC_YEAR);				// YEAR
	RTC_T.Month = RTC_Single_Read(RTC_MONTH);				// MONTH
	RTC_T.Date  = RTC_Single_Read(RTC_DATE);				// DATE
	RTC_T.Day   = RTC_Single_Read(RTC_DAYOFWEEK);				// DAYOFWEEK 요일
	RTC_T.Hour  = RTC_Single_Read(RTC_HOUR);				// HOUR
	RTC_T.Min   = RTC_Single_Read(RTC_MINUTE);				// MINUTE
	RTC_T.Sec   = RTC_Single_Read(RTC_SECOND);				// SECOND
}

//=================================================================================================
void RTC_Single_Write(uint8_t reg, uint8_t wData)				// RTC 1Byte Data 쓰기
{
	uint8_t bcdData = ((wData / 10) << 4) | ((wData % 10) & 0x0F);		// 1010의 자리, 1의 자리 병홥
	I2C_0_write1ByteRegister(RTC_ADDRESS, (uint8_t)reg, bcdData);		// 1Byte Data 쓰기
}

//=================================================================================================
void RTC_Time_Write(uint8_t item, uint8_t value)				// RTC Data 쓰기
{
	uint8_t data = 0;							// data 변수 초기화

	switch(item)								// item
	{
		case RTC_SECOND:						// 초
			if(value > 59) {data = 0;}				// 59초 이상이면 0초로 초기화
			else	       {data = value;}				// 아니면 value 저장
		break;

		case RTC_MINUTE:						// 분
			if(value > 59) {data = 0;}				// 59분 이상이면 0분으로 초기화
			else	       {data = value;}				// 아니면 value 저장
		break;

		case RTC_HOUR:							// 시
			if(value > 23) {data = 0;}				// 23시 이상이면 0시로 초기화
			else	       {data = value;}				// 아니면 value 저장
		break;

		case RTC_DAYOFWEEK:						// 요일 1(일요일) ~ 7(토요일) 사용
			if(value > 7 || value == 0) {data = 1;}			// 7 이상 이거나 0 이면 1(일)로 초기화
			else			    {data = value;}		// 아니면 value 저장
		break;

		case RTC_DATE:							// 일
			if(value > 31) {data = 1;}				// 31일 이상이면 1일로 초기화
			else	       {data = value;}				// 아니면 value 저장
		break;

		case RTC_MONTH:							// 월
			if(value > 12) {data = 1;}				// 12월 이상이면 1월로 초기화
			else	       {data = value;}				// 아니면 value 저장
		break;

		case RTC_YEAR:							// 년
			if(value == 0) {data = 21;}				// 00년 이면 21년 으로 초기화
			else	       {data = value;}				// 아니면 value 저장
		break;
	}
	RTC_Single_Write(item, data);						// RTC 1Byte Data 쓰기
}

//=================================================================================================
//		END OF "RTC.C"
//=================================================================================================
