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
END_MESSAGE_MAP()


// CTransportLayer 消息处理程序
