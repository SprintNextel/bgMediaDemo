
// GMMediaPlayer.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CGMMediaPlayerApp:
// �йش����ʵ�֣������ GMMediaPlayer.cpp
//

class CGMMediaPlayerApp : public CWinAppEx
{
public:
	CGMMediaPlayerApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CGMMediaPlayerApp theApp;