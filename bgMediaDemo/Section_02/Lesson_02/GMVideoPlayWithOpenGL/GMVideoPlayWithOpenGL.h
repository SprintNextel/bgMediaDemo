
// GMVideoPlayWithOpenGL.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CGMVideoPlayWithOpenGLApp:
// �йش����ʵ�֣������ GMVideoPlayWithOpenGL.cpp
//

class CGMVideoPlayWithOpenGLApp : public CWinAppEx
{
public:
	CGMVideoPlayWithOpenGLApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CGMVideoPlayWithOpenGLApp theApp;