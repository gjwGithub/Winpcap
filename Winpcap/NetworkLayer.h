#pragma once
#include "afxcmn.h"
#include "pcap.h"


// CNetworkLayer �Ի���

class CNetworkLayer : public CDialog
{
	DECLARE_DYNAMIC(CNetworkLayer)

public:
	CNetworkLayer(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CNetworkLayer();

// �Ի�������
	enum { IDD = IDD_NETWORKLAYER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnstartNet();
	afx_msg void OnBnClickedBtnstopNet();
	CListCtrl m_list;
	CListBox* m_deviceList;
	CDialogEx* m_mainForm;
	void setMainForm(CDialogEx* me);
	CWinThread* m_IPv4Thread;
	CWinThread* m_IPv6Thread;
	CWinThread* m_ARPThread;
	bool m_runIPv4Thread;
	bool m_runIPv6Thread;
	bool m_runARPThread;
	int m_column;
};
