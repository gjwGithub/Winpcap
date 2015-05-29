// NetworkLayer.cpp : 实现文件
//

#include "stdafx.h"
#include "Winpcap.h"
#include "NetworkLayer.h"
#include "afxdialogex.h"


// CNetworkLayer 对话框

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


// CNetworkLayer 消息处理程序
