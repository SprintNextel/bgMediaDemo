
// UASToolDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "UASTool.h"
#include "UASToolDlg.h"

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


// CUASToolDlg �Ի���




CUASToolDlg::CUASToolDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CUASToolDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CUASToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_UAS_IP, m_cUasIp);
	DDX_Control(pDX, IDC_EDIT_UAS_PORT, m_cUasPort);
	DDX_Control(pDX, IDC_EDIT_UAS_CODE, m_cUasCode);
	DDX_Control(pDX, IDC_LIST_INFO, m_cInfo);
}

BEGIN_MESSAGE_MAP(CUASToolDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BTN_START_SERVICE, &CUASToolDlg::OnBnClickedBtnStartService)
	ON_BN_CLICKED(IDC_BTN_STOP_SERVICE2, &CUASToolDlg::OnBnClickedBtnStopService2)
END_MESSAGE_MAP()


// CUASToolDlg ��Ϣ�������

BOOL CUASToolDlg::OnInitDialog()
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

	m_cUasIp.LimitText(15);
	m_cUasPort.LimitText(5);
	m_cUasCode.LimitText(20);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CUASToolDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CUASToolDlg::OnPaint()
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
HCURSOR CUASToolDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CUASToolDlg::OnBnClickedBtnStartService()
{
	// ��ȡ������������
	CString ip, port, code;
	m_cUasIp.GetWindowText(ip);
	m_cUasPort.GetWindowText(port);
	m_cUasCode.GetWindowText(code);

	USES_CONVERSION;
	int errCode = uas_.Init(T2A(code.GetString()), T2A(ip.GetString()), _ttoi(port.GetString()));
	if (errCode != 0)
	{
		m_cInfo.AddString(_T("��ʼ�� UAS ʧ�ܣ�"));
		return ;
	}
	else
		m_cInfo.AddString(_T("��ʼ�� UAS �ɹ���"));

	errCode = uas_.Start();
	if (errCode != 0)
		m_cInfo.AddString(_T("���� UAS ����ʧ�ܣ�"));
	else
		m_cInfo.AddString(_T("���� UAS ����ɹ���"));
}

void CUASToolDlg::OnBnClickedBtnStopService2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}