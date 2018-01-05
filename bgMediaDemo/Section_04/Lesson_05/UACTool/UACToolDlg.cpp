
// UACToolDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "UACTool.h"
#include "UACToolDlg.h"

#include <atlconv.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CUACToolDlg �Ի���




CUACToolDlg::CUACToolDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CUACToolDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CUACToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_UAS_IP, m_cUasIp);
	DDX_Control(pDX, IDC_EDIT_UAS_PORT, m_cUasPort);
	DDX_Control(pDX, IDC_EDIT_UAS_ID, m_cUasCode);
	DDX_Control(pDX, IDC_EDIT_UAC_IP, m_cUacIp);
	DDX_Control(pDX, IDC_EDIT_UAC_PORT, m_cUacPort);
	DDX_Control(pDX, IDC_EDIT_UAC_ID, m_cUacCode);
	DDX_Control(pDX, IDC_EDIT_MESSAGE, m_cMessage);
	DDX_Control(pDX, IDC_LIST_INFO, m_cInfo);
}

BEGIN_MESSAGE_MAP(CUACToolDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	//ON_EN_CHANGE(IDC_EDIT2, &CUACToolDlg::OnEnChangeEdit2)
	ON_BN_CLICKED(IDC_BTN_START, &CUACToolDlg::OnBnClickedBtnStart)
	ON_BN_CLICKED(IDC_BTN_REGISTER, &CUACToolDlg::OnBnClickedBtnRegister)
	ON_BN_CLICKED(IDC_BTN_UNREGISTER, &CUACToolDlg::OnBnClickedBtnUnregister)
	ON_BN_CLICKED(IDC_BTN_CALL, &CUACToolDlg::OnBnClickedBtnCall)
	ON_BN_CLICKED(IDC_BTN_CLOSE_CALL, &CUACToolDlg::OnBnClickedBtnCloseCall)
	ON_BN_CLICKED(IDC_BTN_SEND_MESSAGE, &CUACToolDlg::OnBnClickedBtnSendMessage)
END_MESSAGE_MAP()


// CUACToolDlg ��Ϣ�������

BOOL CUACToolDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	m_cUasIp.SetWindowText(_T("127.0.0.1"));
	m_cUasPort.SetWindowText(_T("5060"));
	m_cUasCode.SetWindowText(_T("44011200002110000001"));

	m_cUacIp.SetWindowText(_T("127.0.0.1"));
	m_cUacPort.SetWindowText(_T("5061"));
	m_cUacCode.SetWindowText(_T("44011200002110000002"));

	m_cUasIp.LimitText(15);
	m_cUasPort.LimitText(5);
	m_cUasCode.LimitText(20);

	m_cUacIp.LimitText(15);
	m_cUacPort.LimitText(5);
	m_cUacCode.LimitText(20);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CUACToolDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CUACToolDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CUACToolDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CUACToolDlg::OnEnChangeEdit2()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
}

void CUACToolDlg::OnBnClickedBtnStart()
{
	// ����������ݣ���������
	CString uac_ip, uac_port, uac_code;

	m_cUacIp.GetWindowText(uac_ip);
	m_cUacPort.GetWindowText(uac_port);
	m_cUacCode.GetWindowText(uac_code);

	USES_CONVERSION;
	int errCode = uac_.Init(T2A(uac_code.GetString()), T2A(uac_ip.GetString()), _ttoi(uac_port.GetString()));
	if (errCode != 0)
	{
		m_cInfo.AddString(_T("��ʼ�� UAC ʧ�ܣ�"));
		return ;
	}
	else
		m_cInfo.AddString(_T("��ʼ�� UAC �ɹ���"));
}

void CUACToolDlg::OnBnClickedBtnRegister()
{
	//
	// ע��
	//

	CString uas_ip, uas_port, uas_code;

	m_cUasIp.GetWindowText(uas_ip);
	m_cUasPort.GetWindowText(uas_port);
	m_cUasCode.GetWindowText(uas_code);

	USES_CONVERSION;
	uac_.SetUASEnvironment(T2A(uas_code.GetString()), T2A(uas_ip.GetString()), _ttoi(uas_port.GetString()));

	m_cInfo.AddString(_T("����ע������..."));

	int errCode = uac_.Register();
	if (errCode != 0)
		m_cInfo.AddString(_T("ע��ʧ�ܣ�"));
	else
		m_cInfo.AddString(_T("ע��ɹ���"));
}

void CUACToolDlg::OnBnClickedBtnUnregister()
{
	//
	// ע��
	//
	m_cInfo.AddString(_T("����ע������..."));

	int errCode = uac_.Unregister();
	if (errCode != 0)
		m_cInfo.AddString(_T("ע��ʧ�ܣ�"));
	else
		m_cInfo.AddString(_T("ע���ɹ���"));
}

void CUACToolDlg::OnBnClickedBtnCall()
{
	//
	// ͨ��
	//

	m_cInfo.AddString(_T("����ͨ������..."));

	int errCode = uac_.Call();
	if (errCode != 0)
		m_cInfo.AddString(_T("ͨ��ʧ�ܣ�"));
	else
		m_cInfo.AddString(_T("ͨ���ɹ���"));
}

void CUACToolDlg::OnBnClickedBtnCloseCall()
{
	//
	// �Ҷ�
	//

	m_cInfo.AddString(_T("���͹Ҷ�����..."));

	int errCode = uac_.ReleaseCall();
	if (errCode != 0)
		m_cInfo.AddString(_T("�Ҷ�ʧ�ܣ�"));
	else
		m_cInfo.AddString(_T("�Ҷϳɹ���"));
}

void CUACToolDlg::OnBnClickedBtnSendMessage()
{
	//
	// ������Ϣ
	//
	CString uas_ip, uas_port, uas_code;

	m_cUasIp.GetWindowText(uas_ip);
	m_cUasPort.GetWindowText(uas_port);
	m_cUasCode.GetWindowText(uas_code);

	USES_CONVERSION;
	uac_.SetUASEnvironment(T2A(uas_code.GetString()), T2A(uas_ip.GetString()), _ttoi(uas_port.GetString()));

	CString sms;
	m_cMessage.GetWindowText(sms);

	m_cInfo.AddString(_T("���Ͷ���Ϣ..."));

	int errCode = uac_.SendSMS(T2A(sms.GetString()));
	if (errCode != 0)
		m_cInfo.AddString(_T("����Ϣ����ʧ�ܣ�"));
	else
		m_cInfo.AddString(_T("����Ϣ���ͳɹ���"));
}
