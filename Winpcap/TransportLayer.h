#pragma once
#include "afxcmn.h"
#include "pcap.h"


// CTransportLayer 对话框

class CTransportLayer : public CDialog
{
	DECLARE_DYNAMIC(CTransportLayer)

public:
	CTransportLayer(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CTransportLayer();

// 对话框数据
	enum { IDD = IDD_TRANSPORTLAYER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnstartTrans();
	afx_msg void OnBnClickedBtnstopTrans();
	CListCtrl m_list;
	CListBox* m_deviceList;
	CDialogEx* m_mainForm;
	void setMainForm(CDialogEx* me);
	bool m_runTCPThread;
	bool m_runUDPThread;
	CWinThread* m_TCPThread;
	CWinThread* m_UDPThread;
	int m_column;
};
