#pragma once


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
};
