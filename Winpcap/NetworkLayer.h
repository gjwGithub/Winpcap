#pragma once
#include "afxcmn.h"


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
};
