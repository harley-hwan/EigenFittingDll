
// FilterTestDiag.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.


// CFilterTestDiagApp:
// �� Ŭ������ ������ ���ؼ��� FilterTestDiag.cpp�� �����Ͻʽÿ�.
//

class CFilterTestDiagApp : public CWinApp
{
public:
	CFilterTestDiagApp();

// �������Դϴ�.
public:
	virtual BOOL InitInstance();

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};

extern CFilterTestDiagApp theApp;