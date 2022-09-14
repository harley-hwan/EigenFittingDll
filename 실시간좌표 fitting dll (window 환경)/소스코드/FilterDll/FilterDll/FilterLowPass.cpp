#include "stdafx.h"
#include "stdio.h"
#include "FilterLowPass.h"
#include <Eigen\QR>
#include <Eigen\Dense>
#include <cmath>

using namespace Eigen;

//데이터 정상 처리 확인을 위한 로그 파일(디버깅 용도)
FILE* fp_log1 = NULL;
FILE* fp_log2 = NULL;
FILE* fp_log3 = NULL;

//코스 별 조건 저장 파일_지면 높낮이와 바람 세기 정보
FILE* fp_mapinfo = NULL;//타깃 지점 지면 높이 정보 저장할 문서
FILE* fp_windinfo = NULL;//바람 영향 정보

CMyFilter::CMyFilter()
{
	fopen_s(&fp_log1, "functionlog.txt", "a+");	
	if (fp_log1 != NULL)
	{
		fprintf(fp_log1, "\n\nVersion: 210913-2");
		fclose(fp_log1);
	}
}

CMyFilter::~CMyFilter()
{
	if(fp_log1 != NULL)
		fclose(fp_log1);

	if (fp_log2 != NULL)
		fclose(fp_log2);

	if (fp_log3 != NULL)
		fclose(fp_log3);
}

void CMyFilter::InitFitting()
{
	_struct_pos Input_pos_Init;
	_struct_pos past_Input_pos_Init;
	_struct_pos past_Output_pos_Init;

	Input_pos = Input_pos_Init;
	past_Input_pos = past_Input_pos_Init;
	past_Output_pos = past_Output_pos_Init;
	for (int i = 0; i < MAX_DATA_COUNT; i++)
	{
		result_pos[i].x = 0.0;
		result_pos[i].y = 0.0;
		result_pos[i].z = 0.0;

		input_pos[i].x = 0.0;
		input_pos[i].y = 0.0;
		input_pos[i].z = 0.0;	
	}

	coeff_z[4] = { 0.0 };//z좌표(높이)의 polyfit 다항계수
	coeff_x[4] = { 0.0 };//x좌표(좌우)의 polyfit 다항계수

	xpos_1step[MAX_DATA_COUNT] = { 0.0 };
	input_index = 0;
	end_index = 0;//실시간 데이터 중 마지막 데이터의 index 값 저장
	lowpass_endindex = 0;
	polyfit_start = false;
	double diff_zpos_apply = 0.0;//바로 전 좌표와 편차 줄이는 테스트_0731
	
	m_LimitDistance_forPloyfit = LIMIT_TRACKING_DISTANCE;
	m_LimitDistance_forXPos = LIMIT_TRACKING_DISTANCE_FOR_XPOS;

	index_polyfitstart_forxpos = 0;
	diff_xpos_apply = 0.0;

	m_slope_startpolyfit = false;// 2021-08-10-폴리핏 직전 구간이 오르막인 경우 true	
	m_launchangle = 15.0;//탄도 계산

	//2021-09-16-좌우 방향 데이터 보정
	m_boffside_diff = false;
	index_offside_diff = 0;
    m_boffside_diff_2step = false;
	index_offside_diff_2step = 0;
	m_offside_plus = false;

	//2021-10-07-초기 로패스 데이터로 평균 탄도 계산해보기
	for(int i=0; i<10; i++)
		tanval[i] = 0.0;
	sumtanval = 0.0;

	SYSTEMTIME CurTime;
	GetLocalTime(&CurTime);
	
	fopen_s(&fp_log1, "functionlog.txt", "a+");//디버깅 용도
	if (fp_log1 != NULL)
	{
		fprintf(fp_log1, "\n%2d-%2d-%2d-%2d-%2d: InitFitting()", CurTime.wMonth, CurTime.wDay, CurTime.wHour, CurTime.wMinute, CurTime.wSecond);
		fclose(fp_log1);
	}

	//FILE* fp = NULL;
	//fopen_s(&fp, "settingdata.txt", "r");  //test파일을 r(읽기) 모드로 열기
	//double a, b;
	//int i = 0;
	//if (fp != NULL)
	//{
	//	fscanf_s(fp, "%lf,  %lf", &a, &b);
	//	m_LimitDistance_forPloyfit = a;
	//	m_LimitDistance_forXPos = b;
	//	fclose(fp); //파일 포인터 닫기
	//}

	//2021-08-26-타깃 지점의 지면 높낮이 정보 (Tee 박스 높이 기준으로 동일하면 0.0m)
	m_GroundHeight = 0.0;
	float fgrounddata = 0.0;
	fopen_s(&fp_mapinfo, "mapinfo.txt", "r");  //파일을 r(읽기) 모드로 열기
	if (NULL != fp_mapinfo)
	{
		fseek(fp_mapinfo, 0L, SEEK_SET);
		fscanf_s(fp_mapinfo, "%f", &fgrounddata);
		m_GroundHeight = (double)fgrounddata;
		fclose(fp_mapinfo); //파일 포인터 닫기
	}
	//바람 세기 정보 반영
	m_WindEffect = 1.0;
	float fwinddata = 1.0;
	fopen_s(&fp_windinfo, "windeffect.txt", "r");  //파일을 r(읽기) 모드로 열기
	if (NULL != fp_windinfo)
	{
		fseek(fp_windinfo, 0L, SEEK_SET);
		fscanf_s(fp_windinfo, "%f", &fwinddata);
		m_WindEffect = (double)fwinddata;
		fclose(fp_windinfo); //파일 포인터 닫기
	}
}

double CMyFilter::LowPassFilter_Process(double Input, double pastInput, double pastOutput)
{
	//input: 필터링 할 대상 데이터, pastInput: 바로 전 필터링 안한 데이터, pastOutput: 바로 전 필터링 결과 데이터
    //맨 처음 데이터는 input == pastInput == pastOutput 이 동일하게 계산함
	double a1, b0, b1, w0, output;
	w0 = 2 * M_PI * 30;// CUTOFF_FREQUENCY;
	a1 = (w0 - 2 * SAMPLING_FREQUENCY) / (2 * SAMPLING_FREQUENCY + w0);
	b0 = w0 / (2 * SAMPLING_FREQUENCY + w0);
	b1 = b0;

	output = b0*(Input) + b1*(pastInput) - a1*(pastOutput);

	return output;
}

double CMyFilter::LowPassFilter_Process2(double Input, double pastInput, double pastOutput)
{
	//input: 필터링 할 대상 데이터, pastInput: 바로 전 필터링 안한 데이터, pastOutput: 바로 전 필터링 결과 데이터
	//맨 처음 데이터는 input == pastInput == pastOutput 이 동일하게 계산함
	double a1, b0, b1, w0, output;
	w0 = 2 * M_PI * 30;// CUTOFF_FREQUENCY;
	a1 = (w0 - 3 * SAMPLING_FREQUENCY) / (3 * SAMPLING_FREQUENCY + w0);
	b0 = w0 / (3 * SAMPLING_FREQUENCY + w0);
	b1 = b0;

	output = b0*(Input)+b1*(pastInput)-a1*(pastOutput);

	return output;
}
//2021-09-16-초기 각도 활용한 fit
double CMyFilter::BallDataFilter_Process(double Input, double degree, double pastInput)
{
	double tanval, output;

	tanval = tan(degree * (M_PI / 180.0));
	output = Input * tanval;

	return output;
}

//실시간 볼 좌표 받아서 fitting 결과 좌표 생성 함수
int CMyFilter::Filter_Result(_struct_pos Input, bool end_data, double ballspeed, double m_balldir_h, double balldir_v)//input: 실시간 입력 좌표 값, end_data: 마지막 좌표이면 true, 속/탄/방(초기 측정된 볼속도, 높이각, 방햑각)
{
	//입력 좌표 로그 추가
	input_pos[input_index - 1].x = Input.x;
	input_pos[input_index - 1].y = Input.y;
	input_pos[input_index - 1].z = Input.z;
	if (end_data)
	{
		fopen_s(&fp_log3, "datalog_input.txt", "w");//실시간 입력 좌표 기록
		if (fp_log3 != NULL)
		{
			for (int i = 0; i<input_index; i++)
				fprintf(fp_log3, "%3.2f, %3.2f, %3.2f\n", input_pos[i].x, input_pos[i].y, input_pos[i].z);
			fclose(fp_log3);
		}
	}

	//2021-10-07-높이 데이터 1도 높이기
	m_balldir_v = balldir_v + 1.0;

	//높이 좌표 필터: y
	double xpos = 0.0;
	double zpos = 0.0;
	double ypos = 0.0;
	int end_check = 0;
	double  diff_ypos = 0.0;
	//거리 좌표값(y값) 입력값과 유사하게 가져가기-21.08.06
	double ydiff_start = 0.0;
	double ydiff_index = 0.0;
	ydiff_start = 0.02 * ballspeed + 0.5;

	//2021-09-30-프로들 낮은 탄도 보정
	double Direction_V_Cal = 0.0;
	Direction_V_Cal = -0.4 * m_balldir_v + 7.0;// -0.5 * m_balldir_v + 8.5;
	if (Direction_V_Cal < 1.0)
		Direction_V_Cal = 1.0;
	else if (Direction_V_Cal > 3.0)
		Direction_V_Cal = 3.0;
//2021-10-07-초기 탄도 변경-최대 높이 관련 계산 시 활용하던 탄도임
	m_launchangle = m_balldir_v + Direction_V_Cal;

	ypos = Input.y;
	if (input_index == 0)//첫번째 좌표값이면
	{
		zpos = LowPassFilter_Process(Input.z, Input.z, Input.z);
		past_Input_pos.z = Input.z;
		past_Output_pos.z = zpos;

		xpos = xpos_1step[input_index] = LowPassFilter_Process(Input.x, Input.x, Input.x);
		past_Input_pos.x = Input.x;
		past_Output_pos.x = xpos;
//테스트-거리값도 피팅해봄
		ypos = LowPassFilter_Process(Input.y, Input.y, Input.y);
		past_Input_pos.y = Input.y;
		past_Output_pos.y = ypos;

		//2021-09-23-150m 지점에서 lowpass 계산 높이 값_폴리핏 계산 높이 차만큼 거리 가감용
		lowpass_hei_max = 0.0;
		lowpass_hei_max_index = 0;
	}
	else
	{
		zpos = LowPassFilter_Process(Input.z, past_Input_pos.z, past_Output_pos.z);
		past_Input_pos.z = Input.z;
		past_Output_pos.z = zpos;

		xpos_1step[input_index] = LowPassFilter_Process(Input.x, past_Input_pos.x, past_Output_pos.x);
		past_Input_pos.x = Input.x;
		past_Output_pos.x = xpos_1step[input_index];

		//거리가 80m 미만인 경우에는 1단계까지만 계산한 결과 사용
		//거리가 80m 이상인 경우에는 1단계 계산 후 2단계 계산까지 한 결과 사용
		if (Input.y < m_LimitDistance_forXPos)//거리가 60m 이상인 경우 추가 튜닝
			xpos = xpos_1step[input_index];
		else
			xpos = (xpos_1step[input_index] - xpos_1step[input_index - 30]) / 30 + xpos_1step[input_index - 1];

		//테스트-거리값도 피팅해봄
		ypos = LowPassFilter_Process(Input.y, past_Input_pos.y, past_Output_pos.y);
		past_Input_pos.y = Input.y;
		past_Output_pos.y = ypos;

		//2021-09-23-150m 지점에서 lowpass 계산 높이 값_폴리핏 계산 높이 차만큼 거리 가감용
		if (lowpass_hei_max < zpos)
		{
			lowpass_hei_max = zpos;
			lowpass_hei_max_index = input_index;
		}
	}

	result_pos[input_index].z = zpos;
	result_pos[input_index].x = xpos;
	result_pos[input_index].y = ypos;

	//2021.08.06-2차 로패스 제거해봄
//2차 로패스_0730
    if (input_index == 0)//첫번째 좌표값이면
	{
		past_Input_pos2.z = result_pos[input_index].z;
		past_Output_pos2.z = result_pos[input_index].z;
		//2021-09-16-초기 각도 활용한 fit
		zpos = BallDataFilter_Process(ypos, m_balldir_v + Direction_V_Cal, result_pos[input_index].z);
		past_Input_pos2.z = result_pos[input_index].z;
		past_Output_pos2.z = zpos;

		xpos = LowPassFilter_Process2(result_pos[input_index].x, result_pos[input_index].x, result_pos[input_index].x);
		past_Input_pos2.x = result_pos[input_index].x;
		past_Input_pos2.y = result_pos[input_index].y;
	}
	else if (input_index <= 10)
	{
		ypos = LowPassFilter_Process(result_pos[input_index].y, past_Input_pos2.y, result_pos[input_index - 1].y);
		past_Input_pos2.y = result_pos[input_index].y;

		xpos = LowPassFilter_Process2(result_pos[input_index].x, past_Input_pos2.x, result_pos[input_index - 1].x);
		past_Input_pos2.x = result_pos[input_index].x;

		//2021-09-16-초기 각도 활용한 fit
		zpos = BallDataFilter_Process(ypos, m_balldir_v + Direction_V_Cal, past_Output_pos2.z);
		past_Input_pos2.z = result_pos[input_index].z;
		past_Output_pos2.z = zpos;

		balldir_from10 = 0.0;
		balldir_from10 = atan(xpos / ypos) * 180 / 3.14;
	}
	else
	{
		ypos = LowPassFilter_Process(result_pos[input_index].y, past_Input_pos2.y, result_pos[input_index - 1].y);
		past_Input_pos2.y = result_pos[input_index].y;

		//2021-09-16-초기 각도 활용한 fit
		zpos = BallDataFilter_Process(ypos, m_balldir_v + Direction_V_Cal, past_Output_pos2.z);
		past_Input_pos2.z = result_pos[input_index].z;
		past_Output_pos2.z = zpos;

		xpos = BallDataFilter_Process(ypos, balldir_from10, past_Output_pos2.x);
		past_Input_pos2.x = result_pos[input_index].x;
		past_Output_pos2.x = xpos;

	}
	result_pos[input_index].z = zpos;
	result_pos[input_index].x = xpos;
	result_pos[input_index].y = ypos;
//2차 로패스_0730

	int start_index = 0;
	if (end_data)
	{		
		end_index = input_index;//실시간 데이터 중 마지막 데이터의 index 값 저장
		start_index = end_index - 20;// 10;
		if (start_index < 0)
			start_index = 0;
		diff_ypos = (result_pos[end_index].y - result_pos[start_index].y) / (double)(end_index - start_index);
		end_check = 1;
	}

	//Polyfit 로직 수행해야 함
	if (!polyfit_start)
	{
		//2021-07-28-볼속도 별로 폴리 피팅 진행 구간 변경하기
		float limitdist = 1.6 * ballspeed;
		if (limitdist < 80.0)//120.0)
			limitdist = 80.0;// 120.0;
		else if (limitdist > 100.0)// 150)
			limitdist = 100.0;// 150.0;
		m_LimitDistance_forPloyfit = limitdist;

		if ((end_data || Input.y > m_LimitDistance_forPloyfit))//입력 데이터가 마지막이거나, 추적 거리가 Limit를 넘은 경우 polyfit 수행
		{
			lowpass_endindex = input_index;
			//polyfit 계수 계산 (1회만 계산하도록 함)
			polyfit_start = true;
			std::vector<double> x_values1, y_values1, coeff1;
			std::vector<double> x_values2, y_values2, coeff2;
    		//double x, y;

			//32ms 이후 좌표부터 입력되기 때문에 0번째에 강제로 (0,0,0) 넣어줌
			//y좌표에 대한 polyfit
			x_values1.push_back(0.0);
			y_values1.push_back(0.0);

			//z좌표에 대한 polyfit
			x_values2.push_back(0.0);
			y_values2.push_back(0.0);

			for (int i = 0; i <= input_index; i++)
			{
				//z좌표에 대한 polyfit
				x_values1.push_back(result_pos[i].y); //[i + 1].y);
				y_values1.push_back(result_pos[i].z); //[i + 1].z);
				
				//x좌표에 대한 polyfit
				x_values2.push_back(result_pos[i].y); //[i + 1].y);
				y_values2.push_back(result_pos[i].x); //[i + 1].x);
			}
//21-08-09-기존 폴리피팅에서 내리막 데이터인 경우는 가상 높이값과 거리값 계산하지 않음
			polyfit(x_values1, y_values1, coeff1, 3);//높이 좌표는 3차식 폴리피팅
			std::vector<double> x_values3, y_values3, coeff3;
			double pre_zpos[5] = { 0.0 };
			double pre_ypos[5] = { 0.0 };
			for (int i = 0; i < 5; i++)
			{
				pre_zpos[i] = result_pos[input_index - i].z;
				pre_ypos[i] = result_pos[input_index - i].y;
				x_values3.push_back(pre_ypos[i]);
				y_values3.push_back(pre_zpos[i]);
			}
			polyfit(x_values3, y_values3, coeff3, 1);//높이 좌표는 3차식 폴리피팅	
			if (coeff3[1] > 0.0)//오르막 구간이면
			{
	//2021-08-02-예상 제한 거리에 높이값 0으로 데이터 넣어서 테스트 해보기_start
				//제네시스 경기
				double limit_dist_cal_by_height = 0.01 * m_balldir_v + 0.88;
				if (limit_dist_cal_by_height > 1.0)
					limit_dist_cal_by_height = 1.0;
				else if (limit_dist_cal_by_height < 0.9)
					limit_dist_cal_by_height = 0.9;
				double limit_dist_forpolyfit = -0.0571 * ballspeed * ballspeed + 11.6* ballspeed - 296.57; 
				limit_dist_forpolyfit *= limit_dist_cal_by_height;
				limit_dist_forpolyfit *= m_WindEffect;

    //2021-09-23-폴리핏 하기 전의 좌표 중(80m 남짓) 최고점 지점에서 lowpass 계산 높이 값_폴리핏 계산 높이 차만큼 거리 가감용
				if ((lowpass_hei_max - result_pos[lowpass_hei_max_index].z) > 0.0)
				{
					double diff_max_hei = (lowpass_hei_max - result_pos[lowpass_hei_max_index].z) * 2.0 - 2.0; // 1m->0, 3m->4, 6m->10, 8m->14
					if (diff_max_hei > 15.0)
						diff_max_hei = 15.0;
					limit_dist_forpolyfit -= diff_max_hei;
				}
				x_values1.push_back(limit_dist_forpolyfit); 
				y_values1.push_back(0.0); 
	//2021-08-02-예상 제한 거리에 높이값 0으로 데이터 넣어서 테스트 해보기_end

	//2021-08-06-예상 높이 값 데이터 넣어서 테스트해보기_start
				double limit_dist_forpolyfitzpos = 3.0 * ballspeed - 60.0;
				double limit_height_forpolyfitzpos = ballspeed - 35.0;// 30.0;
				//2021-08-10-탄도각으로 최고 높이 보정(폴리핏에서)
				double ratio = 1.0;
				if (15.0 < m_launchangle)//15도보다 높으면
					ratio = 1.5;
				if(m_launchangle > 15.0)
					limit_height_forpolyfitzpos -= (15.0 - m_launchangle) * 2.0;// ratio;//탄도 계산
				else
				{
					double lowheight = 0.5 * m_launchangle * m_launchangle - 16.5 * m_launchangle + 135.0;
					if (lowheight > 20)
						lowheight = 20.0;
					limit_height_forpolyfitzpos -= lowheight;
				}
				limit_dist_forpolyfitzpos -= (15.0 - m_launchangle) * 2.0;//탄도 계산

				x_values1.push_back(limit_dist_forpolyfitzpos); 
				y_values1.push_back(limit_height_forpolyfitzpos); 
	//2021-08-06-예상 높이 값 데이터 넣어서 테스트해보기_end
				m_slope_startpolyfit = true;// 2021-08-10-폴리핏 직전 구간이 오르막인 경우 true
			}

			polyfit(x_values1, y_values1, coeff1, 3);//높이 좌표는 3차식 폴리피팅
			polyfit(x_values2, y_values2, coeff2, 2);// 3);//2021-08-03-좌우 좌표는 2차식 폴리피팅이 더 적합하여 수정함

			for (int i = 0; i < 4; i++)
			{
				coeff_z[i] = coeff1[i];//y좌표(높이)의 polyfit 다항계수
				if(i<3)//x는 2차식으로 테스트
					coeff_x[i] = coeff2[i];//x좌표(좌우)의 polyfit 다항계수
			}
////2021-08-02-좌우 굴곡 줄이기 테스트_start
//			coeff_x[3] *= 0.8;
////2021-08-02-좌우 굴곡 줄이기 테스트_start

			SYSTEMTIME CurTime;
			GetLocalTime(&CurTime);

			fopen_s(&fp_log1, "functionlog.txt", "a+");//디버깅 용도
			if (fp_log1 != NULL)
			{
				fprintf(fp_log1, "\n%2d-%2d-%2d-%2d-%2d: polyfit()", CurTime.wMonth, CurTime.wDay, CurTime.wHour, CurTime.wMinute, CurTime.wSecond);
				fprintf(fp_log1, "\n%2d-%2d-%2d-%2d-%2d: polyfit condition - end data: %1d, Input.y: %3.1f, ballspeed: %3.1f, dir_h: %2.1f, dir_v: %2.1f, limitdist: %3.1f"
					, CurTime.wMonth, CurTime.wDay, CurTime.wHour, CurTime.wMinute, CurTime.wSecond, end_data, Input.y, ballspeed, m_balldir_h, m_balldir_v, m_LimitDistance_forPloyfit);
				fclose(fp_log1);
			}
		}
		if (end_data)//나중에 end_data가 짧으면 폴리피팅 진행하지 않도록 수정해야 함
		{
			double x_val;
			//2021-08-26-타깃 지점 높낮이가 티박스보다 낮거나 높은 경우 아래 조건 중 z좌표 비교 기준을 0.0(평지)에서 변경(m_GroundHeight)
			while (result_pos[input_index].z > m_GroundHeight && result_pos[input_index].y < LIMIT_POLYFIT_DISTANCE)//z 값이 0보다 작아지면 빠져나오는 조건을 넣어야 함
			{
				input_index++;
				x_val = result_pos[input_index].y = get_ypos(result_pos[input_index - 1].y, diff_ypos);
				result_pos[input_index].z = coeff_z[0] + coeff_z[1] * x_val + coeff_z[2] * x_val * x_val + coeff_z[3] * x_val * x_val * x_val;
				result_pos[input_index].x = coeff_x[0] + coeff_x[1] * x_val + coeff_x[2] * x_val * x_val;// +coeff_x[3] * x_val * x_val * x_val;
			}
			SYSTEMTIME CurTime;
			GetLocalTime(&CurTime);

			fopen_s(&fp_log1, "functionlog.txt", "a+");//디버깅 용도
			if (fp_log1 != NULL)
			{
				fprintf(fp_log1, "\n%2d-%2d-%2d-%2d-%2d: data end - Index status", CurTime.wMonth, CurTime.wDay, CurTime.wHour, CurTime.wMinute, CurTime.wSecond);
				fprintf(fp_log1, "\nlowpass_index: %3d, input_index: %3d, total_index: %3d", lowpass_endindex, end_index, input_index);
				fclose(fp_log1);
			}

			fopen_s(&fp_log2, "datalog.txt", "w");//fitting 결과 데이터 기록
			if (fp_log2 != NULL)
			{
				for(int i = 0; i<input_index; i++)
					fprintf(fp_log2, "%3.2f, %3.2f, %3.2f\n", result_pos[i].x, result_pos[i].y, result_pos[i].z);
				fclose(fp_log2);
			}
		}
		else
		{
			//2021-09-24
			//2021-09-16-좌우 방향각 보정 처리-lowpass가 짧아서 이후 fade나 draw 못잡음에 대한 보정
			double diff_xpos_result = xpos_1step[input_index] - result_pos[input_index].x;
			if (!m_boffside_diff && (diff_xpos_result < -3.0 || diff_xpos_result > 3.0))
			{
				m_boffside_diff = true;
				index_offside_diff = input_index;
				if (diff_xpos_result > 0.0)
					m_offside_plus = true;
				else
					m_offside_plus = false;
			}
			if (m_boffside_diff)
			{
				double cosval = 0.0;
				cosval = 1.0 - cos(((result_pos[input_index].y - result_pos[index_offside_diff].y) / 3.0) * 3.14 / 180);
				if (!m_offside_plus)// || diff_xpos_result > 3.0)//low pass와 poly 간 차이가 3m 이상인 경우
				{
					result_pos[input_index].x -= OFFSIDE_DIFF_COEFF * cosval;
				}
				else if (m_offside_plus)
				{
					result_pos[input_index].x += OFFSIDE_DIFF_COEFF * cosval;
				}
				//5m 이상 차이일 경우 2차 보정
				if (!m_boffside_diff_2step && (diff_xpos_result < -10.0 || diff_xpos_result > 10.0))
				{
					m_boffside_diff_2step = true;
					index_offside_diff_2step = input_index;
				}

				double cosval_2step = 0.0;
				cosval_2step = 1.0 - cos(((result_pos[input_index].y - result_pos[index_offside_diff_2step].y) / 3.0) * 3.14 / 180);
				if (m_boffside_diff_2step)
				{
					if (!m_offside_plus)// || diff_xpos_result > 3.0)//low pass와 poly 간 차이가 3m 이상인 경우
					{
						result_pos[input_index].x -= OFFSIDE_DIFF_COEFF_2STEP * cosval_2step;
					}
					else if (m_offside_plus)
					{
						result_pos[input_index].x += OFFSIDE_DIFF_COEFF_2STEP * cosval_2step;
					}
				}
			}
			//2021-09-24-추가 end////////////////
			//기존 코드
			input_index++;
		}
	}
	else
	{
		//polyfit 계산으로 다시 세팅함_위에서 로패스피팅 한 것에 반복으로 제거함
		double x_val;
		x_val = result_pos[input_index].y = ypos;	

		result_pos[input_index].z = coeff_z[0] + coeff_z[1] * x_val + coeff_z[2] * x_val * x_val + coeff_z[3] * x_val * x_val * x_val;
//2021-08-09-x좌표는 폴리핏에서 바로 전 좌표 평균 값 이용으로 변경 테스트하기
		result_pos[input_index].x = coeff_x[0] + coeff_x[1] * x_val + coeff_x[2] * x_val * x_val;

		//2021-09-16-좌우 방향각 보정 처리-lowpass가 짧아서 이후 fade나 draw 못잡음에 대한 보정
		double diff_xpos_result = xpos_1step[input_index] - result_pos[input_index].x;
		if (!m_boffside_diff && (diff_xpos_result < -3.0 || diff_xpos_result > 3.0) && fabs(coeff_x[2]) < 0.00025)
		{
			m_boffside_diff = true;
			index_offside_diff = input_index;
			if (diff_xpos_result > 0.0)
				m_offside_plus = true;
			else
				m_offside_plus = false;
		}
		if (m_boffside_diff)
		{
			double cosval = 0.0;
			cosval = 1.0 - cos(((result_pos[input_index].y - result_pos[index_offside_diff].y) / 3.0) * 3.14 / 180);
			if (!m_offside_plus)// || diff_xpos_result > 3.0)//low pass와 poly 간 차이가 3m 이상인 경우
			{
				result_pos[input_index].x -= OFFSIDE_DIFF_COEFF * cosval;
			}
			else if (m_offside_plus)
			{
				result_pos[input_index].x += OFFSIDE_DIFF_COEFF * cosval;
			}
			//5m 이상 차이일 경우 2차 보정
			if (!m_boffside_diff_2step && (diff_xpos_result < -10.0 || diff_xpos_result > 10.0))
			{
				m_boffside_diff_2step = true;
				index_offside_diff_2step = input_index;
			}

			double cosval_2step = 0.0;
			cosval_2step = 1.0 - cos(((result_pos[input_index].y - result_pos[index_offside_diff_2step].y) / 3.0) * 3.14 / 180);
			if (m_boffside_diff_2step)
			{
				if (!m_offside_plus)// || diff_xpos_result > 3.0)//low pass와 poly 간 차이가 3m 이상인 경우
				{
					result_pos[input_index].x -= OFFSIDE_DIFF_COEFF_2STEP * cosval_2step;
				}
				else if (m_offside_plus)
				{
					result_pos[input_index].x += OFFSIDE_DIFF_COEFF_2STEP * cosval_2step;
				}
			}
		}

		if (input_index == lowpass_endindex + 1)
		{
			diff_xpos_apply = (result_pos[input_index].x - result_pos[input_index - 1].x) - (result_pos[input_index - 1].x - result_pos[input_index - 2].x);
		}
		result_pos[input_index].x -= diff_xpos_apply;

		
		//바로 전 좌표와 편차 줄이는 테스트_0731	
		double avgdiff_zpos_fromLowpassdata = 0.0;
		double diff_zpos_fromLowpassdata = 0.0;

		if (input_index == lowpass_endindex + 1)
		{
			if (m_slope_startpolyfit)// 2021-08-10-폴리핏 직전 구간이 오르막인 경우 true)
			{
				if (result_pos[input_index].z < result_pos[lowpass_endindex].z)
				{
					double zpos_re;
					zpos_re = (result_pos[lowpass_endindex].z - result_pos[lowpass_endindex - 1].z) + result_pos[lowpass_endindex].z;
					diff_zpos_apply = result_pos[input_index].z - zpos_re;						
				}
				else
				{
					double zpos_re;
					zpos_re = (result_pos[lowpass_endindex].z - result_pos[lowpass_endindex - 1].z) + result_pos[lowpass_endindex].z;
					diff_zpos_apply = result_pos[input_index].z - zpos_re;
				}
			}
			else
			{
				if (result_pos[input_index].z < result_pos[lowpass_endindex].z)
				{
					double zpos_re;
					zpos_re = (result_pos[lowpass_endindex].z - result_pos[lowpass_endindex - 1].z) + result_pos[lowpass_endindex].z;
					diff_zpos_apply = result_pos[input_index].z - zpos_re;
				}
				else
				{
					double zpos_re;
					zpos_re = (result_pos[lowpass_endindex].z - result_pos[lowpass_endindex - 1].z) + result_pos[lowpass_endindex].z;
					diff_zpos_apply = result_pos[input_index].z - zpos_re;
				}
			}
		}
		result_pos[input_index].z -= diff_zpos_apply;

		//2021-08-26-지면 높낮이에 따라 limit 거리 조정 필요-내리막이면 길어지고, 오르막이면 짧아지도록 함-기존에는 평지 기준으로 계산된 limit 거리임
		double limit_dist = ballspeed * 4.0 - 30.0;
		double limit_dist_recal = -0.3 * m_GroundHeight;
		limit_dist += limit_dist_recal;

		if (end_data)
		{	
			//2021-08-26-타깃 지점 높낮이가 티박스보다 낮거나 높은 경우 아래 조건 중 z좌표 비교 기준을 0.0(평지)에서 변경(m_GroundHeight)
			while (result_pos[input_index].z > m_GroundHeight && result_pos[input_index].y < LIMIT_POLYFIT_DISTANCE)//z 값이 0보다 작아지면 빠져나오는 조건을 넣어야 함, 거리가 300m 이상이면 빠져나옴
			{
				input_index++;
				x_val = result_pos[input_index].y = get_ypos(result_pos[input_index - 1].y, diff_ypos);
				result_pos[input_index].z = coeff_z[0] + coeff_z[1] * x_val + coeff_z[2] * x_val * x_val + coeff_z[3] * x_val * x_val * x_val;
				result_pos[input_index].x = coeff_x[0] + coeff_x[1] * x_val + coeff_x[2] * x_val * x_val;// +coeff_x[3] * x_val * x_val * x_val;
				//바로 전 좌표와 편차 줄이는 테스트_0731
				result_pos[input_index].z -= diff_zpos_apply;

				if (input_index == index_polyfitstart_forxpos + 1)
				{
					diff_xpos_apply = result_pos[input_index].x - result_pos[input_index - 1].x;
				}
				result_pos[input_index].x -= diff_xpos_apply;

				//2021-09-16-좌우 방향각 보정 처리-lowpass가 짧아서 이후 fade나 draw 못잡음에 대한 보정
				if (m_boffside_diff)
				{
					double cosval = 0.0;
					cosval = 1.0 - cos(((result_pos[input_index].y - result_pos[index_offside_diff].y) / 3.0) * 3.14 / 180);
					if (!m_offside_plus)// || diff_xpos_result > 3.0)//low pass와 poly 간 차이가 3m 이상인 경우
					{
						result_pos[input_index].x -= OFFSIDE_DIFF_COEFF * cosval;
					}
					else if (m_offside_plus)
					{
						result_pos[input_index].x += OFFSIDE_DIFF_COEFF * cosval;
					}
					//5m 차이인 경우 추가 보정
					if (m_boffside_diff_2step)
					{
						double cosval_2step = 0.0;
						cosval_2step = 1.0 - cos(((result_pos[input_index].y - result_pos[index_offside_diff_2step].y) / 3.0) * 3.14 / 180);
						if (!m_offside_plus)// || diff_xpos_result > 3.0)//low pass와 poly 간 차이가 3m 이상인 경우
						{
							result_pos[input_index].x -= OFFSIDE_DIFF_COEFF_2STEP * cosval_2step;
						}
						else if (m_offside_plus)
						{
							result_pos[input_index].x += OFFSIDE_DIFF_COEFF_2STEP * cosval_2step;
						}
					}
				}
			}

			double diff_limitdist = 0.0;
			if (limit_dist < result_pos[input_index].y)
				diff_limitdist = result_pos[input_index].y - limit_dist;
			else
				diff_limitdist = 0.0;
			

			int limit_index = 0;
			if (diff_limitdist > 0.0)//폴리핏 계산 거리가 속도 기준 제한 거리보다 많은 경우 재처리
			{
				for (int i = input_index; i > 0; i--)
				{
					if (result_pos[i].y < limit_dist)
					{
						limit_index = i;
						i = 0;
					}
				}
				int diff_index = limit_index - end_index;// lowpass_endindex;
				double diff_zpos_tozero = 0.0;//1차는 평지 기준(0.0)과의 차이, 2차는 티박스 대비 높낮이 반영 (diff_zpos_tozero + (- m_GroundHeight)) 으로 적용해서 계산하기

				if (result_pos[limit_index].z > m_GroundHeight)//0.0)
				{
					diff_zpos_tozero = result_pos[limit_index - 1].z + (-m_GroundHeight);
					double diff_zpos = 0.0;
					double degreepern = (90.0 / (double)diff_index) * (M_PI / 180.0);

					for (int i = end_index; i <= limit_index; i++)
					{
						diff_zpos = diff_zpos_tozero * (1.0 - cos(degreepern * (double)(i - end_index + 1)));
						result_pos[i].z = result_pos[i].z - diff_zpos;
					}
				}

				for (int i = limit_index + 1; i < MAX_DATA_COUNT; i++)
				{
					result_pos[i].x = result_pos[i].y = result_pos[i].z = 0.0;
					input_index = limit_index;
				}
			}
			SYSTEMTIME CurTime;
			GetLocalTime(&CurTime);

			fopen_s(&fp_log1, "functionlog.txt", "a+");//디버깅 용도
			if (fp_log1 != NULL)
			{
				fprintf(fp_log1, "\n%2d-%2d-%2d-%2d-%2d: data end - Index status", CurTime.wMonth, CurTime.wDay, CurTime.wHour, CurTime.wMinute, CurTime.wSecond);
				fprintf(fp_log1, "\nlowpass_index: %3d, input_index: %3d, total_index: %3d", lowpass_endindex, end_index, input_index-1);
				fclose(fp_log1);
			}
			fopen_s(&fp_log2, "datalog.txt", "w");//fitting 결과 데이터 기록
			if (fp_log2 != NULL)
			{
				for (int i = 0; i<input_index; i++)
					fprintf(fp_log2, "%3.2f, %3.2f, %3.2f\n", result_pos[i].x, result_pos[i].y, result_pos[i].z);
				fclose(fp_log2);
			}
		}
		else
			input_index++;
	}	

	return end_check;
}

//polyfit 조건: 거리가 180m 이상이거나, 데이터가 끝나는 조건 둘 중 하나에 해당하면 polyfit 호출
//첫번째 좌표부터 마지막 좌표까지 데이터로 polyfit
//z(거리좌표)값과 y(높이좌표)값으로 polyfit 진행
//z(거리좌표)값과 x(좌우좌표)값으로 polyfit 진행
void CMyFilter::result_polyfit()
{

}

void CMyFilter::polyfit(const std::vector<double> &xv, const std::vector<double> &yv, std::vector<double> &coeff, int order)
{
	Eigen::MatrixXd A(xv.size(), order + 1);
	Eigen::VectorXd yv_mapped = Eigen::VectorXd::Map(&yv.front(), yv.size());
	Eigen::VectorXd result;

	assert(xv.size() == yv.size());
	assert(xv.size() >= order + 1);

	// create matrix
	for (size_t i = 0; i < xv.size(); i++)
		for (size_t j = 0; j < order + 1; j++)
			A(i, j) = pow(xv.at(i), j);

	// solve for linear least squares fit
	result = A.householderQr().solve(yv_mapped);

	coeff.resize(order + 1);
	for (size_t i = 0; i < order + 1; i++)
		coeff[i] = result[i];
}

//측정된 데이터가 끝나서 더 이상 좌표가 없는 경우 다항식에서 x값에 해당하는 y좌표 계산은 이전 좌표까지로 계산된 평균 차 값으로 증가시킴
double CMyFilter::get_ypos(double input_y, double diff_y)
{
	double output_y = input_y + diff_y;

	return output_y;
}
