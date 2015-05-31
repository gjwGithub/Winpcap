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
	, m_deviceList(NULL)
	, m_mainForm(NULL)
{

}

CNetworkLayer::~CNetworkLayer()
{
}

void CNetworkLayer::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_NET, m_list);

	m_list.SetExtendedStyle(m_list.GetExtendedStyle()
		| LVS_EX_GRIDLINES
		| LVS_EX_FULLROWSELECT);
	m_list.InsertColumn(0, _T("Time"));
	m_list.InsertColumn(1, _T("Form IP"));
	m_list.InsertColumn(2, _T("To IP"));
	m_list.InsertColumn(3, _T("TTL"));
	m_list.InsertColumn(4, _T("Type"));
	m_list.InsertColumn(5, _T("Length"));

	CRect rect;
	m_list.GetClientRect(rect); //获得当前客户区信息
	int columnCount = 6;
	for (int i = 0; i < columnCount; i++)
	{
		m_list.SetColumnWidth(i, rect.Width() / columnCount); //设置列的宽度。
	}

	m_deviceList = (CListBox*)m_mainForm->GetDlgItem(IDC_LIST);
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

void CNetworkLayer::setMainForm(CDialogEx* me)
{
	m_mainForm = me;
}
