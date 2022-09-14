// FilterDll.cpp : DLL ���� ���α׷��� ���� ������ �Լ��� �����մϴ�.
//

//#include "stdafx.h"

// dllmain.cpp: DLL ���� ���α׷��� �������� �����մϴ�.
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
	//���� �ʱ�ȭ
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

FILTERDLL_API int Filter_Result(double Input_x, double Input_y, double Input_z, bool end_data, double ballspeed, double balldir_h, double balldir_v)//x:�¿�, y: �Ÿ�, z: ����
{
	int end_check = 0;
	_struct_pos Raw_Input;
	Raw_Input.x = Input_x;
	Raw_Input.y = Input_y;
	Raw_Input.z = Input_z;
	
	end_check = MYFILTER.Filter_Result(Raw_Input, end_data, ballspeed, balldir_h, balldir_v);
	if (end_check != 1)//�����Ͱ� 1�̸� ������ ��� ����
	{
		result_data.lowpass_index = MYFILTER.lowpass_endindex - 1;
		result_data.index = MYFILTER.input_index - 1;//��ǥ�� ����� (�迭 ��ȣ�� �Է� index���� �ϳ� �۰� �Ҵ�) 
		result_data.total_index = result_data.index;
		result_data.result_pos[result_data.index].x = MYFILTER.result_pos[result_data.index].x;
		result_data.result_pos[result_data.index].y = MYFILTER.result_pos[result_data.index].y;
		result_data.result_pos[result_data.index].z = MYFILTER.result_pos[result_data.index].z;		
	}
	else//������ ���� �ش��ϴ� ó��
	{		//end_index	
		////2021-10-06-���� �����Ͱ� 1���� ������ ��� �ӽ� ó��
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

// 1. Filter_Result() �Լ� ȣ�� �ø��� ȣ��
// 2. Filter_Result() ���ϰ��� 0�̸� _FilterResult ���ϰ��� �ѳ��� ��ǥ ���� �޾Ƽ� ó����, index ���� ��ǥ ��ȣ�� �ش���
// 3. Filter_Result() ���ϰ��� 1�̸� Filter_Result() ȣ�� �ߴ��ϰ�, �Ѳ����� ó����(polyfit) ������ �Է��� ���� total_index���� ������ �о ó����
FILTERDLL_API _FilterResult GetResultData()// Filter_Result() �Լ� ���ϰ��� 1�̸� ȣ�� �ߴ��ϰ�, �Ѳ����� ó���� ������ �Է��� ���� total_index���� ������ �о ó����
{
	return result_data;
}