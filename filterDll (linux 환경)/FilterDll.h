#pragma once

#define MAX_DATA_COUNT2 1000


extern "C" {
	struct _3DPos
	{
		double x;//좌우
		double y;//거리
		double z;//?�이

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


	// ?�수 ?�출 ?�서: 1. (???�디?? InitFitting(), 2. (??취득?? ?�이???�날?�까지) Filter_Result() & GetResultData()

	// 1. 좌표 ?�이??GetResultData())??Filter_Result() ?�수 ?�출 ?�마???�출
	// 2. Filter_Result() 리턴값이 0?�면 _FilterResult 리턴값으�?받는 ?�나??좌표 값을 받아??처리?? index 값이 좌표 번호???�당??	// 3. Filter_Result() 리턴값이 1?�면 Filter_Result() ?�출 중단?�고, ?�꺼번에 처리??polyfit) ?�이???�력???�해 total_index까�? ?�이???�어??처리??
	void InitFitting();//변??초기?? ???�디 ?�마???�출?�기
	int Filter_Result(double Input_x, double Input_y, double Input_z, bool end_data, double ballspeed, double balldir_h, double balldir_v);//x:좌우, y: 거리, z: ?�이 ?�시�??�력 좌표, end_data: 마�?�??�이?�이�?true, return 값이 1?�면 fitting ?�료, end_data true??경우 return값도 1????	                                                                                                                                                     //ballspeed: 볼속?? balldir_h: 방향각도, balldir_v: ?�이각도
	_FilterResult GetResultData();
}