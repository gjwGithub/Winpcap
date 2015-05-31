// TransportLayer.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Winpcap.h"
#include "TransportLayer.h"
#include "afxdialogex.h"


// CTransportLayer �Ի���

IMPLEMENT_DYNAMIC(CTransportLayer, CDialog)

CTransportLayer::CTransportLayer(CWnd* pParent /*=NULL*/)
	: CDialog(CTransportLayer::IDD, pParent)
	, m_deviceList(NULL)
	, m_mainForm(NULL)
{

}

CTransportLayer::~CTransportLayer()
{
}

void CTransportLayer::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_TRANS, m_list);

	m_list.SetExtendedStyle(m_list.GetExtendedStyle()
		| LVS_EX_GRIDLINES
		| LVS_EX_FULLROWSELECT);
	m_list.InsertColumn(0, _T("Time"));
	m_list.InsertColumn(1, _T("Form Port"));
	m_list.InsertColumn(2, _T("To Port"));
	m_list.InsertColumn(3, _T("Seq Num"));
	m_list.InsertColumn(4, _T("Type"));
	m_list.InsertColumn(5, _T("Length"));

	CRect rect;
	m_list.GetClientRect(rect); //��õ�ǰ�ͻ�����Ϣ
	int columnCount = 6;
	for (int i = 0; i < columnCount; i++)
	{
		m_list.SetColumnWidth(i, rect.Width() / columnCount); //�����еĿ�ȡ�
	}

	m_deviceList = (CListBox*)m_mainForm->GetDlgItem(IDC_LIST);
}


BEGIN_MESSAGE_MAP(CTransportLayer, CDialog)
	ON_BN_CLICKED(ID_BTNSTART_TRANS, &CTransportLayer::OnBnClickedBtnstartTrans)
	ON_BN_CLICKED(ID_BTNSTOP_TRANS, &CTransportLayer::OnBnClickedBtnstopTrans)
END_MESSAGE_MAP()


// CTransportLayer ��Ϣ�������


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


void CTransportLayer::setMainForm(CDialogEx* me)
{
	m_mainForm = me;
}
