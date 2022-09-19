#pragma once


#include <cstdio>
#include <vector>

#define SAMPLING_FREQUENCY 1000 //��źȭ�� ���� �ִ�ġ(2000) ����
#define M_PI 3.1415926536
#define LIMIT_TRACKING_DISTANCE 140.0//180.0
#define LIMIT_TRACKING_DISTANCE_FOR_XPOS 60.0//180.0
#define LIMIT_POLYFIT_DISTANCE 300.0
#define TIME_BALLDATA 0.032 //0.004
#define MAX_DATA_COUNT 1000
#define OFFSIDE_DIFF_COEFF 40
#define OFFSIDE_DIFF_COEFF_2STEP 80


//�Ÿ� ��ǥ: z �� �������� ���͸�
struct _struct_pos
{
	double x;//�¿�
	double y;//�Ÿ�//����
	double z;//����//�Ÿ�

	_struct_pos()
	{
		x = 0.0;
		y = 0.0;
		z = 0.0;
	}
};

class CMyFilter
{
private:
	CMyFilter();
	~CMyFilter();

public:
	/*
	* @brief Ŭ���� �ν��Ͻ� ���
	* @return Ŭ���� �ν��Ͻ�
	*/
	static CMyFilter & Instance()
	{
		static CMyFilter Instance;
		return Instance;
	}

private:

public:
	_struct_pos Input_pos;
	_struct_pos past_Input_pos;
	_struct_pos past_Input_pos2;
	_struct_pos past_Output_pos;
	_struct_pos past_Output_pos2;
	_struct_pos result_pos[MAX_DATA_COUNT];
	_struct_pos input_pos[MAX_DATA_COUNT];
	double coeff_z[4];//z��ǥ(����)�� polyfit ���װ��coeff_y[4];//y��ǥ(����)�� polyfit ���װ��
	double coeff_x[4];//x��ǥ(�¿�)�� polyfit ���װ��

	double xpos_1step[MAX_DATA_COUNT];
	int input_index = 0;
	int end_index = 0;//�ǽð� ������ �� ������ �������� index �� ����
	int lowpass_endindex = 0;//lowpass ������ ���� �ε���
	bool polyfit_start = false;
	double diff_zpos_apply = 0.0;//�ٷ� �� ��ǥ�� ���� ���̴� �׽�Ʈ_0731

	int index_polyfitstart_forxpos;
	double diff_xpos_apply;

	double m_LimitDistance_forPloyfit;
	double m_LimitDistance_forXPos;

	bool m_slope_startpolyfit;// ������ ���� ������ �������� ��� true
	double m_launchangle;//ź�� ���

	//2021-08-26-Ÿ�� ������ ���� ������ ���� (Tee �ڽ� ���� �������� �����ϸ� 0.0m)
	double m_GroundHeight;
	double m_WindEffect;//�ٶ� ����

//2021-10-07-�ʱ� ���н� �����ͷ� ��� ź�� ����غ���
	double tanval[10];
	double sumtanval = 0.0;
	double m_balldir_v;

	//2021-09-16-�¿� ���� ������ ����
	bool m_boffside_diff;
	int index_offside_diff;
	bool m_boffside_diff_2step;
	int index_offside_diff_2step;
	bool m_offside_plus;//�ʱ� ���� ������ ������ �����ؾ� ��,

	double balldir_from10;// = 0.0;

	//2021-09-23-�ְ��� �������� lowpass ��� ���� ��_������ ��� ���� ����ŭ �Ÿ� ������
	double lowpass_hei_max;
	int lowpass_hei_max_index;

	void InitFitting();
	double LowPassFilter_Process(double Input, double pastInput, double pastOutput);
	double LowPassFilter_Process2(double Input, double pastInput, double pastOutput);
	double BallDataFilter_Process(double Input, double degree, double pastInput);// double pastInput, double pastOutput);
	int Filter_Result(_struct_pos Input, bool end_data, double ballspeed, double m_balldir_h, double m_balldir_v);//input: �ǽð� �Է� ��ǥ ��, end_data: ������ ��ǥ�̸� true // �����Ͱ� ���̸� return�� 1, ���� �ƴϸ� return�� 0
	void polyfit(const std::vector<double> &xv, const std::vector<double> &yv, std::vector<double> &coeff, int order);
	double get_ypos(double input_y, double diff_y);
	void result_polyfit();
};

// CMyFilter Ŭ���� �ν��Ͻ� ��ũ��
#define MYFILTER CMyFilter::Instance()