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
	ON_BN_CLICKED(ID_BTNSTART_NET, &CNetworkLayer::OnBnClickedBtnstartNet)
	ON_BN_CLICKED(ID_BTNSTOP_NET, &CNetworkLayer::OnBnClickedBtnstopNet)
END_MESSAGE_MAP()


// CNetworkLayer 消息处理程序


void CNetworkLayer::OnBnClickedBtnstartNet()
{
	// TODO: Add your control notification handler code here

	CButton* start = (CButton*)GetDlgItem(ID_BTNSTART_NET);
	start->EnableWindow(FALSE);
	CButton* stop = (CButton*)GetDlgItem(ID_BTNSTOP_NET);
	stop->EnableWindow(TRUE);
}


void CNetworkLayer::OnBnClickedBtnstopNet()
{
	// TODO: Add your control notification handler code here

	CButton* start = (CButton*)GetDlgItem(ID_BTNSTART_NET);
	start->EnableWindow(TRUE);
	CButton* stop = (CButton*)GetDlgItem(ID_BTNSTOP_NET);
	stop->EnableWindow(FALSE);
}
