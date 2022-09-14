// FilterDll.cpp : DLL 응용 프로그램을 위해 내보낸 함수를 정의합니다.
//

//#include "stdafx.h"

// dllmain.cpp: DLL 응용 프로그램의 진입점을 정의합니다.
#include "stdafx.h"
#include "FilterLowPass.h"
#include "FilterDll.h"

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

_FilterResult result_data;

FILTERDLL_API void InitFitting()
{
	//변수 초기화
	MYFILTER.InitFitting();

	_FilterResult result_data_Init;

	result_data_Init.index = 0;
	result_data_Init.lowpass_index = 0;
	result_data_Init.total_index = 0;
	for (int i = 0; i < MAX_DATA_COUNT2; i++)
	{
		result_data_Init.result_pos[i].x = 0.0;
		result_data_Init.result_pos[i].y = 0.0;
		result_data_Init.result_pos[i].z = 0.0;
	}

	result_data = result_data_Init;
}

FILTERDLL_API int Filter_Result(double Input_x, double Input_y, double Input_z, bool end_data, double ballspeed, double balldir_h, double balldir_v)//x:좌우, y: 거리, z: 높이
{
	int end_check = 0;
	_struct_pos Raw_Input;
	Raw_Input.x = Input_x;
	Raw_Input.y = Input_y;
	Raw_Input.z = Input_z;
	
	end_check = MYFILTER.Filter_Result(Raw_Input, end_data, ballspeed, balldir_h, balldir_v);
	if (end_check != 1)//데이터가 1이면 끝으로 계산 종료
	{
		result_data.lowpass_index = MYFILTER.lowpass_endindex - 1;
		result_data.index = MYFILTER.input_index - 1;//좌표가 몇개인지 (배열 번호는 입력 index보다 하나 작게 할당) 
		result_data.total_index = result_data.index;
		result_data.result_pos[result_data.index].x = MYFILTER.result_pos[result_data.index].x;
		result_data.result_pos[result_data.index].y = MYFILTER.result_pos[result_data.index].y;
		result_data.result_pos[result_data.index].z = MYFILTER.result_pos[result_data.index].z;		
	}
	else//데이터 끝에 해당하는 처리
	{		//end_index	
		////2021-10-06-센서 데이터가 1개로 끝나는 경우 임시 처리
		//if (MYFILTER.input_index <= 1)
		//{
		//	result_data.total_index = 0;
		//	result_data.result_pos[0].x = 0.0;
		//	result_data.result_pos[0].y = 0.0;
		//	result_data.result_pos[0].z = 0.0;
		//}
		//else
		//{
			result_data.total_index = MYFILTER.input_index - 1;

			for (int i = result_data.lowpass_index + 1; i <= result_data.total_index; i++)
			{
				result_data.result_pos[i].x = MYFILTER.result_pos[i].x;
				result_data.result_pos[i].y = MYFILTER.result_pos[i].y;
				result_data.result_pos[i].z = MYFILTER.result_pos[i].z;
			}
		//}
	}

	return end_check;
}

// 1. Filter_Result() 함수 호출 시마다 호출
// 2. Filter_Result() 리턴값이 0이면 _FilterResult 리턴값은 한나의 좌표 값을 받아서 처리함, index 값이 좌표 번호에 해당함
// 3. Filter_Result() 리턴값이 1이면 Filter_Result() 호출 중단하고, 한꺼번에 처리된(polyfit) 데이터 입력을 위해 total_index까지 데이터 읽어서 처리함
FILTERDLL_API _FilterResult GetResultData()// Filter_Result() 함수 리턴값이 1이면 호출 중단하고, 한꺼번에 처리된 데이터 입력을 위해 total_index까지 데이터 읽어서 처리함
{
	return result_data;
}