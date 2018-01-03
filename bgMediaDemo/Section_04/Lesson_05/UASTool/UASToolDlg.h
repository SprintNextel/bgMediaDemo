
// UASToolDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"

#include "UASImp.h"


// CUASToolDlg �Ի���
class CUASToolDlg : public CDialog
{
// ����
public:
	CUASToolDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_UASTOOL_DIALOG };

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
	CEdit m_cUasIp;
	CEdit m_cUasPort;
	CEdit m_cUasCode;
	afx_msg void OnBnClickedBtnStartService();
	afx_msg void OnBnClickedBtnStopService2();

public:
	bgUASImp uas_;
	CListBox m_cInfo;
};
