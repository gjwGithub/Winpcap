
// WinpcapDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Winpcap.h"
#include "WinpcapDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CWinpcapDlg dialog



CWinpcapDlg::CWinpcapDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CWinpcapDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CWinpcapDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB, m_tab);
	DDX_Control(pDX, IDC_LIST, m_deviceList);
}

BEGIN_MESSAGE_MAP(CWinpcapDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CWinpcapDlg::OnBnClickedOk)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB, &CWinpcapDlg::OnTcnSelchangeTab)
END_MESSAGE_MAP()


// CWinpcapDlg message handlers

BOOL CWinpcapDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here


	m_tab.InsertItem(0, L"Data Link Layer");
	m_tab.InsertItem(1, L"Network Layer");
	m_tab.InsertItem(2, L"Transport Layer");

	m_dataLinkLayer.setMainForm(this);
	m_dataLinkLayer.Create(IDD_DATALINKLAYER, GetDlgItem(IDC_TAB));
	
	m_networkLayer.setMainForm(this);
	m_networkLayer.Create(IDD_NETWORKLAYER, GetDlgItem(IDC_TAB));

	m_transportLayer.setMainForm(this);
	m_transportLayer.Create(IDD_TRANSPORTLAYER, GetDlgItem(IDC_TAB));

	//获得IDC_TABTEST客户区大小 
	CRect rs;
	m_tab.GetClientRect(&rs);
	//调整子对话框在父窗口中的位置 
	rs.top += 21;
	rs.bottom -= 2;
	rs.left += 2;
	rs.right -= 3;
	
	//设置子对话框尺寸并移动到指定位置 
	m_dataLinkLayer.MoveWindow(&rs);
	m_networkLayer.MoveWindow(&rs);
	m_transportLayer.MoveWindow(&rs);

	//分别设置隐藏和显示 
	m_dataLinkLayer.ShowWindow(true);
	m_networkLayer.ShowWindow(false);
	m_transportLayer.ShowWindow(false);
	
	//设置默认的选项卡 
	m_tab.SetCurSel(0);

	addDevices();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CWinpcapDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CWinpcapDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CWinpcapDlg::OnBnClickedOk()
{
	// TODO:  在此添加控件通知处理程序代码
	//CDialogEx::OnOK();

}


void CWinpcapDlg::OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO:  在此添加控件通知处理程序代码
	int CurSel = m_tab.GetCurSel();
	switch (CurSel)
	{
	case 0:
		m_dataLinkLayer.ShowWindow(true);
		m_networkLayer.ShowWindow(false);
		m_transportLayer.ShowWindow(false);
		break;
	case 1:
		m_dataLinkLayer.ShowWindow(false);
		m_networkLayer.ShowWindow(true);
		m_transportLayer.ShowWindow(false);
		break;
	case 2:
		m_dataLinkLayer.ShowWindow(false);
		m_networkLayer.ShowWindow(false);
		m_transportLayer.ShowWindow(true);
		break;
	default:
		break;
	}

	*pResult = 0;
}


void CWinpcapDlg::addDevices()
{
	pcap_if_t *alldevs;
	pcap_if_t *d;
	int i = 0;
	char errbuf[PCAP_ERRBUF_SIZE];

	/* Retrieve the device list on the local machine */
	if (pcap_findalldevs_ex(PCAP_SRC_IF_STRING, NULL, &alldevs, errbuf) == -1)
	{
		CString string;
		string.Format(L"Error in pcap_findalldevs: %s\n", errbuf);
		AfxMessageBox(string);
		return;
	}

	/* Print the list */
	for (d = alldevs; d; d = d->next)
	{
		CString str;
		if (d->description)
			str=d->description;
		else
			str=" (No description available)\n";
		m_deviceList.InsertString(i++, str);
		/*printf("%d. %s", ++i, d->name);
		if (d->description)
		printf(" (%s)\n", d->description);
		else
		printf(" (No description available)\n");*/
	}

	if (i == 0)
	{
		CString string;
		string.Format(L"\nNo interfaces found! Make sure WinPcap is installed.\n");
		AfxMessageBox(string);
		return;
	}
}
