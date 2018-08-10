
// GMVideoPlayWithOpenGLDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"
#include "GxxGmDecoderPlay.h"


// CGMVideoPlayWithOpenGLDlg �Ի���
class CGMVideoPlayWithOpenGLDlg : public CDialog
{
// ����
public:
	CGMVideoPlayWithOpenGLDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_GMVIDEOPLAYWITHOPENGL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	GxxGmDecoderPlay decoder;
	CEdit m_cUrl;
	afx_msg void OnBnClickedBtnBrowse();
	afx_msg void OnBnClickedBtnDemuxing();
	afx_msg void OnBnClickedBtnPlay();
	afx_msg void OnBnClickedBtnStop();
};
