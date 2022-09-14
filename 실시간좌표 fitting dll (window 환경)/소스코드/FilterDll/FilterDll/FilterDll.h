#pragma once

#ifdef	FILTERDLL_EXPORTS

#define FILTERDLL_API __declspec(dllexport)

#else

#define FILTERDLL_API __declspec(dllimport)

#endif

#define MAX_DATA_COUNT2 1000


extern "C" {
	struct _3DPos
	{
		double x;//좌우
		double y;//거리
		double z;//높이

		//_3DPos()
		//{
		//	x = 0.0;
		//	y = 0.0;
		//	z = 0.0;
		//}
	};

	struct _FilterResult
	{
		int index;
		int total_index;
		int lowpass_index;
		_3DPos result_pos[MAX_DATA_COUNT2];

		//_FilterResult()
		//{
		//	index = 0;
		//	total_index = 0;
		//	lowpass_index = 0;
		//}
	};


	// 함수 호출 순서: 1. (샷 레디시) InitFitting(), 2. (샷 취득시, 데이터 끝날때까지) Filter_Result() & GetResultData()

	// 1. 좌표 데이터(GetResultData())는 Filter_Result() 함수 호출 시마다 호출
	// 2. Filter_Result() 리턴값이 0이면 _FilterResult 리턴값으로 받는 하나의 좌표 값을 받아서 처리함, index 값이 좌표 번호에 해당함
	// 3. Filter_Result() 리턴값이 1이면 Filter_Result() 호출 중단하고, 한꺼번에 처리된(polyfit) 데이터 입력을 위해 total_index까지 데이터 읽어서 처리함

	FILTERDLL_API void InitFitting();//변수 초기화, 샷 레디 시마다 호출하기
	FILTERDLL_API int Filter_Result(double Input_x, double Input_y, double Input_z, bool end_data, double ballspeed, double balldir_h, double balldir_v);//x:좌우, y: 거리, z: 높이 실시간 입력 좌표, end_data: 마지막 데이터이면 true, return 값이 1이면 fitting 완료, end_data true일 경우 return값도 1이 됨
	                                                                                                                                                     //ballspeed: 볼속도, balldir_h: 방향각도, balldir_v: 높이각도
	FILTERDLL_API _FilterResult GetResultData();
}