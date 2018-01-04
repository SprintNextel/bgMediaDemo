
// UACToolDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"

#include "UACImp.h"


// CUACToolDlg �Ի���
class CUACToolDlg : public CDialog
{
// ����
public:
	CUACToolDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_UACTOOL_DIALOG };

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
	afx_msg void OnEnChangeEdit2();
	afx_msg void OnBnClickedBtnStart();
	afx_msg void OnBnClickedBtnRegister();
	afx_msg void OnBnClickedBtnUnregister();
	afx_msg void OnBnClickedBtnCall();
	afx_msg void OnBnClickedBtnCloseCall();
	afx_msg void OnBnClickedBtnSendMessage();

public:
	CEdit m_cUasIp;
	CEdit m_cUasPort;
	CEdit m_cUasCode;
	CEdit m_cUacIp;
	CEdit m_cUacPort;
	CEdit m_cUacCode;
	CEdit m_cMessage;
	CListBox m_cInfo;

public:
	bgUACImp uac_;
	
};
