
// WinpcapDlg.h : header file
//

#pragma once
#include "afxcmn.h"
#include "DataLinkLayer.h"
#include "NetworkLayer.h"
#include "TransportLayer.h"
#include "afxwin.h"


// CWinpcapDlg dialog
class CWinpcapDlg : public CDialogEx
{
// Construction
public:
	CWinpcapDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_WINPCAP_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	CTabCtrl m_tab;
	CDataLinkLayer m_dataLinkLayer;
	CNetworkLayer m_networkLayer;
	CTransportLayer m_transportLayer;
	afx_msg void OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult);
	CListBox m_deviceList;
	void addDevices();
};
