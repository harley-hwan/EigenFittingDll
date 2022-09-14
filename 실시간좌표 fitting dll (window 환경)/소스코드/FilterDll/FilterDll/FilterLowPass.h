#pragma once


#include <stdio.h>
#include <vector>

#define SAMPLING_FREQUENCY 1000 //평탄화를 위해 최대치(2000) 적용
#define M_PI 3.1415926536
#define LIMIT_TRACKING_DISTANCE 140.0//180.0
#define LIMIT_TRACKING_DISTANCE_FOR_XPOS 60.0//180.0
#define LIMIT_POLYFIT_DISTANCE 300.0
#define TIME_BALLDATA 0.032 //0.004
#define MAX_DATA_COUNT 1000
#define OFFSIDE_DIFF_COEFF 40
#define OFFSIDE_DIFF_COEFF_2STEP 80


//거리 좌표: z 를 기준으로 필터링
struct _struct_pos
{
	double x;//좌우
	double y;//거리//높이
	double z;//높이//거리

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
	* @brief 클래스 인스턴스 얻기
	* @return 클래스 인스턴스
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
	double coeff_z[4];//z좌표(높이)의 polyfit 다항계수coeff_y[4];//y좌표(높이)의 polyfit 다항계수
	double coeff_x[4];//x좌표(좌우)의 polyfit 다항계수

	double xpos_1step[MAX_DATA_COUNT];
	int input_index = 0;
	int end_index = 0;//실시간 데이터 중 마지막 데이터의 index 값 저장
	int lowpass_endindex = 0;//lowpass 마지막 수행 인덱스
	bool polyfit_start = false;
	double diff_zpos_apply = 0.0;//바로 전 좌표와 편차 줄이는 테스트_0731

	int index_polyfitstart_forxpos;
	double diff_xpos_apply;

	double m_LimitDistance_forPloyfit;
	double m_LimitDistance_forXPos;

	bool m_slope_startpolyfit;// 폴리핏 직전 구간이 오르막인 경우 true
	double m_launchangle;//탄도 계산

	//2021-08-26-타깃 지점의 지면 높낮이 정보 (Tee 박스 높이 기준으로 동일하면 0.0m)
	double m_GroundHeight;
	double m_WindEffect;//바람 영향

//2021-10-07-초기 로패스 데이터로 평균 탄도 계산해보기
	double tanval[10];
	double sumtanval = 0.0;
	double m_balldir_v;

	//2021-09-16-좌우 방향 데이터 보정
	bool m_boffside_diff;
	int index_offside_diff;
	bool m_boffside_diff_2step;
	int index_offside_diff_2step;
	bool m_offside_plus;//초기 편차 방향을 끝까지 유지해야 함, 

	double balldir_from10;// = 0.0;

	//2021-09-23-최고점 지점에서 lowpass 계산 높이 값_폴리핏 계산 높이 차만큼 거리 가감용
	double lowpass_hei_max;
	int lowpass_hei_max_index;

	void InitFitting();
	double LowPassFilter_Process(double Input, double pastInput, double pastOutput);
	double LowPassFilter_Process2(double Input, double pastInput, double pastOutput);
	double BallDataFilter_Process(double Input, double degree, double pastInput);// double pastInput, double pastOutput);
	int Filter_Result(_struct_pos Input, bool end_data, double ballspeed, double m_balldir_h, double m_balldir_v);//input: 실시간 입력 좌표 값, end_data: 마지막 좌표이면 true // 데이터가 끝이면 return값 1, 끝이 아니면 return값 0
	void polyfit(const std::vector<double> &xv, const std::vector<double> &yv, std::vector<double> &coeff, int order);
	double get_ypos(double input_y, double diff_y);
	void result_polyfit();
};

// CMyFilter 클래스 인스턴스 매크로
#define MYFILTER CMyFilter::Instance()