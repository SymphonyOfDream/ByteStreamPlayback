
// TestHarnessDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TestHarness.h"
#include "TestHarnessDlg.h"

#include "../../PortPlaybackThread.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
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


// CTestHarnessDlg dialog




CTestHarnessDlg::CTestHarnessDlg(CWnd* pParent /*=NULL*/)
: CDialog(CTestHarnessDlg::IDD, pParent)
, m_iSelectedPlaybackComPort(-1)
, m_strSelectedPlaybackLogFile(L"")
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

}

void CTestHarnessDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TXT_PLAYBACK_FILE, m_txtSelectedLogFile);
	DDX_Control(pDX, IDC_CBO_PLAYBACK_COM_PORT, m_cboPlaybackComPorts);
	DDX_Control(pDX, IDC_CMD_PLAYBACK, m_cmdPlayback);
	DDX_Control(pDX, IDC_LST_PLAYBACK_DATA_STREAMS, m_lstPlaybackDataStreams);
	DDX_Control(pDX, IDC_CMD_ADD_DATA_STREAM, m_cmdAddDataStream);
	DDX_Control(pDX, IDC_CMD_CLEAR, m_cmdClear);
	DDX_Control(pDX, IDC_LBL_PLAYBACK_IN_PROGRESS, m_lblPlaybackInProgress);
}

BEGIN_MESSAGE_MAP(CTestHarnessDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_CMD_SELECT_PLAYBACK_FILE, &CTestHarnessDlg::OnBnClickedCmdSelectPlaybackFile)
	ON_BN_CLICKED(IDC_CMD_PLAYBACK, &CTestHarnessDlg::OnBnClickedCmdPlayback)
	ON_CBN_SELCHANGE(IDC_CBO_PLAYBACK_COM_PORT, &CTestHarnessDlg::OnCbnSelchangeCboPlaybackComPort)
	ON_EN_CHANGE(IDC_TXT_PLAYBACK_FILE, &CTestHarnessDlg::OnEnChangeTxtPlaybackFile)
	ON_BN_CLICKED(IDC_CMD_ADD_DATA_STREAM, &CTestHarnessDlg::OnBnClickedCmdAddDataStream)
	ON_LBN_SELCHANGE(IDC_LST_PLAYBACK_DATA_STREAMS, &CTestHarnessDlg::OnLbnSelchangeLstPlaybackDataStreams)
	ON_BN_CLICKED(IDC_CMD_CLEAR, &CTestHarnessDlg::OnBnClickedCmdClear)
	ON_BN_CLICKED(IDCANCEL, &CTestHarnessDlg::OnCmdExit)
END_MESSAGE_MAP()


// CTestHarnessDlg message handlers

BOOL CTestHarnessDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
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

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	CString str;
	for (int iPortNbr = 1; iPortNbr < 100; ++iPortNbr) {
		str.Format(L"%d", iPortNbr);
		m_cboPlaybackComPorts.AddString(str);
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTestHarnessDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTestHarnessDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTestHarnessDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CTestHarnessDlg::OnBnClickedCmdSelectPlaybackFile()
{
	CString strFilter = L"Serial Logs (*.log)|*.log|All Files (*.*)|*.*||";

	CFileDialog FileDlg(TRUE, L".log", NULL, OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST, strFilter, this);

	if (FileDlg.DoModal() == IDOK) {
		m_strSelectedPlaybackLogFile = FileDlg.GetPathName();
		m_txtSelectedLogFile.SetWindowText((LPCTSTR)m_strSelectedPlaybackLogFile);
	}

	m_cmdAddDataStream.EnableWindow(m_iSelectedPlaybackComPort > 0 && m_strSelectedPlaybackLogFile.GetLength() > 0);
}

void CTestHarnessDlg::OnCbnSelchangeCboPlaybackComPort()
{
	m_iSelectedPlaybackComPort = m_cboPlaybackComPorts.GetCurSel();
	if (m_iSelectedPlaybackComPort >= 0) {
		++m_iSelectedPlaybackComPort;
	}

	m_cmdAddDataStream.EnableWindow(m_iSelectedPlaybackComPort > 0 && m_strSelectedPlaybackLogFile.GetLength() > 0);
}

void CTestHarnessDlg::OnEnChangeTxtPlaybackFile()
{
	m_txtSelectedLogFile.GetWindowText(m_strSelectedPlaybackLogFile);

	m_cmdAddDataStream.EnableWindow(m_iSelectedPlaybackComPort > 0 && m_strSelectedPlaybackLogFile.GetLength() > 0);
}

void CTestHarnessDlg::OnBnClickedCmdAddDataStream()
{
	m_vectPlaybackDataStreams.push_back(SPlaybackDataStream(m_strSelectedPlaybackLogFile, m_iSelectedPlaybackComPort));
	CString str;
	str.Format(L"%s <=> Playback COM Port: COM%d:", (LPCTSTR)m_strSelectedPlaybackLogFile, m_iSelectedPlaybackComPort);
	m_lstPlaybackDataStreams.AddString(str);

	m_strSelectedPlaybackLogFile = L"";
	m_iSelectedPlaybackComPort = -1;
	m_cboPlaybackComPorts.SetCurSel(-1);
	m_txtSelectedLogFile.SetWindowText(L"");

	m_cmdPlayback.EnableWindow(TRUE);
}

void CTestHarnessDlg::OnLbnSelchangeLstPlaybackDataStreams()
{
	m_cmdPlayback.EnableWindow(m_lstPlaybackDataStreams.GetCount());
}

void CTestHarnessDlg::OnBnClickedCmdClear()
{
	m_vectPlaybackDataStreams.clear();
	m_lstPlaybackDataStreams.ResetContent();
	m_cmdPlayback.EnableWindow(FALSE);
}


void CTestHarnessDlg::OnBnClickedCmdPlayback()
{
	m_cmdPlayback.EnableWindow(FALSE);
	m_cmdAddDataStream.EnableWindow(FALSE);
	m_cmdClear.EnableWindow(FALSE);

	PortPlayback_Initialize();

	for (int idx = 0; idx < m_vectPlaybackDataStreams.size(); ++idx) 
	{
		CString strPlaybackFile = m_vectPlaybackDataStreams[idx].m_strSelectedPlaybackLogFile;
		int iPlaybackPort = m_vectPlaybackDataStreams[idx].m_iSelectedPlaybackComPort;

		int iRC = PortPlayback_AddDataStream((LPCTSTR)strPlaybackFile, iPlaybackPort);

		switch (iRC) {
			case -1:
				AfxMessageBox(L"Library not initialized!");
				break;
			case -2:
				AfxMessageBox(L"File specified cannot be opened.");
				break;
			case -3:
				AfxMessageBox(L"Error reading file.");
				break;
			case -4:
				AfxMessageBox(L"Error reading file.");
				break;
			case -5:
				AfxMessageBox(L"Line endings in file do not seem to be \\r\\n.");
				break;
			case -6:
				AfxMessageBox(L"Could not open COM port specified.");
				break;
			case -7:
				AfxMessageBox(L"Could not SET COM port CommState.");
				break;
			case -8:
				AfxMessageBox(L"Could not GET COM port CommState.");
				break;
			case -9:
				AfxMessageBox(L"Could not SET COM port timeouts.");
				break;
			case -10:
				AfxMessageBox(L"Could not GET COM port timeouts.");
				break;
			case -11:
				AfxMessageBox(L"Header line had empty values.");
				break;
		}

		if (iRC < 0) {
			AfxMessageBox(L"ERROR in source log file, no action taken.");
			m_cmdPlayback.EnableWindow(TRUE);
			m_cmdAddDataStream.EnableWindow(m_iSelectedPlaybackComPort > 0 && m_strSelectedPlaybackLogFile.GetLength() > 0);
			m_cmdClear.EnableWindow(TRUE);
			return;
		}
	}

	m_lblPlaybackInProgress.ShowWindow(SW_SHOW);

	::DoEvents();

	m_iPlaybackThreadCount = 0;

	PortPlayback_Play(&m_iPlaybackThreadCount);

	// use do/while so we sleep 1/2 second to give library a chance to spawn off playback threads,
	// thus increasing the m_iPlaybackThreadCount count
	do {
		::Sleep(1000);
		::DoEvents();
	} while (m_iPlaybackThreadCount > 0);

	PortPlayback_DeInitialize();

	m_lblPlaybackInProgress.ShowWindow(SW_HIDE);

	m_cmdPlayback.EnableWindow(TRUE);
	m_cmdAddDataStream.EnableWindow(m_iSelectedPlaybackComPort > 0 && m_strSelectedPlaybackLogFile.GetLength() > 0);
	m_cmdClear.EnableWindow(TRUE);
}

void CTestHarnessDlg::OnCmdExit()
{
	PortPlayback_DeInitialize();
	OnCancel();
}
