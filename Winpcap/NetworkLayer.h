#pragma once
#include "afxcmn.h"


// CNetworkLayer 对话框

class CNetworkLayer : public CDialog
{
	DECLARE_DYNAMIC(CNetworkLayer)

public:
	CNetworkLayer(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CNetworkLayer();

// 对话框数据
	enum { IDD = IDD_NETWORKLAYER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnstartNet();
	afx_msg void OnBnClickedBtnstopNet();
	CListCtrl m_list;
	CListBox* m_deviceList;
	CDialogEx* m_mainForm;
	void setMainForm(CDialogEx* me);
};
