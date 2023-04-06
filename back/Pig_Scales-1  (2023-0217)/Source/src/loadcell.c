//=================================================================================================
//		File  Name	:	loadcell.c
//		File  내용	:	load cell 구동 펌웨어
//=================================================================================================
//		load cell 관련 설정 및 동작 정의
//=================================================================================================
#include <loadcell.h>

//=================================================================================================
void Cell_Desplay(uint8_t Number)						// 로드셀 무게 표시
{
	uint32_t Value = 0;							// Data 저장용 변수 초기화

	if     (Number == 1) {Value = CELL.Value_1;}				// 1번 로드셀 측정무게 값
	else if(Number == 2) {Value = CELL.Value_2;}				// 2번 로드셀 측정무게 값
	else if(Number == 3) {Value = CELL.Value_3;}				// 3번 로드셀 측정무게 값

	Weight_10000 = Value / 10000;						// 측정무게 100.00의 자리 100.00kg
	Weight_1000  = Value % 10000 / 1000;					// 측정무게  10.00의 자리  10.00kg
	Weight_100   = Value % 1000 / 100;					// 측정무게   1.00의 자리   1.00kg
	Weight_10    = Value % 100 / 10;					// 측정무게   0.10의 자리   0.10Kg
	Weight_1     = Value % 10 / 1;						// 측정무게   0.01의 자리   0.01Kg

	if(Value > 10000)							// 측정무게 100Kg 보다 크면
	{
		DIGIT[Number-1][3] = fnd_tbl[Weight_10000];			// 측정무게 100.00의 자리
		DIGIT[Number-1][2] = fnd_tbl[Weight_1000];			// 측정무게  10.00의 자리
		DIGIT[Number-1][1] = fnd_tbl[Weight_100]+FND_Point;		// 측정무게   1.00의 자리
		DIGIT[Number-1][0] = fnd_tbl[Weight_10];			// 측정무게   0.10의 자리
	}
	else if(Value > 1000)							// 측정무게 10Kg 보다 크면
	{
		DIGIT[Number-1][3] = fnd_tbl[Weight_1000];			// 측정무게 10.00의 자리
		DIGIT[Number-1][2] = fnd_tbl[Weight_100]+FND_Point;		// 측정무게  1.00의 자리
		DIGIT[Number-1][1] = fnd_tbl[Weight_10];			// 측정무게  0.10의 자리
		DIGIT[Number-1][0] = fnd_tbl[Weight_1];				// 측정무게  0.01의 자리
	}
	else if(Value < 1000)							// 측정무게 10Kg 보다 작으면
	{
		DIGIT[Number-1][3] = FND_OFF;					// 측정무게 10.00의 자리 OFF
		DIGIT[Number-1][2] = fnd_tbl[Weight_100]+FND_Point;		// 측정무게  1.00의 자리
		DIGIT[Number-1][1] = fnd_tbl[Weight_10];			// 측정무게  0.10의 자리
		DIGIT[Number-1][0] = fnd_tbl[Weight_1];				// 측정무게  0.01의 자리
	}
}

//=================================================================================================
void INT_Desplay(uint8_t Number)						// 보정값 무게 표시
{
	uint32_t Value = 0;							// Data 저장용 변수 초기화

	if     (Number == 1) {Value = CELL.Value_1;}				// 1번 로드셀 측정무게 값
	else if(Number == 2) {Value = CELL.Value_2;}				// 2번 로드셀 측정무게 값
	else if(Number == 3) {Value = CELL.Value_3;}				// 3번 로드셀 측정무게 값

	Weight_10000 = Value / 10000;						// 측정무게 100.00의 자리 100.00kg
	Weight_1000  = Value % 10000 / 1000;					// 측정무게  10.00의 자리  10.00kg
	Weight_100   = Value % 1000 / 100;					// 측정무게   1.00의 자리   1.00kg
	Weight_10    = Value % 100 / 10;					// 측정무게   0.10의 자리   0.10Kg
	Weight_1     = Value % 10 / 1;						// 측정무게   0.01의 자리   0.01Kg

	if(Value > 10000)							// 측정무게 100Kg 보다 크면
	{
		DIGIT[0][3] = fnd_tbl[Weight_10000];				// 측정무게 100.00의 자리
		DIGIT[0][2] = fnd_tbl[Weight_1000];				// 측정무게  10.00의 자리
		DIGIT[0][1] = fnd_tbl[Weight_100]+FND_Point;			// 측정무게   1.00의 자리
		DIGIT[0][0] = fnd_tbl[Weight_10];				// 측정무게   0.10의 자리
	}
	else if(Value > 1000)							// 측정무게 10Kg 보다 크면
	{
		DIGIT[0][3] = fnd_tbl[Weight_1000];				// 측정무게 10.00의 자리
		DIGIT[0][2] = fnd_tbl[Weight_100]+FND_Point;			// 측정무게  1.00의 자리
		DIGIT[0][1] = fnd_tbl[Weight_10];				// 측정무게  0.10의 자리
		DIGIT[0][0] = fnd_tbl[Weight_1];				// 측정무게  0.01의 자리
	}
	else if(Value < 1000)							// 측정무게 10Kg 보다 작으면
	{
		DIGIT[0][3] = FND_OFF;						// 측정무게 10.00의 자리 OFF
		DIGIT[0][2] = fnd_tbl[Weight_100]+FND_Point;			// 측정무게  1.00의 자리
		DIGIT[0][1] = fnd_tbl[Weight_10];				// 측정무게  0.10의 자리
		DIGIT[0][0] = fnd_tbl[Weight_1];				// 측정무게  0.01의 자리
	}
}

//=================================================================================================
int32_t CellRead_1(void)							// 1 번 셀 Data 읽기, HX711 제어
{
	int32_t Value = 0;							// Data 저장용 변수 초기화

	SCK_LOW_1;								// HX711 SCk LOW
	while(DO1);								// HX711 SDA가 HIGH 이면 대기
	for(uint8_t i = 0; i < 24; i++)						// 24bit Data 만큼 반복
	{
		Value = Value << 1;						// Data 1자리 쉬프트
		SCK_HIGH_1;							// HX711 SCk HIGH
		_delay_us(20);
		SCK_LOW_1;							// HX711 SCk LOW
		_delay_us(20);
		if(DO1) {Value++;}						// HX711 SDA가 HIGH 이면 Bit Data 더하기
	}
	SCK_HIGH_1;								// HX711 SCk HIGH
	_delay_us(20);
	SCK_LOW_1;								// HX711 SCk LOW
	_delay_ms(100);
	Value = Value ^ 0x400000;						// 전체 Data 의 2의 보수 값 구하기
	if(Value > Cell_Capa_1 * 10000 + CELL.OFF_Set) {Value = 0;}		// 전체 Data 가 상한값 이상이면 Data 초기화 (if(Value > 0x7FF000) {Value = 0;})
	Proportion_Set();							// 배율 계산
	Value = Value / CELL.Proportion_1;					// Data / 비율 = 무게
	return(Value);								// 리턴 무게 Data
}

//=================================================================================================
int32_t CellRead_2(void)							// 2 번 셀 Data 읽기, HX711 제어
{
	int32_t Value = 0;							// Data 저장용 변수 초기화

	SCK_LOW_2;								// HX711 SCk LOW
	while(DO2);								// HX711 SDA가 HIGH 이면 대기
	for(uint8_t i = 0; i < 24; i++)						// 24bit Data 만큼 반복
	{
		Value = Value << 1;						// Data 1자리 쉬프트
		SCK_HIGH_2;							// HX711 SCk HIGH
		_delay_us(20);
		SCK_LOW_2;							// HX711 SCk LOW
		_delay_us(20);
		if(DO2) {Value++;}						// HX711 SDA가 HIGH 이면 Bit Data 더하기
	}
	SCK_HIGH_2;								// HX711 SCk HIGH
	_delay_us(20);
	SCK_LOW_2;								// HX711 SCk LOW
	_delay_ms(100);
	Value = Value ^ 0x400000;						// 전체 Data 의 2의 보수 값 구하기
	if(Value > Cell_Capa_2 * 10000 + CELL.OFF_Set) {Value = 0;}		// 전체 Data 가 상한값 이상이면 Data 초기화 (if(Value > 0x7FF000) {Value = 0;})
	Proportion_Set();							// 배율 계산
	Value = Value / CELL.Proportion_2;					// Data / 비율 = 무게
	return(Value);								// 리턴 무게 Data
}

//=================================================================================================
int32_t CellRead_3(void)							// 3 번 셀 Data 읽기 HX711 제어
{
	int32_t Value = 0;							// Data 저장용 변수 초기화

	SCK_LOW_3;								// HX711 SCk LOW
	while(DO3);								// HX711 SDA가 HIGH 이면 대기
	for(uint8_t i = 0; i < 24; i++)						// 24bit Data 만큼 반복
	{
		Value = Value << 1;						// Data 1자리 쉬프트
		SCK_HIGH_3;							// HX711 SCk HIGH
		_delay_us(20);
		SCK_LOW_3;							// HX711 SCk LOW
		_delay_us(20);
		if(DO3) {Value++;}						// HX711 SDA가 HIGH 이면 Bit Data 더하기
	}
	SCK_HIGH_3;								// HX711 SCk HIGH
	_delay_us(20);
	SCK_LOW_3;								// HX711 SCk LOW
	_delay_ms(100);
	Value = Value ^ 0x400000;						// 전체 Data 의 2의 보수 값 구하기
	if(Value > Cell_Capa_1 * 10000 + CELL.OFF_Set) {Value = 0;}		// 전체 Data 가 상한값 이상이면 Data 초기화 (if(Value > 0x7FF000) {Value = 0;})
	Proportion_Set();							// 배율 계산
	Value = Value / CELL.Proportion_3;					// Data / 비율 = 무게
	return(Value);								// 리턴 무게 Data
}

//=================================================================================================
uint16_t Read_Load_Cell(uint8_t Number)						// 로드셀 측정값 읽기
{
	int32_t Value = 0;							// 측정값 저장용 변수 초기화
	int32_t ValSum = 0;							// OFF-Set 계산값 저장용 변수 초기화
	int32_t Old_ValSum = 0;							// Old_Value 변수 초기화
	int32_t IN_S_Value = 0;							// 입구체중 저장용 변수 초기화
 	uint16_t Re_cnt_1 = 50;							// 양수 연속 측정 후 초기화 횟수
	CELL.OFF_Set = 0;							// OFF_Set 값 저장용 변수 초기화
//-------------------------------------------------------------------------------------------------
	if(Number == 1)								// 1번 로드셀 이면
	{
		Value = CellRead_1();						// 1번 로드셀 측정값
 		Old_ValSum = CELL.Old_ValSum_1;					// 1번 로드셀 Old_Value_1 값
		CELL.OFF_Set = CELL.OFF_Set_1;					// 1번 로드셀 OFF_Set
		IN_S_Value = IN_Weight_1;					// 1번 로드셀 입구체중
		CAPA = Cell_Capa_1 * 1000;
	}
	else if(Number == 2)							// 2번 로드셀 이면
	{
		Value = CellRead_2();						// 2번 로드셀 측정값
 		Old_ValSum = CELL.Old_ValSum_2;					// 2번 로드셀 Old_Value_2 값
		CELL.OFF_Set = CELL.OFF_Set_2;					// 2번 로드셀 OFF_Set
		IN_S_Value = IN_Weight_2;					// 2번 로드셀 입구체중
		CAPA = Cell_Capa_2 * 1000;
	}
	else if(Number == 3)							// 3번 로드셀 이면
	{
		Value = CellRead_3();						// 3번 로드셀 측정값
 		Old_ValSum = CELL.Old_ValSum_3;					// 3번 로드셀 Old_Value_3 값
		CELL.OFF_Set = CELL.OFF_Set_3;					// 3번 로드셀 OFF_Set
		IN_S_Value = IN_Weight_3;					// 3번 로드셀 입구체중
		CAPA = Cell_Capa_3 * 1000;
	}
//-------------------------------------------------------------------------------------------------
	if(Value > 0)								// 무게 Data 가 있으면
	{
		ValSum = Value - CELL.OFF_Set;					// 무게 Data - OFF_Set
		ValSum = ValSum / 10;						// 낮은 Data 값 지우기
		ValSum = ValSum * 10;						// 낮은 Data 값 지우기

		if(ValSum > 0)							// ValSum 이 0 보다 작으면
		{
			if(ValSum >= Old_ValSum)				// ValSum 이 Old_ValSum 보다크거나 같으면
			{
				if(Number == 1) {Value_Count_1++; if(Value_Count_1 > Re_cnt_1) {CELL.OFF_Set_1 = CELL.OFF_Set + ValSum; Alarm();}}	// Value_Count_1 를 증가 하고, 초기화 값 보다 커지면 초기화 진행
				if(Number == 2) {Value_Count_2++; if(Value_Count_2 > Re_cnt_1) {CELL.OFF_Set_2 = CELL.OFF_Set + ValSum; Alarm();}}	// Value_Count_2 를 증가 하고, 초기화 값 보다 커지면 초기화 진행
				if(Number == 3) {Value_Count_3++; if(Value_Count_3 > Re_cnt_1) {CELL.OFF_Set_3 = CELL.OFF_Set + ValSum; Alarm();}}	// Value_Count_3 를 증가 하고, 초기화 값 보다 커지면 초기화 진행
			}
			else
			{
				if(Number == 1) {CELL.Old_ValSum_1 = ValSum;}	// Old_ValSum_1 을 ValSum 로 저장
				if(Number == 2) {CELL.Old_ValSum_2 = ValSum;}	// Old_ValSum_2 을 ValSum 로 저장
				if(Number == 3) {CELL.Old_ValSum_3 = ValSum;}	// Old_ValSum_3 을 ValSum 로 저장
			}
		}
		
		if(ValSum == 0)
		{
			if     (Number == 1) {Value_Count_1 = 0;}		// 1번 로드셀 이면 Value_Count_1 초기화
			else if(Number == 2) {Value_Count_2 = 0;}		// 2번 로드셀 이면 Value_Count_2 초기화
			else if(Number == 3) {Value_Count_3 = 0;}		// 3번 로드셀 이면 Value_Count_3 초기화
		}
//-------------------------------------------------------------------------------------------------
		if(ValSum < 0 || ValSum > CAPA)					// ValSum 이 0 보다 작거나 CAPA 보다 크면 초기화 카운터
		{
			if     (Number == 1) {Value_Count_1++;}			// 1번 로드셀 이면 Value_Count_1 증가
			else if(Number == 2) {Value_Count_2++;}			// 2번 로드셀 이면 Value_Count_2 증가
			else if(Number == 3) {Value_Count_3++;}			// 3번 로드셀 이면 Value_Count_3 증가

			if(Number == 1)
			{
				if(Value_Count_1 > 5)				// Value_Count_1 가 초기화 값 보다 크면
				{
					Cell_Zero_Set(Number);			// 로드셀 0 값 찾기
					Value_Count_1 = 0;
				}
				else {CELL.OFF_Set_1 = CELL.OFF_Set -5;}	// OFF_Set - 5
				ValSum = 0;					// ValSum 을 0으로 초기화
			}
			else if(Number == 2)
			{
				if(Value_Count_2 > 5)				// Value_Count_2 가 초기화 값 보다 크면
				{
					Cell_Zero_Set(Number);			// 로드셀 0 값 찾기
					Value_Count_1 = 0;
				}
				else {CELL.OFF_Set_2 = CELL.OFF_Set -5;}	// OFF_Set - 5
				ValSum = 0;					// ValSum 을 0으로 초기화
			}
			else if(Number == 3)
			{
				if(Value_Count_3 > 5)				// Value_Count_3 가 초기화 값 보다 크면
				{
					Cell_Zero_Set(Number);			// 로드셀 0 값 찾기
					Value_Count_1 = 0;
				}
				else {CELL.OFF_Set_3 = CELL.OFF_Set -5;}	// OFF_Set - 5
				ValSum = 0;					// ValSum 을 0으로 초기화
			}
		}
		else if(ValSum > 0 && IN_S_Value > ValSum)			// ValSum 이 0 보다 크고 입구체중 보다 작으면
		{
			if(Number == 1)
			{
				CELL.OFF_Set_1 = CELL.OFF_Set + 5;		// OFF_Set + 5
				ValSum = 0;					// ValSum 을 0으로 초기화
			}
			else if(Number == 2)
			{
				CELL.OFF_Set_2 = CELL.OFF_Set + 5;		// OFF_Set + 5
				ValSum = 0;					// ValSum 을 0으로 초기화
			}
			else if(Number == 3)
			{
				CELL.OFF_Set_3 = CELL.OFF_Set + 5;		// OFF_Set + 5
				ValSum = 0;					// ValSum 을 0으로 초기화
			}
		}

		if(ValSum > CAPA) {ValSum = 0;}					// ValSum 이 CAPA(200Kg) 보다 크면
	}
	ValSum = ValSum / 10;							// ValSum 를 10으로 나눔
	return(ValSum);
//-------------------------------------------------------------------------------------------------
// 	if(Value > 0)								// 무게 Data 가 있으면
// 	{
// 		ValSum = Value - CELL.OFF_Set;					// 무게 Data - OFF_Set
// 		ValSum = ValSum / 10;						// 낮은 Data 값 지우기
// 		ValSum = ValSum * 10;						// 낮은 Data 값 지우기
// 
// 		if(ValSum > 0)							// ValSum 이 0 보다 작으면
// 		{
// 	 		printf("ValSum %ld \r\n", ValSum);
// 
// 			if(ValSum >= Old_ValSum)				// ValSum 이 Old_ValSum 보다크거나 같으면 
// 			{
// 				if(Number == 1) {Value_Count_1++; if(Value_Count_1 > Re_cnt_1) {CELL.OFF_Set_1 = CELL.OFF_Set + ValSum; Alarm();}}	// Value_Count_1 를 증가 하고, 초기화 값 보다 커지면 초기화 진행
// 				if(Number == 2) {Value_Count_2++; if(Value_Count_2 > Re_cnt_1) {CELL.OFF_Set_2 = CELL.OFF_Set + ValSum; Alarm();}}	// Value_Count_2 를 증가 하고, 초기화 값 보다 커지면 초기화 진행
// 				if(Number == 3) {Value_Count_3++; if(Value_Count_3 > Re_cnt_1) {CELL.OFF_Set_3 = CELL.OFF_Set + ValSum; Alarm();}}	// Value_Count_3 를 증가 하고, 초기화 값 보다 커지면 초기화 진행
// 			}
// 			else
// 			{
// 				if(Number == 1) {CELL.Old_ValSum_1 = ValSum;}	// Old_ValSum_1 을 ValSum 로 저장
// 				if(Number == 2) {CELL.Old_ValSum_2 = ValSum;}	// Old_ValSum_2 을 ValSum 로 저장
// 				if(Number == 3) {CELL.Old_ValSum_3 = ValSum;}	// Old_ValSum_3 을 ValSum 로 저장 
// 			}
// 		}
// 
// 		if(ValSum == 0)
// 		{
// 			if(Number == 1)	{Value_Count_1 = 0;}			// Value_Count_1 초기화
// 			if(Number == 2) {Value_Count_2 = 0;}			// Value_Count_2 초기화
// 			if(Number == 3) {Value_Count_3 = 0;}			// Value_Count_3 초기화
// 		}
// 
// 		if(ValSum < -IN_S_Value)					// ValSum 이 -입구체중 보다 작으면
// 		{
// //-------------------------------------------------------------------------------------------------
// 			if(Number == 1)						// 1번 로드셀 이면
// 			{
// 				Value_Count_1++;				// 1번 Value_Count 증가
// 				if(Value_Count_1 > Re_cnt_2)			// 1번 Value_Count 가 초기화 값 보다 크면
// 				{
// 					Cell_Zero_Set(1);
// 					Cell_Zero_Set(1);
// 				}
// 			}
// //-------------------------------------------------------------------------------------------------
// 			if(Number == 2)						// 2번 로드셀 이면
// 			{					
// 				Value_Count_2++;				// 2번 Value_Count 증가
// 				if(Value_Count_2 > Re_cnt_2)			// 2번 Value_Count 가 초기화 값 보다 크면
// 				{
// 					Cell_Zero_Set(2);
// 					Cell_Zero_Set(2);
// 				}
// 			}
// //-------------------------------------------------------------------------------------------------
// 			if(Number == 3)						// 3번 로드셀 이면
// 			{
// 				Value_Count_3++;				// 3번 Value_Count 증가
// 				if(Value_Count_3 > Re_cnt_2)			// 3번 Value_Count 가 초기화 값 보다 크면
// 				{
// 					Cell_Zero_Set(3);
// 					Cell_Zero_Set(3);
// 				}
// 			}
// //-------------------------------------------------------------------------------------------------
// 			ValSum = 0;						// ValSum 을 0으로 초기화
// 		}
// 
// 		if(IN_S_Value > ValSum && ValSum > -IN_S_Value)			// ValSum 이 입구체중 보다 작고 -입구체중 보다 크면
// 		{
// 			if(ValSum > 0)						// ValSum 이 0g 보다 크면
// 			{
// 				if     (Number == 1) {CELL.OFF_Set_1 = CELL.OFF_Set + ValSum;}	// 1번 로드셀 이면 OFF_Set + ValSum
// 				else if(Number == 2) {CELL.OFF_Set_2 = CELL.OFF_Set + ValSum;}	// 2번 로드셀 이면 OFF_Set + ValSum
// 				else if(Number == 3) {CELL.OFF_Set_3 = CELL.OFF_Set + ValSum;}	// 3번 로드셀 이면 OFF_Set + ValSum
// 			}
// 			else if(ValSum < 0)					// ValSum 이 0g 보다 작으면
// 			{
// 				if     (Number == 1) {CELL.OFF_Set_1 = CELL.OFF_Set + ValSum;}	// 1번 로드셀 이면 OFF_Set + ValSum
// 				else if(Number == 2) {CELL.OFF_Set_2 = CELL.OFF_Set + ValSum;}	// 2번 로드셀 이면 OFF_Set + ValSum
// 				else if(Number == 3) {CELL.OFF_Set_3 = CELL.OFF_Set + ValSum;}	// 3번 로드셀 이면 OFF_Set + ValSum
// 			}
// 			ValSum = 0;						// ValSum 을 0으로 초기화
// 		}
// 
// 		if(ValSum > CAPA) {ValSum = 0;}					// ValSum 이 CAPA(200Kg) 보다 크면
// 	}
// 	ValSum = ValSum / 10;							// ValSum 를 10으로 나눔
// 	return(ValSum);								// 리턴 ValSum
}

//=================================================================================================
void Cell_Zero_Set(uint8_t Number)						// 로드셀 0 값 찾기
{
	int32_t Value = 0;							// 측정 값 변수 초기화
	int32_t Value_Sum = 0;							// 측정 값 계산 변수 초기화

	if(Number == 1)								// 1번 로드셀 이면
	{
		Value = CellRead_1();						// 1번 로드셀 Data 읽기
		do{
			Value_Sum = CELL.OFF_Set_1 - Value;			// 1번 로드셀 OFF_Set 초기값(20Kg) - Data = Value_Sum
			if     (Value_Sum > 0) {CELL.OFF_Set_1 = CELL.OFF_Set_1 - Value_Sum;}	// Value_Sum 값이 0 보다 크면   OFF_Set - Value_Sum 해서 OFF_Set에 저장
			else if(Value_Sum < 0) {CELL.OFF_Set_1 = CELL.OFF_Set_1 + Value_Sum;}	// Value_Sum 값이 0 보다 작으면 OFF_Set + Value_Sum 해서 OFF_Set에 저장
		}while(Value_Sum != 0);						// !Value_Sum 이 0 일때까지 반복
	}
	else if(Number == 2)							// 2번 로드셀 이면
	{
		Value = CellRead_2();						// 2번 로드셀 Data 읽기
		do{
			Value_Sum = CELL.OFF_Set_2 - Value;			// 2번 로드셀 OFF_Set 초기값(20Kg) - Data = Value_Sum
			if     (Value_Sum > 0) {CELL.OFF_Set_2 = CELL.OFF_Set_2 - Value_Sum;}	// Value_Sum 값이 0 보다 크면   OFF_Set - Value_Sum 해서 OFF_Set에 저장
			else if(Value_Sum < 0) {CELL.OFF_Set_2 = CELL.OFF_Set_2 + Value_Sum;}	// Value_Sum 값이 0 보다 작으면 OFF_Set + Value_Sum 해서 OFF_Set에 저장
		}while(Value_Sum != 0);						// !Value_Sum 이 0 일때까지 반복
	}
	else if(Number == 3)							// 3번 로드셀 이면
	{
		Value = CellRead_3();						// 3번 로드셀 Data 읽기
		do{
	 		Value_Sum = CELL.OFF_Set_3 - Value;			// 3번 로드셀 OFF_Set 초기값(20Kg) - Data = Value_Sum
	 		if     (Value_Sum > 0) {CELL.OFF_Set_3 = CELL.OFF_Set_3 - Value_Sum;}	// Value_Sum 값이 0 보다 크면   OFF_Set - Value_Sum 해서 OFF_Set에 저장
	 		else if(Value_Sum < 0) {CELL.OFF_Set_3 = CELL.OFF_Set_3 + Value_Sum;}	// Value_Sum 값이 0 보다 작으면 OFF_Set + Value_Sum 해서 OFF_Set에 저장
		}while(Value_Sum != 0);						// Value_Sum 이 0 일때까지 반복
	}
	Beep(10);
	_delay_ms(20);
	wdt_reset();								// Reset WDT
}

//=================================================================================================
void IN_Scales_Set(uint8_t Number)						// IN_Scales 설정
{
	DIGIT[2][0] = FND_OFF;							// 로드셀 RO, 입구체중 1의 자리 지우기

	if     (Number == 1) {IN_S = ee_read_word(Cell_IN_S_1_Addr);}		// 1번 로드셀 이면 입구체중값 불러오기
	else if(Number == 2) {IN_S = ee_read_word(Cell_IN_S_2_Addr);}		// 2번 로드셀 이면 입구체중값 불러오기
	else if(Number == 3) {IN_S = ee_read_word(Cell_IN_S_2_Addr);}		// 3번 로드셀 이면 입구체중값 불러오기
//-------------------------------------------------------------------------------------------------
	IN_S_10000 = IN_S / 10000;						// 로드셀 입구체중 10000의 자리
	IN_S_1000  = IN_S % 10000/ 1000;					// 로드셀 입구체중  1000의 자리
	IN_S_100   = IN_S % 1000 / 100;						// 로드셀 입구체중   100의 자리
	IN_S_10    = IN_S % 100 / 10;						// 로드셀 입구체중    10의 자리
	IN_S_1     = IN_S % 10;							// 로드셀 입구체중     1의 자리
//-------------------------------------------------------------------------------------------------
	while(SET.Menu)								// 메뉴모드 이면 반복
	{
		SET.Blink = 1;							// 자릿수 깜박임 설정

		switch(KEY.Count)						// 선택 Key 가 눌린횟수
		{
			case 0:							// 로드셀 입구체중 10000의 자리
				DIGIT[2][3] = fnd_tbl[IN_S_1];			// 로드셀 입구체중 1의 자리 표시
				if(Blink) {DIGIT[1][3] = FND_OFF;}		// Blink = 1 이면 로드셀 입구체중 10000의 자리 OFF
				else	  {DIGIT[1][3] = fnd_tbl[IN_S_10000];}	// Blink = 0 이면 로드셀 입구체중 10000의 자리 표시

				if(KEY.Buff)					// Key 입력이 있으면
				{
					if     (KEY.Key == 1 && IN_S_10000 < 1) {IN_S_10000++; KEY.Key = 0;}	// 업  버튼 이면 입구체중 10000의 자리 증가
					else if(KEY.Key == 3 && IN_S_10000 > 0) {IN_S_10000--; KEY.Key = 0;}	// 다운버튼 이면 입구체중 10000의 자리 감소
				}
			break;

			case 1:							// 로드셀 입구체중 1000의 자리
				DIGIT[1][3] = fnd_tbl[IN_S_10000];		// 로드셀 입구체중 10000의 자리 표시
				if(Blink) {DIGIT[1][2] = FND_OFF;}			// Blink = 1 이면 로드셀 입구체중 1000의 자리 OFF
				else	  {DIGIT[1][2] = fnd_tbl[IN_S_1000]+FND_Point;}	// Blink = 0 이면 로드셀 입구체중 1000의 자리 표시

				if(KEY.Buff)					// Key 입력이 있으면
				{
					if     (KEY.Key == 1 && IN_S_1000 < 9) {IN_S_1000++; KEY.Key = 0;}	// 업  버튼 이면 입구체중 1000의 자리 증가
					else if(KEY.Key == 3 && IN_S_1000 > 0) {IN_S_1000--; KEY.Key = 0;}	// 다운버튼 이면 입구체중 1000의 자리 감소
				}
			break;

			case 2:							// 로드셀 입구체중 100의 자리
				DIGIT[1][2] = fnd_tbl[IN_S_1000]+FND_Point;	// 로드셀 입구체중 1000의 자리 표시
				if(Blink) {DIGIT[1][1] = FND_OFF;}		// Blink = 1 이면 로드셀 입구체중 100의 자리 OFF
				else	  {DIGIT[1][1] = fnd_tbl[IN_S_100];}	// Blink = 0 이면 로드셀 입구체중 100의 자리 표시

				if(KEY.Buff)					// Key 입력이 있으면
				{
					if     (KEY.Key == 1 && IN_S_100 < 9) {IN_S_100++; KEY.Key = 0;}	// 업  버튼 이면 입구체중 100의 자리 증가
					else if(KEY.Key == 3 && IN_S_100 > 0) {IN_S_100--; KEY.Key = 0;}	// 다운버튼 이면 입구체중 100의 자리 감소
				}
			break;

			case 3:							// 로드셀 입구체중 10의 자리
				DIGIT[1][1] = fnd_tbl[IN_S_100];		// 로드셀 입구체중 100의 자리 표시
				if(Blink) {DIGIT[1][0] = FND_OFF;}		// Blink = 1 이면 로드셀 입구체중 10의 자리 OFF
				else	  {DIGIT[1][0] = fnd_tbl[IN_S_10];}	// Blink = 0 이면 로드셀 입구체중 10의 자리 표시

				if(KEY.Buff)					// Key 입력이 있으면
				{
					if     (KEY.Key == 1 && IN_S_10 < 9) {IN_S_10++; KEY.Key = 0;}		// 업  버튼 이면 입구체중 10의 자리 증가
					else if(KEY.Key == 3 && IN_S_10 > 0) {IN_S_10--; KEY.Key = 0;}		// 다운버튼 이면 입구체중 10의 자리 감소
				}
			break;

			case 4:							// 로드셀 입구체중 1의 자리
				DIGIT[1][0] = fnd_tbl[IN_S_10];			// 로드셀 입구체중 10의 자리 표시
				if(Blink) {DIGIT[2][3] = FND_OFF;}		// Blink = 1 이면 로드셀 입구체중 1의 자리 OFF
				else	  {DIGIT[2][3] = fnd_tbl[IN_S_1];}	// Blink = 0 이면 로드셀 입구체중 1의 자리 표시

				if(KEY.Buff)					// Key 입력이 있으면
				{
					if     (KEY.Key == 1 && IN_S_1 < 9) {IN_S_1++; KEY.Key = 0;}		// 업  버튼 이면 입구체중 1의 자리 증가
					else if(KEY.Key == 3 && IN_S_1 > 0) {IN_S_1--; KEY.Key = 0;}		// 다운버튼 이면 입구체중 1의 자리 감소
				}
			break;

			case 5:
				KEY.Count = 0;					// 반복을 위해 KEY.Count 를 초기화 시킴
			break;
		}
		wdt_reset();							// 워치독 리셋
	}
//-------------------------------------------------------------------------------------------------
	IN_S = 0;								// 로드셀 입구체중 초기화
	IN_S += (IN_S_10000 * 10000);						// 로드셀 입구체중 10000의 자리
	IN_S += (IN_S_1000  * 1000);						// 로드셀 입구체중  1000의 자리
	IN_S += (IN_S_100   * 100);						// 로드셀 입구체중   100의 자리
	IN_S += (IN_S_10    * 10);						// 로드셀 입구체중    10의 자리
	IN_S += (IN_S_1     * 1);						// 로드셀 입구체중     1의 자리
//-------------------------------------------------------------------------------------------------
	if     (Number == 1) {ee_write_word(Cell_IN_S_1_Addr, IN_S);}		// 1번 로드셀 이면 입구체중값 저장
	else if(Number == 2) {ee_write_word(Cell_IN_S_2_Addr, IN_S);}		// 2번 로드셀 이면 입구체중값 저장
	else if(Number == 3) {ee_write_word(Cell_IN_S_3_Addr, IN_S);}		// 3번 로드셀 이면 입구체중값 저장

	SET.Menu = 1;								// Menu 모드 유지
	SET.Select = 0;								// Select 버튼 초기화
	SET.Blink = 0;								// 자릿수 깜박임 초기화
	KEY.Count = 0;								// 선택 Key 가 눌린횟수 초기화
}

//=================================================================================================
void RO_Set(uint8_t Number)							// RO 설정
{
	DIGIT[2][0] = FND_OFF;							// 로드셀 RO, 입구체중 1의 자리 지우기

	if     (Number == 1) {RO = ee_read_dword(Cell_RO_1_Addr);}		// 1번 로드셀 이면 RO값 불러오기
	else if(Number == 2) {RO = ee_read_dword(Cell_RO_2_Addr);}		// 2번 로드셀 이면 RO값 불러오기
	else if(Number == 3) {RO = ee_read_dword(Cell_RO_3_Addr);}		// 3번 로드셀 이면 RO값 불러오기
//-------------------------------------------------------------------------------------------------
	RO_10000 = RO / 10000;							// 로드셀 RO 10000의 자리
	RO_1000  = RO % 10000 / 1000;						// 로드셀 RO  1000의 자리
	RO_100   = RO % 1000 / 100;						// 로드셀 RO   100의 자리
	RO_10    = RO % 100 / 10;						// 로드셀 RO    10의 자리
	RO_1     = RO % 10;							// 로드셀 RO     1의 자리
//-------------------------------------------------------------------------------------------------
	while(SET.Menu)								// 메뉴가 1 이면 반복
	{
		SET.Blink = 1;							// 자릿수 깜박임 설정

		switch(KEY.Count)						// 선택 Key 가 눌린횟수
		{
			case 0:							// 로드셀 RO 10000의 자리
				DIGIT[2][3] = fnd_tbl[RO_1];			// 로드셀 RO 1의 자리 표시
				if(Blink) {DIGIT[1][3] = FND_OFF;}				// Blink = 1 이면 로드셀 RO 10000의 자리 OFF
				else	  {DIGIT[1][3] = fnd_tbl[RO_10000] + FND_Point;}	// Blink = 0 이면 로드셀 RO 10000의 자리 표시

				if(KEY.Buff)					// Key 입력이 있으면
				{
					if     (KEY.Key == 1 && RO_10000 < 8) {RO_10000++; KEY.Key = 0;}	// 업  버튼 이면 RO값 10000의 자리 증가
					else if(KEY.Key == 3 && RO_10000 > 1) {RO_10000--; KEY.Key = 0;}	// 다운버튼 이면 RO값 10000의 자리 감소
				}
			break;

			case 1:							// 로드셀 RO 1000의 자리
				DIGIT[1][3] = fnd_tbl[RO_10000] + FND_Point;	// 로드셀 RO 10000의 자리 표시
				if(Blink) {DIGIT[1][2] = FND_OFF;}		// Blink = 1 이면 로드셀 RO 1000의 자리 OFF
				else	  {DIGIT[1][2] = fnd_tbl[RO_1000];}	// Blink = 0 이면 로드셀 RO 1000의 자리 표시

				if(KEY.Buff)					// Key 입력이 있으면
				{
					if     (KEY.Key == 1 && RO_1000 < 9) {RO_1000++; KEY.Key = 0;}		// 업  버튼 이면 RO값 1000의 자리 증가
					else if(KEY.Key == 3 && RO_1000 > 0) {RO_1000--; KEY.Key = 0;}		// 다운버튼 이면 RO값 1000의 자리 감소
				}
			break;

			case 2:							// 로드셀 RO 100의 자리
				DIGIT[1][2] = fnd_tbl[RO_1000];			// 로드셀 RO 1000의 자리 표시
				if(Blink) {DIGIT[1][1] = FND_OFF;}		// Blink = 1 이면 로드셀 RO 100의 자리 OFF
				else	  {DIGIT[1][1] = fnd_tbl[RO_100];}	// Blink = 0 이면 로드셀 RO 100의 자리 표시

				if(KEY.Buff)					// Key 입력이 있으면
				{
					if     (KEY.Key == 1 && RO_100 < 9) {RO_100++; KEY.Key = 0;}		// 업  버튼 이면 RO값 100의 자리 증가
					else if(KEY.Key == 3 && RO_100 > 0) {RO_100--; KEY.Key = 0;}		// 다운버튼 이면 RO값 100의 자리 감소
				}
			break;

			case 3:							// 로드셀 RO 10의 자리
				DIGIT[1][1] = fnd_tbl[RO_100];			// 로드셀 RO 100의 자리 표시
				if(Blink) {DIGIT[1][0] = FND_OFF;}		// Blink = 1 이면 로드셀 RO 10의 자리 OFF
				else	  {DIGIT[1][0] = fnd_tbl[RO_10];}	// Blink = 0 이면 로드셀 RO 10의 자리 표시

				if(KEY.Buff)					// Key 입력이 있으면
				{
					if     (KEY.Key == 1 && RO_10 < 9) {RO_10++; KEY.Key = 0;}		// 업  버튼 이면 RO값 10의 자리 증가
					else if(KEY.Key == 3 && RO_10 > 0) {RO_10--; KEY.Key = 0;}		// 다운버튼 이면 RO값 10의 자리 감소
				}
			break;

			case 4:							// 로드셀 RO 1의 자리
				DIGIT[1][0] = fnd_tbl[RO_10];			// 로드셀 RO 10의 자리 표시
				if(Blink) {DIGIT[2][3] = FND_OFF;}		// Blink = 1 이면 로드셀 RO 1의 자리 OFF
				else	  {DIGIT[2][3] = fnd_tbl[RO_1];}	// Blink = 0 이면 로드셀 RO 1의 자리 표시

				if(KEY.Buff)					// Key 입력이 있으면
				{
					if     (KEY.Key == 1 && RO_1 < 9) {RO_1++; KEY.Key = 0;}		// 업  버튼 이면 RO값 1의 자리 증가
					else if(KEY.Key == 3 && RO_1 > 0) {RO_1--; KEY.Key = 0;}		// 다운버튼 이면 RO값 1의 자리 감소
				}
			break;

			case 5:
				KEY.Count = 0;					// 반복을 위해 KEY.Count 를 초기화 시킴
			break;
		}
		wdt_reset();							// 워치독 리셋
	}
//-------------------------------------------------------------------------------------------------
	RO = 0;
	RO += (RO_10000 * 10000);						// 로드셀 RO 10000의 자리
	RO += (RO_1000  * 1000);						// 로드셀 RO  1000의 자리
	RO += (RO_100   * 100);							// 로드셀 RO   100의 자리
	RO += (RO_10    * 10);							// 로드셀 RO    10의 자리
	RO += (RO_1     * 1);							// 로드셀 RO     1의 자리
//-------------------------------------------------------------------------------------------------
	if     (Number == 1) {ee_write_dword(Cell_RO_1_Addr, RO);}		// 1번 로드셀 이면 정격출력값 저장
	else if(Number == 2) {ee_write_dword(Cell_RO_2_Addr, RO);}		// 2번 로드셀 이면 정격출력값 저장
	else if(Number == 3) {ee_write_dword(Cell_RO_3_Addr, RO);}		// 3번 로드셀 이면 정격출력값 저장

	SET.Menu = 1;								// Menu 모드 유지
	SET.Select = 0;								// Select 버튼 초기화
	SET.Blink = 0;								// 자릿수 깜박임 초기화
	KEY.Count = 0;								// 선택 Key 가 눌린횟수 초기화
}

//=================================================================================================
void Interval_Set(uint8_t Number)						// Interval 설정
{
	uint32_t Value = 0;							// Value 변수 초기화

	if     (Number == 1) {RO = ee_read_dword(Cell_RO_1_Addr);}		// 1번 로드셀 이면 RO값 불러오기
	else if(Number == 2) {RO = ee_read_dword(Cell_RO_2_Addr);}		// 2번 로드셀 이면 RO값 불러오기
	else if(Number == 3) {RO = ee_read_dword(Cell_RO_3_Addr);}		// 3번 로드셀 이면 RO값 불러오기
//-------------------------------------------------------------------------------------------------
	RO_10000 = RO / 10000;							// 로드셀 RO 10000의 자리
	RO_1000  = RO % 10000 / 1000;						// 로드셀 RO  1000의 자리
	RO_100   = RO % 1000 / 100;						// 로드셀 RO   100의 자리
	RO_10    = RO % 100 / 10;						// 로드셀 RO    10의 자리
	RO_1     = RO % 10;
//-------------------------------------------------------------------------------------------------
	while(SET.Menu && SET.Select)						// 메뉴가 1 이면 반복
	{
		Timer.RegExitTimer = 0;						// 무게의 연속 측정을 위해 RegExitTimer 초기화 

		if     (Number == 1) {Value_Count_1 = 0;}
		else if(Number == 2) {Value_Count_2 = 0;}
		else if(Number == 3) {Value_Count_3 = 0;}

		if(KEY.Buff)							// Key 입력이 있으면
		{
			if     (KEY.Key == 1) {RO = RO + 10; KEY.Key = 0;}	// 업  버튼 이면 RO 값 1의 자리 증가
			else if(KEY.Key == 3) {RO = RO - 10; KEY.Key = 0;}	// 다운버튼 이면 RO 값 1의 자리 감소

			DIGIT[1][3] = fnd_tbl[RO / 10000];			// 로드셀 RO 10000의 자리
			DIGIT[1][2] = fnd_tbl[RO % 10000 / 1000];		// 로드셀 RO  1000의 자리
			DIGIT[1][1] = fnd_tbl[RO % 1000 / 100 ];		// 로드셀 RO   100의 자리
			DIGIT[1][0] = fnd_tbl[RO % 100 / 10];			// 로드셀 RO    10의 자리

			DIGIT[2][3] = fnd_tbl[RO % 10];				// 로드셀 RO    1의 자리
		}

		if     (Number == 1) {Cell_RO_1 = RO; Value = CELL.Value_1 = Read_Load_Cell(Number);}	// 1번 로드셀 측정값
		else if(Number == 2) {Cell_RO_2 = RO; Value = CELL.Value_2 = Read_Load_Cell(Number);}	// 2번 로드셀 측정값
		else if(Number == 3) {Cell_RO_3 = RO; Value = CELL.Value_3 = Read_Load_Cell(Number);}	// 3번 로드셀 측정값

		if(Value)							// 측정체중이 있으면
		{
			if     (Number == 1) {CELL.Value_1 = CELL.Value_1; INT_Desplay(Number);}	// 1번 로드셀 이면 측정체중 표시
			else if(Number == 2) {CELL.Value_2 = CELL.Value_2; INT_Desplay(Number);}	// 2번 로드셀 이면 측정체중 표시
			else if(Number == 3) {CELL.Value_3 = CELL.Value_3; INT_Desplay(Number);}	// 3번 로드셀 이면 측정체중 표시
		}
		else if(Value == 0) {INT_Desplay(Number);}			// 측정체중이 0 이면
		wdt_reset();							// 워치독 리셋
	}
	if     (Number == 1) {ee_write_dword(Cell_RO_1_Addr, RO);}		// 1번 로드셀 이면 정격출력값 저장
	else if(Number == 2) {ee_write_dword(Cell_RO_2_Addr, RO);}		// 2번 로드셀 이면 정격출력값 저장
	else if(Number == 3) {ee_write_dword(Cell_RO_3_Addr, RO);}		// 3번 로드셀 이면 정격출력값 저장
}

//=================================================================================================
void CAPA_Set(uint8_t Number)							// CAPA 설정
{
	DIGIT[2][0] = FND_OFF;							// 로드셀 RO, 입구체중 1의 자리 지우기

	if     (Number == 1) {CAPA = ee_read_word(Cell_Capa_1_Addr);}		// 1번 로드셀 이면 용량값 불러오기
	else if(Number == 2) {CAPA = ee_read_word(Cell_Capa_2_Addr);}		// 2번 로드셀 이면 용량값 불러오기
	else if(Number == 3) {CAPA = ee_read_word(Cell_Capa_3_Addr);}		// 3번 로드셀 이면 용량값 불러오기
//-------------------------------------------------------------------------------------------------
	CAPA_1000 = CAPA / 1000;						// 로드셀 CAPA 1000의 자리
	CAPA_100  = CAPA % 1000 / 100;						// 로드셀 CAPA 100의 자리
	CAPA_10   = CAPA % 100 / 10;						// 로드셀 CAPA  10의 자리
	CAPA_1    = CAPA % 10;							// 로드셀 CAPA   1의 자리
//-------------------------------------------------------------------------------------------------
	while(SET.Menu)								// 메뉴가 1 이면 반복
	{
		SET.Blink = 1;							// 자릿수 깜박임 설정

		switch(KEY.Count)						// 선택 Key 가 눌린횟수
		{
			case 0:							// 로드셀 CAPA 1000의 자리
				DIGIT[1][0] = fnd_tbl[CAPA_1];			// 로드셀 CAPA    1의 자리 표시
				if(Blink) {DIGIT[1][3] = FND_OFF;}		// Blink = 1 이면 로드셀 CAPA 100의 자리 OFF
				else	  {DIGIT[1][3] = fnd_tbl[CAPA_1000];}	// Blink = 0 이면 로드셀 CAPA 100의 자리 표시

				if(KEY.Buff)					// Key 입력이 있으면
				{
					if     (KEY.Key == 1 && CAPA_1000 < 1) {CAPA_1000++; KEY.Key = 0;}	// 업  버튼 이면 CAPA값 1000의 자리 증가
					else if(KEY.Key == 3 && CAPA_1000 > 0) {CAPA_1000--; KEY.Key = 0;}	// 다운버튼 이면 CAPA값 1000의 자리 감소
				}
			break;

			case 1:							// 로드셀 CAPA  100의 자리
				DIGIT[1][3] = fnd_tbl[CAPA_1000];		// 로드셀 CAPA 1000의 자리 표시
				if(Blink) {DIGIT[1][2] = FND_OFF;}		// Blink = 1 이면 로드셀 CAPA 100의 자리 OFF
				else	  {DIGIT[1][2] = fnd_tbl[CAPA_100];}	// Blink = 0 이면 로드셀 CAPA 100의 자리 표시

				if(KEY.Buff)					// Key 입력이 있으면
				{
					if     (KEY.Key == 1 && CAPA_100 < 5) {CAPA_100++; KEY.Key = 0;}	// 업  버튼 이면 CAPA값 100의 자리 증가
					else if(KEY.Key == 3 && CAPA_100 > 0) {CAPA_100--; KEY.Key = 0;}	// 다운버튼 이면 CAPA값 100의 자리 감소
				}
			break;

			case 2:							// 로드셀 CAPA 10의 자리
				DIGIT[1][2] = fnd_tbl[CAPA_100];		// 로드셀 CAPA 100의 자리 표시
				if(Blink) {DIGIT[1][1] = FND_OFF;}		// Blink = 1 이면 로드셀 CAPA 10의 자리 OFF
				else	  {DIGIT[1][1] = fnd_tbl[CAPA_10];}	// Blink = 0 이면 로드셀 CAPA 10의 자리 표시

				if(KEY.Buff)					// Key 입력이 있으면
				{
					if     (KEY.Key == 1 && CAPA_10 < 9) {CAPA_10++; KEY.Key = 0;}	// 업  버튼 이면 CAPA값 10의 자리 증가
					else if(KEY.Key == 3 && CAPA_10 > 0) {CAPA_10--; KEY.Key = 0;}	// 다운버튼 이면 CAPA값 10의 자리 감소
				}
			break;

			case 3:							// 로드셀 CAPA 1의 자리
				DIGIT[1][1] = fnd_tbl[CAPA_10];			// 로드셀 CAPA 10의 자리 표시
				if(Blink) {DIGIT[1][0] = FND_OFF;}		// Blink = 1 이면 로드셀 CAPA 1의 자리 OFF
				else	  {DIGIT[1][0] = fnd_tbl[CAPA_1];}	// Blink = 0 이면 로드셀 CAPA 1의 자리 표시

				if(KEY.Buff)					// Key 입력이 있으면
				{
					if     (KEY.Key == 1 && CAPA_1 < 9) {CAPA_1++; KEY.Key = 0;}	// 업  버튼 이면 CAPA값 1의 자리 증가
					else if(KEY.Key == 3 && CAPA_1 > 0) {CAPA_1--; KEY.Key = 0;}	// 다운버튼 이면 CAPA값 1의 자리 감소
				}
			break;

			case 4:
				KEY.Count = 0;					// 반복을 위해 Count를 최저로 감소 시킴
			break;
		}
		wdt_reset();							// 워치독 리셋
	}
//-------------------------------------------------------------------------------------------------
	CAPA = 0;
	CAPA += (CAPA_1000 * 1000);						// 로드셀 CAPA 1000의 자리
	CAPA += (CAPA_100  * 100);						// 로드셀 CAPA  100의 자리
	CAPA += (CAPA_10   * 10);						// 로드셀 CAPA   10의 자리
	CAPA += (CAPA_1    * 1);						// 로드셀 CAPA    1의 자리
//-------------------------------------------------------------------------------------------------
	if     (Number == 1) {ee_write_word(Cell_Capa_1_Addr, CAPA);}		// 1번 로드셀 이면 용량값 저장
	else if(Number == 2) {ee_write_word(Cell_Capa_2_Addr, CAPA);}		// 2번 로드셀 이면 용량값 저장
	else if(Number == 3) {ee_write_word(Cell_Capa_3_Addr, CAPA);}		// 3번 로드셀 이면 용량값 저장

	SET.Menu = 1;								// Menu 모드 유지
	SET.Select = 0;								// Select 버튼 초기화
	SET.Blink = 0;								// 자릿수 깜박임 초기화
	KEY.Count = 0;								// 선택 Key 가 눌린횟수 초기화
}

//=================================================================================================
void CNT_Set(uint8_t Number)							// 갯수 설정
{
	FND_Value_Data(0, 2, 12);						// FND 0 ~ 2 FND_OFF

	while(SET.Menu && SET.Select)						// 메뉴 와 선택 이 모두 1 이면 반복
	{
		DIGIT[0][3] = FND_c;						// FND 0 에 cnt 표시
		DIGIT[0][2] = FND_n;
		DIGIT[0][1] = FND_t;
		DIGIT[0][0] = fnd_tbl[Number];					// FND 0 에 Cell 번호 표시

		DIGIT[1][3] = FND_OFF;						// FND 1 OFF
		DIGIT[1][2] = FND_OFF;
		DIGIT[1][1] = fnd_tbl[CNT / 10];				// FND 1 에 CNT 번호 표시
		DIGIT[1][0] = fnd_tbl[CNT % 10];

		DIGIT[2][0] = FND_OFF;						// 로드셀 RO, 입구체중 1의 자리 지우기

		if(KEY.Buff)							// Key 입력이 있으면
		{
			if     (KEY.Key == 1 && CNT < 4) {CNT = CNT * 2; KEY.Key = 0;}	// 업  버튼 이면 갯수 설정값 증가
			else if(KEY.Key == 3 && CNT > 1) {CNT = CNT / 2; KEY.Key = 0;}	// 다운버튼 이면 갯수 설정값 감소
		}
		wdt_reset();							// 워치독 리셋
	}
//-------------------------------------------------------------------------------------------------
	if     (Number == 1) {ee_write_byte(Cell_CNT_1_Addr, CNT);}		// 1번 로드셀 이면 갯수 설정값을 EEPROM 에 저장
	else if(Number == 2) {ee_write_byte(Cell_CNT_2_Addr, CNT);}		// 2번 로드셀 이면 갯수 설정값을 EEPROM 에 저장
	else if(Number == 3) {ee_write_byte(Cell_CNT_3_Addr, CNT);}		// 3번 로드셀 이면 갯수 설정값을 EEPROM 에 저장
}

//=================================================================================================
void ID_Set(uint8_t Number)							// ID 설정
{
	FND_Value_Data(0, 2, 12);						// FND 0 ~ 2 FND_OFF

	while(SET.Menu && SET.Select)						// 메뉴 와 선택 이 모두 1 이면 반복
	{
		DIGIT[0][3] = FND_I;						// FND 0 에 ID 표시
		DIGIT[0][2] = FND_d;
		DIGIT[0][1] = FND_MINUS;
		DIGIT[0][0] = fnd_tbl[Number];					// FND 0 에 Cell 번호 표시

		DIGIT[1][3] = FND_OFF;						// FND 1 OFF
		DIGIT[1][2] = FND_OFF;
		DIGIT[1][1] = fnd_tbl[ID / 10];					// FND 1 에 ID 번호 표시
		DIGIT[1][0] = fnd_tbl[ID % 10];

		DIGIT[2][0] = FND_OFF;						// 로드셀 RO, 입구체중 1의 자리 지우기

		if(KEY.Buff)							// Key 입력이 있으면
		{
			if     (KEY.Key == 1 && ID < 60) {ID++; KEY.Key = 0;}	// 업  버튼 이면 ID값 증가
			else if(KEY.Key == 3 && ID > 0)  {ID--; KEY.Key = 0;}	// 다운버튼 이면 ID값 감소
		}
		wdt_reset();							// 워치독 리셋
	}
//-------------------------------------------------------------------------------------------------
	if     (Number == 1) {ee_write_byte(Cell_ID_1_Addr, ID);}		// 1번 로드셀 이면 ID 를 EEPROM 에 저장
	else if(Number == 2) {ee_write_byte(Cell_ID_2_Addr, ID);}		// 2번 로드셀 이면 ID 를 EEPROM 에 저장
	else if(Number == 3) {ee_write_byte(Cell_ID_3_Addr, ID);}		// 3번 로드셀 이면 ID 를 EEPROM 에 저장
}

//=================================================================================================
void Cell_Select(uint8_t Number)						// 로드셀 선택
{
	uint8_t Count = 1;							// 카운트 변수 초기화

	SET.Select = 0;								// Select 버튼 초기화
	FND_Value_Data(0, 2, 12);						// FND 0 ~ 2 FND_OFF
//-------------------------------------------------------------------------------------------------
	if(Number == 1)								// 1번 로드셀 이면
	{
		ID   = ee_read_byte(Cell_ID_1_Addr);				// 1번 로드셀 ID 값
		CNT  = ee_read_byte(Cell_CNT_1_Addr);				// 1번 로드셀 갯수 설정값
		CAPA = ee_read_word(Cell_Capa_1_Addr);				// 1번 로드셀 CAPA 값
		RO   = ee_read_dword(Cell_RO_1_Addr);				// 1번 로드셀 RO 값
// 		INT  = ee_read_word(Cell_Interval_1_Addr);			// 1번 로드셀 보정 값
		IN_S = ee_read_word(Cell_IN_S_1_Addr);				// 1번 로드셀 입구체중 값
	}
	else if(Number == 2)							// 2번 로드셀 이면
	{
		ID   = ee_read_byte(Cell_ID_2_Addr);				// 2번 로드셀 ID 값
		CNT  = ee_read_byte(Cell_CNT_2_Addr);				// 2번 로드셀 갯수 설정값
		CAPA = ee_read_word(Cell_Capa_2_Addr);				// 2번 로드셀 CAPA 값
		RO   = ee_read_dword(Cell_RO_2_Addr);				// 2번 로드셀 RO 값
// 		INT  = ee_read_word(Cell_Interval_2_Addr);			// 2번 로드셀 보정 값
		IN_S = ee_read_word(Cell_IN_S_2_Addr);				// 2번 로드셀 입구체중 값
	}
	else if(Number == 3)							// 3번 로드셀 이면
	{
		ID   = ee_read_byte(Cell_ID_3_Addr);				// 3번 로드셀 ID 값
		CNT  = ee_read_byte(Cell_CNT_3_Addr);				// 3번 로드셀 갯수 설정값
		CAPA = ee_read_word(Cell_Capa_3_Addr);				// 3번 로드셀 CAPA 값
		RO   = ee_read_dword(Cell_RO_3_Addr);				// 3번 로드셀 RO 값
// 		INT  = ee_read_word(Cell_Interval_3_Addr);			// 3번 로드셀 보정 값
		IN_S = ee_read_word(Cell_IN_S_3_Addr);				// 3번 로드셀 입구체중 값
	}
//-------------------------------------------------------------------------------------------------
	CAPA_1000 = CAPA / 1000;						// 로드셀 CAPA 1000의 자리
	CAPA_100  = CAPA % 1000 / 100;						// 로드셀 CAPA  100의 자리
	CAPA_10   = CAPA % 100 / 10;						// 로드셀 CAPA   10의 자리
	CAPA_1    = CAPA % 10;							// 로드셀 CAPA    1의 자리
//-------------------------------------------------------------------------------------------------
	RO_10000 = RO / 10000;							// 로드셀 RO 10000의 자리
	RO_1000  = RO % 10000 / 1000;						// 로드셀 RO  1000의 자리
	RO_100   = RO % 1000 / 100;						// 로드셀 RO   100의 자리
	RO_10    = RO % 100 / 10;						// 로드셀 RO    10의 자리
	RO_1     = RO % 10;							// 로드셀 RO     1의 자리
//-------------------------------------------------------------------------------------------------
	IN_S_10000 = IN_S / 10000;						// 입구 체중 10.000kg의 자리
	IN_S_1000 = IN_S % 10000/ 1000;						// 입구 체중  1.000kg의 자리
	IN_S_100 = IN_S % 1000 / 100;						// 입구 체중  0.100kg의 자리
	IN_S_10 = IN_S % 100 / 10;						// 입구 체중  0.010kg의 자리
	IN_S_1 = IN_S % 10;							// 입구 체중  0.001kg의 자리
//-------------------------------------------------------------------------------------------------
	while(SET.Menu)								// 메뉴가 1 이면 반복
	{
		if(KEY.Buff)							// Key 입력이 있으면
		{
			if     (KEY.Key == 1 && Count < 7) {Count++; KEY.Key = 0;}	// 업  버튼 이면 Count 증가
			else if(KEY.Key == 3 && Count > 0) {Count--; KEY.Key = 0;}	// 다운버튼 이면 Count 감소
		}
		DIGIT[2][0] = fnd_tbl[Count];					// FND 2.0 에 Count 표시

		switch(Count)							// Count
		{
			case 0:							// Count = 0 이면
				Count = 6;					// 반복을 위해 Count를 최고로 증가 시킴
			break;

			case 1:							// Count = 1 이면
				DIGIT[0][3] = FND_OFF;				// FND 0 에 id 표시
				DIGIT[0][2] = FND_OFF;
				DIGIT[0][1] = FND_I;
				DIGIT[0][0] = FND_d;

				DIGIT[1][3] = FND_OFF;				
				DIGIT[1][2] = FND_OFF;
				DIGIT[1][1] = fnd_tbl[ID / 10];			// FND 1 에 ID값 표시
				DIGIT[1][0] = fnd_tbl[ID % 10];

				DIGIT[2][3] = FND_OFF;				// 로드셀 RO, 입구체중 1의 자리 지우기

				if(SET.Select) {ID_Set(Number);}		// Cell ID 설정
			break;

			case 2:							// Count = 2 이면
				DIGIT[0][3] = FND_OFF;				// FND 0 에 cnt 표시
				DIGIT[0][2] = FND_c;
				DIGIT[0][1] = FND_n;
				DIGIT[0][0] = FND_t;

				DIGIT[1][3] = FND_OFF;
				DIGIT[1][2] = FND_OFF;
				DIGIT[1][1] = fnd_tbl[CNT / 10];		// FND 1 에 갯수 설정값 표시
				DIGIT[1][0] = fnd_tbl[CNT % 10];

				DIGIT[2][3] = FND_OFF;				// 로드셀 RO, 입구체중 1의 자리 지우기

				if(SET.Select) {CNT_Set(Number);}		// Cell 갯수 설정
			break;

			case 3:							// Count = 3 이면
				DIGIT[0][3] = FND_C;				// FND 0 에 Capa 표시
				DIGIT[0][2] = FND_a;
				DIGIT[0][1] = FND_P;
				DIGIT[0][0] = FND_a;

				DIGIT[1][3] = fnd_tbl[CAPA_1000];		// 로드셀 CAPA 1000의 자리
				DIGIT[1][2] = fnd_tbl[CAPA_100 ];		// 로드셀 CAPA  100의 자리
				DIGIT[1][1] = fnd_tbl[CAPA_10  ];		// 로드셀 CAPA   10의 자리
				DIGIT[1][0] = fnd_tbl[CAPA_1   ];		// 로드셀 CAPA    1의 자리

				DIGIT[2][3] = FND_OFF;				// 로드셀 RO, 입구체중 1의 자리 지우기

				if(SET.Select) {CAPA_Set(Number);}		// 로드셀 CAPA 설정
			break;

			case 4:							// Count = 4 이면
				DIGIT[0][3] = FND_OFF;				// FND 0 에 ro 표시
				DIGIT[0][2] = FND_OFF;
				DIGIT[0][1] = FND_r;
				DIGIT[0][0] = FND_o;

				DIGIT[1][3] = fnd_tbl[RO_10000]+FND_Point;	// 로드셀 RO 10000의 자리 FND 1 에 RO 값 표시
				DIGIT[1][2] = fnd_tbl[RO_1000 ];		// 로드셀 RO  1000의 자리
				DIGIT[1][1] = fnd_tbl[RO_100  ];		// 로드셀 RO   100의 자리
				DIGIT[1][0] = fnd_tbl[RO_10   ];		// 로드셀 RO    10의 자리

				DIGIT[2][3] = fnd_tbl[RO_1    ];		// 로드셀 RO     1의 자리 FND 2 에 RO값 표시

				if(SET.Select) {RO_Set(Number);}		// 로드셀 RO 설정
			break;

			case 5:							// Count = 5 이면
				DIGIT[0][3] = FND_S;				// FND 0 에 Scal 표시
				DIGIT[0][2] = FND_c;
				DIGIT[0][1] = FND_a;
				DIGIT[0][0] = FND_l;

				DIGIT[1][3] = fnd_tbl[RO_10000]+FND_Point;	// 로드셀 RO 10000의 자리 FND 1 에 RO 값 표시
				DIGIT[1][2] = fnd_tbl[RO_1000 ];		// 로드셀 RO  1000의 자리
				DIGIT[1][1] = fnd_tbl[RO_100  ];		// 로드셀 RO   100의 자리
				DIGIT[1][0] = fnd_tbl[RO_10   ];		// 로드셀 RO    10의 자리

				DIGIT[2][3] = fnd_tbl[RO_1    ];		// 로드셀 RO     1의 자리 FND 2 에 RO값 표시

				if(SET.Select) {Interval_Set(Number);}		// 로드셀 RO 설정
			break;

			case 6:							// Count = 6 이면
				DIGIT[0][3] = FND_I;				// FND 0 에 IN-S 표시
				DIGIT[0][2] = FND_N;
				DIGIT[0][1] = FND_MINUS;
				DIGIT[0][0] = FND_S;

				DIGIT[1][3] = fnd_tbl[IN_S_10000];		// 입구 체중 10.000kg의 자리 FND 1 에 입구체중값 표시
				DIGIT[1][2] = fnd_tbl[IN_S_1000]+FND_Point;	// 입구 체중  1.000kg의 자리
				DIGIT[1][1] = fnd_tbl[IN_S_100];		// 입구 체중  0.100kg의 자리
				DIGIT[1][0] = fnd_tbl[IN_S_10];			// 입구 체중  0.010kg의 자리
				DIGIT[2][3] = fnd_tbl[IN_S_1];			// 입구 체중  0.001kg의 자리 FND 2 에 입구체중값 표시

				if(SET.Select) {IN_Scales_Set(Number);}		// 로드셀 입구체중 설정
			break;

			case 7:							// Count = 7 이면
				Count = 1;					// 반복을 위해 Count를 최저로 감소 시킴
			break;
		}
		wdt_reset();							// 워치독 리셋
	}
}

//=================================================================================================
void Loadcell_Set(void)								// 로드셀 설정 모드
{
	uint8_t Count = 1;							// 카운트 변수 초기화
	SET.Select = 0;								// Select 버튼 초기화

	FND_Value_Data(0, 2, 12);						// FND 0 ~ 2 FND_OFF
	_delay_ms(250);

	while(SET.Menu)								// 메뉴가 1 이면 반복
	{
		if(KEY.Buff)							// Key 입력이 있으면
		{
			if     (KEY.Key == 1 && Count < 4) {Count++; KEY.Key = 0;}	// 업  버튼 이면 Count 증가
			else if(KEY.Key == 3 && Count > 0) {Count--; KEY.Key = 0;}	// 다운버튼 이면 Count 감소
		}
		DIGIT[2][0] = fnd_tbl[Count];					// FND 2.0 에 Count 표시

		switch(Count)							// Count
		{
			case 0:							// Count = 0 이면
				Count = 3;					// 반복을 위해 Count를 최고로 증가 시킴
			break;

			case 1:							// Count = 1 이면
				DIGIT[0][3] = FND_c;				// FND 0 에 cell 표시
				DIGIT[0][2] = FND_e;
				DIGIT[0][1] = FND_l;
				DIGIT[0][0] = FND_l;

				DIGIT[1][0] = FND_1;				// FND 1 에 1번 로드셀 표시

				if(SET.Select) {Cell_Select(1);}		// 1번 로드셀 설정 선택
			break;

			case 2:							// Count = 2 이면
				DIGIT[0][3] = FND_c;				// FND 0 에 cell 표시
				DIGIT[0][2] = FND_e;
				DIGIT[0][1] = FND_l;
				DIGIT[0][0] = FND_l;

				DIGIT[1][0] = FND_2;				// FND 1 에 2번 로드셀 표시

				if(SET.Select) {Cell_Select(2);}		// 2번 로드셀 설정 선택
			break;

			case 3:							// Count = 3 이면
				DIGIT[0][3] = FND_c;				// FND 0 에 cell 표시
				DIGIT[0][2] = FND_e;
				DIGIT[0][1] = FND_l;
				DIGIT[0][0] = FND_l;

				DIGIT[1][0] = FND_3;				// FND 1 에 3번 로드셀 표시

				if(SET.Select) {Cell_Select(3);}		// 3번 로드셀 설정 선택
			break;

			case 4:							// Count = 4 이면
				Count = 1;					// 반복을 위해 Count를 최저로 감소 시킴
			break;
		}
		wdt_reset();							// 워치독 리셋
	}
}

//=================================================================================================
//		END OF "loadcell.c"
//=================================================================================================
