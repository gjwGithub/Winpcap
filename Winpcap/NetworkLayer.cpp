// NetworkLayer.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Winpcap.h"
#include "NetworkLayer.h"
#include "afxdialogex.h"


// CNetworkLayer �Ի���

IMPLEMENT_DYNAMIC(CNetworkLayer, CDialog)

CNetworkLayer::CNetworkLayer(CWnd* pParent /*=NULL*/)
	: CDialog(CNetworkLayer::IDD, pParent)
{

}

CNetworkLayer::~CNetworkLayer()
{
}

void CNetworkLayer::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CNetworkLayer, CDialog)
END_MESSAGE_MAP()


// CNetworkLayer ��Ϣ�������
