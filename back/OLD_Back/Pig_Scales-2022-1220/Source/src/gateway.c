//=================================================================================================
//		File  Name	:	gateway.c
//		File  내용	:	gateway 연동 펌웨어
//		작성  일자	:	2022년 07월 25일
//=================================================================================================
//		gateway 관련 설정 및 동작 정의
//=================================================================================================
#include <gateway.h> 

//=================================================================================================
void Gukbun_Set(void)								// 게이트 웨이 국번 설정
{
	FND_Value_Data(0, 2, 12);						// FND 0 ~ 2 FND_OFF

	while(SET.Menu && SET.Select)						// 메뉴 와 선택 이 모두 1 이면 반복
	{
		DIGIT[0][3] = FND_G;						// FND 0 에 G-no (국번)표시
		DIGIT[0][2] = FND_MINUS;
		DIGIT[0][1] = FND_n;
		DIGIT[0][0] = FND_o;

		DIGIT[1][1] = fnd_tbl[Gukbun / 10];				// FND 1 에 국번 번호 표시
		DIGIT[1][0] = fnd_tbl[Gukbun % 10];

		if(KEY.Buff)							// Key 입력이 있으면
		{
			if     (KEY.Key == 1 && Gukbun < 60) {Gukbun++; KEY.Key = 0;}	// 업  버튼 이면 국번 60까지 증가
			else if(KEY.Key == 3 && Gukbun > 1)  {Gukbun--; KEY.Key = 0;}	// 다운버튼 이면 국번  0까지 감소
		}
		wdt_reset();							// 워치독 리셋
	}
	ee_write_byte(Gateway_Gukbun_Addr, Gukbun);				// 국번 번호를 EEPROM 에 저장
}

//=================================================================================================
void Gwid_Set(void)								// 게이트 웨이 ID 설정
{
	FND_Value_Data(0, 2, 12);						// FND 0 ~ 2 FND_OFF

	while(SET.Menu && SET.Select)						// 메뉴 와 선택 이 모두 1 이면 반복
	{
		DIGIT[0][3] = FND_G;						// FND 0 에 G-Id (국번)표시
		DIGIT[0][2] = FND_MINUS;
		DIGIT[0][1] = FND_I;
		DIGIT[0][0] = FND_d;

		DIGIT[1][1] = fnd_tbl[Gwid / 10];				// FND 1 에 게이트 웨이 ID 표시
		DIGIT[1][0] = fnd_tbl[Gwid % 10];

		if(KEY.Buff)							// Key 입력이 있으면
		{
			if     (KEY.Key == 1 && Gwid < 60) {Gwid++; KEY.Key = 0;}	// 업  버튼 이면 ID 60까지 증가
			else if(KEY.Key == 3 && Gwid > 1)  {Gwid--; KEY.Key = 0;}	// 다운버튼 이면 ID  0까지 감소
		}
		wdt_reset();							// 워치독 리셋
	}
	ee_write_byte(Gateway_GWID_Addr, Gwid);					// 게이트 웨이 ID 를 EEPROM 에 저장
}

//=================================================================================================
void Gateway_Set(void)								// 게이트 웨이 설정
{
	uint8_t count = 1;							// 카운트 변수 초기화
	SET.Select = 0;								// Select 버튼 초기화

	FND_Value_Data(0, 2, 12);						// FND 0 ~ 2 FND_OFF
	
	while(SET.Menu)								// 메뉴가 1 이면 반복
	{
		if(KEY.Buff)							// Key 입력이 있으면
		{
			if     (KEY.Key == 1 && count < 3) {count++; KEY.Key = 0;}	// 업  버튼 이면 Count 증가
			else if(KEY.Key == 3 && count > 0) {count--; KEY.Key = 0;}	// 다운버튼 이면 Count 감소
		}
		DIGIT[2][0] = fnd_tbl[count];					// FND 2.0 에 Count 표시

		switch(count)							// count
		{
			case 0:							// Count = 0 이면
				count = 2;					// 반복을 위해 Count를 최고로 증가 시킴
			break;

			case 1:							// Count = 1 이면
				DIGIT[0][3] = FND_G;				// FND 0 에 G-no (국번)표시
				DIGIT[0][2] = FND_MINUS;
				DIGIT[0][1] = FND_n;
				DIGIT[0][0] = FND_o;

				DIGIT[1][3] = FND_OFF;				// FND 1 에 국번 번호 표시
				DIGIT[1][2] = FND_OFF;
				DIGIT[1][1] = fnd_tbl[Gukbun / 10];
				DIGIT[1][0] = fnd_tbl[Gukbun % 10];

				if(SET.Select) {Gukbun_Set();}			// 게이트 웨이 국번 설정
			break;

			case 2:							// Count = 2 이면
				DIGIT[0][3] = FND_G;				// FND 0 에 G-Id 표시
				DIGIT[0][2] = FND_MINUS;
				DIGIT[0][1] = FND_I;
				DIGIT[0][0] = FND_d;

				DIGIT[1][3] = FND_OFF;				// FND 1 에 게이트 웨이 ID 표시
				DIGIT[1][2] = FND_OFF;
				DIGIT[1][1] = fnd_tbl[Gwid / 10];
				DIGIT[1][0] = fnd_tbl[Gwid % 10];

				if(SET.Select) {Gwid_Set();}			// 게이트 웨이 ID 설정
			break;

			case 3:							// Count = 3 이면
				count = 1;					// 반복을 위해 Count를 최저로 감소 시킴
			break;
		}
		wdt_reset();							// 워치독 리셋
	}
}

//=================================================================================================
//		END OF "gateway.c"
//=================================================================================================
