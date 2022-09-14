
// FilterTestDiagDlg.h : 헤더 파일
//

#pragma once

#include "FilterDll.h"

#define TIMER_RECV 100


// CFilterTestDiagDlg 대화 상자
class CFilterTestDiagDlg : public CDialogEx
{
// 생성입니다.
public:
	CFilterTestDiagDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FILTERTESTDIAG_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();	
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedOk();
	CListBox m_listInfo;
	afx_msg void OnBnClickedButton1Init();
	afx_msg void OnBnClickedButton2Send();

	int m_isendingdata;
	int m_isenddata_count;
	int m_isenddata_index;
	double m_Buffer[400][3];
	double m_BallSpeed, m_BallDirH, m_BallDirV;
	void AddInfo(CString strInfo);
};
