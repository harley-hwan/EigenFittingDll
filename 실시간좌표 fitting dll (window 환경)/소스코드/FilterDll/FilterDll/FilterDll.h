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
		double x;//�¿�
		double y;//�Ÿ�
		double z;//����

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


	// �Լ� ȣ�� ����: 1. (�� �����) InitFitting(), 2. (�� ����, ������ ����������) Filter_Result() & GetResultData()

	// 1. ��ǥ ������(GetResultData())�� Filter_Result() �Լ� ȣ�� �ø��� ȣ��
	// 2. Filter_Result() ���ϰ��� 0�̸� _FilterResult ���ϰ����� �޴� �ϳ��� ��ǥ ���� �޾Ƽ� ó����, index ���� ��ǥ ��ȣ�� �ش���
	// 3. Filter_Result() ���ϰ��� 1�̸� Filter_Result() ȣ�� �ߴ��ϰ�, �Ѳ����� ó����(polyfit) ������ �Է��� ���� total_index���� ������ �о ó����

	FILTERDLL_API void InitFitting();//���� �ʱ�ȭ, �� ���� �ø��� ȣ���ϱ�
	FILTERDLL_API int Filter_Result(double Input_x, double Input_y, double Input_z, bool end_data, double ballspeed, double balldir_h, double balldir_v);//x:�¿�, y: �Ÿ�, z: ���� �ǽð� �Է� ��ǥ, end_data: ������ �������̸� true, return ���� 1�̸� fitting �Ϸ�, end_data true�� ��� return���� 1�� ��
	                                                                                                                                                     //ballspeed: ���ӵ�, balldir_h: ���Ⱒ��, balldir_v: ���̰���
	FILTERDLL_API _FilterResult GetResultData();
}