
// GMVideoPlayWithOpenGLDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "GMVideoPlayWithOpenGL.h"
#include "GMVideoPlayWithOpenGLDlg.h"

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


// CGMVideoPlayWithOpenGLDlg �Ի���




CGMVideoPlayWithOpenGLDlg::CGMVideoPlayWithOpenGLDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGMVideoPlayWithOpenGLDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CGMVideoPlayWithOpenGLDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_URL, m_cUrl);
}

BEGIN_MESSAGE_MAP(CGMVideoPlayWithOpenGLDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BTN_BROWSE, &CGMVideoPlayWithOpenGLDlg::OnBnClickedBtnBrowse)
	ON_BN_CLICKED(IDC_BTN_DEMUXING, &CGMVideoPlayWithOpenGLDlg::OnBnClickedBtnDemuxing)
	ON_BN_CLICKED(IDC_BTN_PLAY, &CGMVideoPlayWithOpenGLDlg::OnBnClickedBtnPlay)
	ON_BN_CLICKED(IDC_BTN_STOP, &CGMVideoPlayWithOpenGLDlg::OnBnClickedBtnStop)
END_MESSAGE_MAP()


// CGMVideoPlayWithOpenGLDlg ��Ϣ�������

BOOL CGMVideoPlayWithOpenGLDlg::OnInitDialog()
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
	m_cUrl.SetWindowText(_T("C:\\Users\\WANGY\\Desktop\\video\\С�����Ϊʲôû��Ů����.mp4"));

	CWnd *pcwnd = GetDlgItem(IDC_STATIC_SCREEN);
	decoder.Initialize((int)pcwnd->GetSafeHwnd());

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CGMVideoPlayWithOpenGLDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CGMVideoPlayWithOpenGLDlg::OnPaint()
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
HCURSOR CGMVideoPlayWithOpenGLDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CGMVideoPlayWithOpenGLDlg::OnBnClickedBtnBrowse()
{
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("MP4 File (*.mp4)|*.mp4||"), this);
	INT_PTR ret = dlg.DoModal();

	if (ret == IDOK)
	{
		CString url = dlg.GetPathName();
		m_cUrl.SetWindowText(url);
	}
}

void CGMVideoPlayWithOpenGLDlg::OnBnClickedBtnDemuxing()
{
	USES_CONVERSION;
	CString str_url;
	m_cUrl.GetWindowText(str_url);

	// �⸴��
	int errCode = decoder.Demuxing(T2A(str_url.GetBuffer(0)));
	if (errCode != 0)
		TRACE("�⸴��ʧ�ܣ�\n");
}

void CGMVideoPlayWithOpenGLDlg::OnBnClickedBtnPlay()
{
	int errCode = decoder.Play();
}

void CGMVideoPlayWithOpenGLDlg::OnBnClickedBtnStop()
{
	decoder.Stop();
}
