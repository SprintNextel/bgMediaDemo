
// GMVideoSynPlayWithSDL2_V2.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CGMVideoSynPlayWithSDL2_V2App:
// �йش����ʵ�֣������ GMVideoSynPlayWithSDL2_V2.cpp
//

class CGMVideoSynPlayWithSDL2_V2App : public CWinAppEx
{
public:
	CGMVideoSynPlayWithSDL2_V2App();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CGMVideoSynPlayWithSDL2_V2App theApp;