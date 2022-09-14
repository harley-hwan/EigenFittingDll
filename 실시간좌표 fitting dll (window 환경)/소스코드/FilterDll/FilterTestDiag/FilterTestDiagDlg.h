
// FilterTestDiagDlg.h : ��� ����
//

#pragma once

#include "FilterDll.h"

#define TIMER_RECV 100


// CFilterTestDiagDlg ��ȭ ����
class CFilterTestDiagDlg : public CDialogEx
{
// �����Դϴ�.
public:
	CFilterTestDiagDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FILTERTESTDIAG_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.


// �����Դϴ�.
protected:
	HICON m_hIcon;

	// ������ �޽��� �� �Լ�
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
