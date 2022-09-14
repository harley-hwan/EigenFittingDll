
// FilterTestDiagDlg.cpp : ���� ����
//

#include "stdafx.h"
#include "FilterTestDiag.h"
#include "FilterTestDiagDlg.h"
#include "afxdialogex.h"
#include "stdio.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ���� ���α׷� ������ ���Ǵ� CAboutDlg ��ȭ �����Դϴ�.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

// �����Դϴ�.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CFilterTestDiagDlg ��ȭ ����

CFilterTestDiagDlg::CFilterTestDiagDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_FILTERTESTDIAG_DIALOG, pParent)
	,m_BallSpeed(70.0)
	,m_BallDirV(10.0)
	,m_BallDirH(0.0)
{	
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFilterTestDiagDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_LIST1_STATUS, m_listInfo);//m_BallSpeed
	DDX_Text(pDX, IDC_EDIT_BALLSPEED, m_BallSpeed);
	DDX_Text(pDX, IDC_EDIT_DIR_V, m_BallDirV);
	DDX_Text(pDX, IDC_EDIT_DIR_H, m_BallDirH);
}

BEGIN_MESSAGE_MAP(CFilterTestDiagDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDOK, &CFilterTestDiagDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON1_INIT, &CFilterTestDiagDlg::OnBnClickedButton1Init)
	ON_BN_CLICKED(IDC_BUTTON2_SEND, &CFilterTestDiagDlg::OnBnClickedButton2Send)
END_MESSAGE_MAP()


// CFilterTestDiagDlg �޽��� ó����

BOOL CFilterTestDiagDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// �ý��� �޴��� "����..." �޴� �׸��� �߰��մϴ�.

	// IDM_ABOUTBOX�� �ý��� ��� ������ �־�� �մϴ�.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// �� ��ȭ ������ �������� �����մϴ�.  ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	//  �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

	// TODO: ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	m_isendingdata = 0;

//	float buffer[400][3];
//	//�׽�Ʈ
//	FILE* fp = NULL;
//	fopen_s(&fp, "filter.txt", "r");  //test������ r(�б�) ���� ����
//
//	for (int j = 0; j < 400; j++)
//	{
//		buffer[j][0] = 0.0;
//		buffer[j][1] = 0.0;
//		buffer[j][2] = 0.0;
//	}
//	char a, b, c;
//	a = b = c = ' ';
//	float e;
//	int i = 0;
//	m_isenddata_count = 0;
//	m_isenddata_index = 0;
//	if (fp != NULL)
//	{
//		//	fseek(fp, 0L, SEEK_SET);
//		while (0 < fscanf_s(fp, "%c", &a, sizeof(a)))
//		{
////			if (a == 'x' && b == '"' && c == ':')
//			if (a == 'x')
//			{
//				fscanf_s(fp, "%c", &b, sizeof(b));
//				if (b == '"')
//				{
//					fscanf_s(fp, "%c", &c, sizeof(c));
//					if (c == ':')
//					{
//						fscanf_s(fp, "%f", &e);
//						buffer[i][0] = e;
//					}
//				}
//				
//			}
//			if (a == 'y')
//			{
//				fscanf_s(fp, "%c", &b, sizeof(b));
//				if (b == '"')
//				{
//					fscanf_s(fp, "%c", &c, sizeof(c));
//					if (c == ':')
//					{
//						fscanf_s(fp, "%f", &e);
//						buffer[i][1] = e;
//					}
//				}
//
//			}
//			if (a == 'z')
//			{
//				fscanf_s(fp, "%c", &b, sizeof(b));
//				if (b == '"')
//				{
//					fscanf_s(fp, "%c", &c, sizeof(c));
//					if (c == ':')
//					{
//						fscanf_s(fp, "%f", &e);
//						buffer[i][2] = e;
//						i++;
//					}
//				}
//			}
//		
//		}
//		float test1 = buffer[1][1];
//		float test2 = buffer[1][2];
//
//		FILE* fp2 = NULL;
//		fopen_s(&fp2, "filter2.txt", "w");  //test������ r(�б�) ���� ����
//		if (fp2 != NULL)
//		{
//			//				
//			for (int i = 0; i <= 400; i++)
//			{
//				fprintf(fp2, "\n%3.2f, %3.2f, %3.2f", buffer[i][0], buffer[i][1], buffer[i][2]);
//			}
//			fclose(fp2);
//		}

	//	m_isenddata_count = i;
	//	//fread(buffer, sizeof(double), 1000, fp); //��ü �б�
	//	fclose(fp); //���� ������ �ݱ�
	//}

	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
}

void CFilterTestDiagDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸�����
//  �Ʒ� �ڵ尡 �ʿ��մϴ�.  ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
//  �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CFilterTestDiagDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ�Դϴ�.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �������� �׸��ϴ�.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�.
HCURSOR CFilterTestDiagDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CFilterTestDiagDlg::OnBnClickedOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_isendingdata = 0;
	KillTimer(TIMER_RECV);

	CDialogEx::OnOK();
}

FILE* fp_w = NULL;
void CFilterTestDiagDlg::OnTimer(UINT_PTR nIDEvent)
{
	int check_result = 0;
	if (TIMER_RECV == nIDEvent)
	{
//		double ballspeed = 74.5;
		if (m_isenddata_index == m_isenddata_count - 1)//�ǽð� ������ �� ������ �������̸� �Ʒ� �κ� ȣ��(��ǥ�μ��� ������ �μ��� true)
		{
			check_result = Filter_Result(m_Buffer[m_isenddata_index][0], m_Buffer[m_isenddata_index][1], m_Buffer[m_isenddata_index][2], true, m_BallSpeed, m_BallDirH, m_BallDirV);

			if (check_result == 1)//������ ���� ��
			{
				KillTimer(TIMER_RECV);
				m_isendingdata = 0;

				//�ǽð����� ���� �׽�Ʈ
				CString strMsg;
				_FilterResult resultdata = GetResultData();//������ �о����
				for (int i = resultdata.index+1; i <= resultdata.total_index; i++)
				{
					strMsg.Format(_T("Pos %3d, %3.2f, %3.2f, %3.2f"), i, resultdata.result_pos[i].x, resultdata.result_pos[i].y, resultdata.result_pos[i].z);
					AddInfo(strMsg);
				}
				
				strMsg.Format(_T("Finish to send data"));
				AddInfo(strMsg);

				
				//fopen_s(&fp_w, "rawdata_result.txt", "w"); 
				if (fp_w != NULL)
				{
	//				
					for (int i = resultdata.index+1; i <= resultdata.total_index; i++)
					{
						fprintf(fp_w, "\n%3.2f, %3.2f, %3.2f", resultdata.result_pos[i].x, resultdata.result_pos[i].y, resultdata.result_pos[i].z);
					}
					fprintf(fp_w, "\nindex: %d, total: %d, lowpass: %d", resultdata.index, resultdata.total_index, resultdata.lowpass_index);
					fclose(fp_w);
				}
			}
		}
		else//�ǽð� ������ �� ������ �����Ͱ� �ƴ� ��� �Ʒ� �κ� ȣ�� (��ǥ�� �μ��� ������ �μ��� false)
		{
			check_result = Filter_Result(m_Buffer[m_isenddata_index][0], m_Buffer[m_isenddata_index][1], m_Buffer[m_isenddata_index][2], false, m_BallSpeed, m_BallDirH, m_BallDirV);
			//if (check_result == 1)//������ ���� ��
			//{
			//	KillTimer(TIMER_RECV);
			//	m_isendingdata = 0;

			//	_FilterResult resultdata = GetResultData();
			//}
			m_isenddata_index++;

			//�ǽð����� ���� �׽�Ʈ
			_FilterResult resultdata_current = GetResultData();//������ �о����
			CString strMsg2;
			int idx = resultdata_current.index;
			strMsg2.Format(_T("Pos %3d, %3.2f, %3.2f, %3.2f"), idx, resultdata_current.result_pos[idx].x, resultdata_current.result_pos[idx].y, resultdata_current.result_pos[idx].z);
			AddInfo(strMsg2);

			if (fp_w != NULL)
			{
				fprintf(fp_w, "\n%3.2f, %3.2f, %3.2f", resultdata_current.result_pos[idx].x, resultdata_current.result_pos[idx].y, resultdata_current.result_pos[idx].z);
			}
		}

//		check_result = Filter_Result(double Input_x, double Input_y, double Input_z, bool end_data);
		//if (check_result == 1)//������ ���� ��
		//{
		//	KillTimer(TIMER_RECV);
		//	m_isendingdata = 0;
		//}
	}

	CDialog::OnTimer(nIDEvent);
}

void CFilterTestDiagDlg::OnBnClickedButton1Init()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if (m_isendingdata == 0)
	{
		CString strMsg;
		strMsg.Format(_T("Data Init Command"));
		AddInfo(strMsg);

		InitFitting();
	}
	else
	{
		CString strMsg;
		strMsg.Format(_T("Sending Data..Can't use Init Command"));
		AddInfo(strMsg);
	}
}


void CFilterTestDiagDlg::OnBnClickedButton2Send()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if (m_isendingdata == 0)
	{
		UpdateData(TRUE);

		CString strMsg;
		strMsg.Format(_T("Start to send the Shot Data"));
		AddInfo(strMsg);

		SetTimer(TIMER_RECV, 4, NULL);
		m_isendingdata = 1;

		fopen_s(&fp_w, "rawdata_result.txt", "w");

//�׽�Ʈ ������ �б�
		FILE* fp = NULL;
		fopen_s(&fp, "rawdata.txt", "r");  //test������ r(�б�) ���� ����
		
		for (int j = 0; j < 400; j++)
		{
			m_Buffer[j][0] = 0.0;
			m_Buffer[j][1] = 0.0;
			m_Buffer[j][2] = 0.0;
		}
		double a, b, c;
		int i = 0;
		m_isenddata_count = 0;
		m_isenddata_index = 0;
		if (fp != NULL)
		{
			//	fseek(fp, 0L, SEEK_SET);
			while (0 < fscanf_s(fp, "%lf  %lf  %lf", &a, &b, &c))
			{
				m_Buffer[i][0] = a;
				m_Buffer[i][1] = b;
				m_Buffer[i][2] = c;
				i++;
			}
			m_isenddata_count = i;
			//fread(buffer, sizeof(double), 1000, fp); //��ü �б�
			fclose(fp); //���� ������ �ݱ�
		}
	}
	else
	{
		CString strMsg;
		strMsg.Format(_T("Sending Data..Can't use Starting command"));
		AddInfo(strMsg);
	}
}

void CFilterTestDiagDlg::AddInfo(CString strInfo)
{

	SYSTEMTIME t;
	GetLocalTime(&t);

	CString strMsg;
	strMsg.Format(_T("[%02d:%02d:%02d.%03d] %s"), t.wHour, t.wMinute, t.wSecond, t.wMilliseconds, strInfo);

	m_listInfo.AddString(strMsg);
	m_listInfo.SetCurSel(m_listInfo.GetCount() - 1);
}
