
// TestHarnessDlg.h : header file
//

#pragma once
#include "afxwin.h"

#include "vector"


// CTestHarnessDlg dialog
class CTestHarnessDlg : public CDialog
{
// Construction
public:
	CTestHarnessDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_TESTHARNESS_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCmdSelectPlaybackFile();

private:
	CString m_strSelectedPlaybackLogFile;
	int m_iSelectedPlaybackComPort;

	struct SPlaybackDataStream {
		CString m_strSelectedPlaybackLogFile;
		int m_iSelectedPlaybackComPort;

		SPlaybackDataStream() {
			m_strSelectedPlaybackLogFile = L"";
			m_iSelectedPlaybackComPort = -1;
		}

		SPlaybackDataStream(CString strSelectedPlaybackLogFile, int iSelectedPlaybackComPort) {
			m_strSelectedPlaybackLogFile = strSelectedPlaybackLogFile;
			m_iSelectedPlaybackComPort = iSelectedPlaybackComPort;
		}
	};

	std::vector<SPlaybackDataStream> m_vectPlaybackDataStreams;

	volatile long m_iPlaybackThreadCount;

public:
	CEdit m_txtSelectedLogFile;
	afx_msg void OnBnClickedCmdPlayback();
	CComboBox m_cboPlaybackComPorts;
	afx_msg void OnCbnSelchangeCboPlaybackComPort();
	CButton m_cmdPlayback;
	afx_msg void OnEnChangeTxtPlaybackFile();
	afx_msg void OnBnClickedCmdAddDataStream();
	CListBox m_lstPlaybackDataStreams;
	CButton m_cmdAddDataStream;
	afx_msg void OnLbnSelchangeLstPlaybackDataStreams();
	afx_msg void OnBnClickedCmdClear();
	CButton m_cmdClear;
	CStatic m_lblPlaybackInProgress;
	afx_msg void OnCmdExit();
};


