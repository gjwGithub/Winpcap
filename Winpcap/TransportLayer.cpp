// TransportLayer.cpp : 实现文件
//

#include "stdafx.h"
#include "Winpcap.h"
#include "TransportLayer.h"
#include "afxdialogex.h"


// CTransportLayer 对话框

IMPLEMENT_DYNAMIC(CTransportLayer, CDialog)

CTransportLayer::CTransportLayer(CWnd* pParent /*=NULL*/)
	: CDialog(CTransportLayer::IDD, pParent)
{

}

CTransportLayer::~CTransportLayer()
{
}

void CTransportLayer::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CTransportLayer, CDialog)
	ON_BN_CLICKED(ID_BTNSTART_TRANS, &CTransportLayer::OnBnClickedBtnstartTrans)
	ON_BN_CLICKED(ID_BTNSTOP_TRANS, &CTransportLayer::OnBnClickedBtnstopTrans)
END_MESSAGE_MAP()


// CTransportLayer 消息处理程序


void CTransportLayer::OnBnClickedBtnstartTrans()
{
	// TODO: Add your control notification handler code here

	CButton* start = (CButton*)GetDlgItem(ID_BTNSTART_TRANS);
	start->EnableWindow(FALSE);
	CButton* stop = (CButton*)GetDlgItem(ID_BTNSTOP_TRANS);
	stop->EnableWindow(TRUE);
}


void CTransportLayer::OnBnClickedBtnstopTrans()
{
	// TODO: Add your control notification handler code here

	CButton* start = (CButton*)GetDlgItem(ID_BTNSTART_TRANS);
	start->EnableWindow(TRUE);
	CButton* stop = (CButton*)GetDlgItem(ID_BTNSTOP_TRANS);
	stop->EnableWindow(FALSE);
}
