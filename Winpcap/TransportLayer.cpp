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
	, m_deviceList(NULL)
	, m_mainForm(NULL)
	, m_runTCPThread(false)
	, m_runUDPThread(false)
	, m_TCPThread(NULL)
	, m_UDPThread(NULL)
	, m_column(0)
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
	m_list.GetClientRect(rect); //获得当前客户区信息
	int columnCount = 6;
	for (int i = 0; i < columnCount; i++)
	{
		m_list.SetColumnWidth(i, rect.Width() / columnCount); //设置列的宽度。
	}

	m_deviceList = (CListBox*)m_mainForm->GetDlgItem(IDC_LIST);
}


BEGIN_MESSAGE_MAP(CTransportLayer, CDialog)
	ON_BN_CLICKED(ID_BTNSTART_TRANS, &CTransportLayer::OnBnClickedBtnstartTrans)
	ON_BN_CLICKED(ID_BTNSTOP_TRANS, &CTransportLayer::OnBnClickedBtnstopTrans)
END_MESSAGE_MAP()


// CTransportLayer 消息处理程序

bool isWaitingForTCPInit = true;

UINT TCPThreadFunc(LPVOID lpParam)
{
	CTransportLayer* me = (CTransportLayer*)lpParam;

	pcap_if_t *alldevs;
	pcap_if_t *d;
	int inum;
	int i = 0;
	pcap_t *adhandle;
	int res;
	char errbuf[PCAP_ERRBUF_SIZE];
	struct tm ltime;
	char timestr[16];
	struct pcap_pkthdr *header;
	const u_char *pkt_data;
	time_t local_tv_sec;


	/* Retrieve the device list on the local machine */
	if (pcap_findalldevs_ex(PCAP_SRC_IF_STRING, NULL, &alldevs, errbuf) == -1)
	{
		CString string;
		string.Format(L"Error in pcap_findalldevs: %s\n", errbuf);
		AfxMessageBox(string);
		return 0;
	}

	//printf("Enter the interface number (1-%d):", i);
	//scanf_s("%d", &inum);
	int index;
	index = me->m_deviceList->GetCurSel();
	if (index == -1)
	{
		AfxMessageBox(L"Must select a device!");
		return 0;
	}
	else
	{
		inum = index;
	}

	/* Jump to the selected adapter */
	for (d = alldevs, i = 0; i< inum - 1; d = d->next, i++);

	/* Open the device */
	if ((adhandle = pcap_open(d->name,          // name of the device
		65536,            // portion of the packet to capture. 
		// 65536 guarantees that the whole packet will be captured on all the link layers
		PCAP_OPENFLAG_PROMISCUOUS,    // promiscuous mode
		1000,             // read timeout
		NULL,             // authentication on the remote machine
		errbuf            // error buffer
		)) == NULL)
	{
		CString string;
		string.Format(L"\nUnable to open the adapter. %s is not supported by WinPcap\n", d->name);
		AfxMessageBox(string);
		/* Free the device list */
		pcap_freealldevs(alldevs);
		return 0;
	}

	struct bpf_program fcode;
	/* Don't care about netmask, it won't be used for this filter */
	u_int netmask = 0xffffff;

	//compile the filter
	if (pcap_compile(adhandle, &fcode, "tcp", 1, netmask) <0)
	{
		CString string;
		string.Format(L"\nUnable to compile the packet filter. Check the syntax.\n");
		AfxMessageBox(string);
		/* Free the device list */
		return 0;
	}

	//set the filter
	if (pcap_setfilter(adhandle, &fcode)<0)
	{
		CString string;
		string.Format(L"\nError setting the filter.\n");
		AfxMessageBox(string);
		pcap_close(adhandle);
		/* Free the device list */
		return 0;
	}

	pcap_freealldevs(alldevs);

	isWaitingForTCPInit = false;

	while ((res = pcap_next_ex(adhandle, &header, &pkt_data)) >= 0 && me->m_runTCPThread) {

		if (res == 0)
			/* Timeout elapsed */
			continue;

		/* convert the timestamp to readable format */
		local_tv_sec = header->ts.tv_sec;
		localtime_s(&ltime, &local_tv_sec);
		strftime(timestr, sizeof timestr, "%H:%M:%S", &ltime);

		CString time;
		time += timestr;
		time += ",";
		time.AppendFormat(L"%.6d", header->ts.tv_usec);
		CString toPort;
		toPort.Format(L"%d", (int)pkt_data[2] * 256 + (int)pkt_data[3]);
		CString fromPort;
		fromPort.Format(L"%d", (int)pkt_data[0] * 256 + (int)pkt_data[1]);
		CString seqNum;
		seqNum.Format(L"%u", (unsigned int)pkt_data[4] * 256 * 256 * 256 + (unsigned int)pkt_data[5] * 256 * 256 + (unsigned int)pkt_data[6] * 256 + (unsigned int)pkt_data[7]);
		CString type;
		type.Format(L"TCP");
		CString length;
		length.Format(L"%d", header->len);
		me->m_list.InsertItem(me->m_column, time);
		me->m_list.SetItemText(me->m_column, 1, fromPort);
		me->m_list.SetItemText(me->m_column, 2, toPort);
		me->m_list.SetItemText(me->m_column, 3, seqNum);
		me->m_list.SetItemText(me->m_column, 4, type);
		me->m_list.SetItemText(me->m_column, 5, length);
		me->m_list.EnsureVisible(me->m_column, FALSE);

		me->m_column++;
	}

	if (res == -1) {
		CString string;
		string.Format(L"Error reading the packets: %s\n", pcap_geterr(adhandle));
		AfxMessageBox(string);
		return -1;
	}

	return 0;
}

UINT UDPThreadFunc(LPVOID lpParam)
{
	CTransportLayer* me = (CTransportLayer*)lpParam;

	while (isWaitingForTCPInit);

	pcap_if_t *alldevs;
	pcap_if_t *d;
	int inum;
	int i = 0;
	pcap_t *adhandle;
	int res;
	char errbuf[PCAP_ERRBUF_SIZE];
	struct tm ltime;
	char timestr[16];
	struct pcap_pkthdr *header;
	const u_char *pkt_data;
	time_t local_tv_sec;


	/* Retrieve the device list on the local machine */
	if (pcap_findalldevs_ex(PCAP_SRC_IF_STRING, NULL, &alldevs, errbuf) == -1)
	{
		CString string;
		string.Format(L"Error in pcap_findalldevs: %s\n", errbuf);
		AfxMessageBox(string);
		return 0;
	}

	//printf("Enter the interface number (1-%d):", i);
	//scanf_s("%d", &inum);
	int index;
	index = me->m_deviceList->GetCurSel();
	if (index == -1)
	{
		AfxMessageBox(L"Must select a device!");
		return 0;
	}
	else
	{
		inum = index;
	}

	/* Jump to the selected adapter */
	for (d = alldevs, i = 0; i< inum - 1; d = d->next, i++);

	/* Open the device */
	if ((adhandle = pcap_open(d->name,          // name of the device
		65536,            // portion of the packet to capture. 
		// 65536 guarantees that the whole packet will be captured on all the link layers
		PCAP_OPENFLAG_PROMISCUOUS,    // promiscuous mode
		1000,             // read timeout
		NULL,             // authentication on the remote machine
		errbuf            // error buffer
		)) == NULL)
	{
		CString string;
		string.Format(L"\nUnable to open the adapter. %s is not supported by WinPcap\n", d->name);
		AfxMessageBox(string);
		/* Free the device list */
		pcap_freealldevs(alldevs);
		return 0;
	}

	struct bpf_program fcode;
	/* Don't care about netmask, it won't be used for this filter */
	u_int netmask = 0xffffff;

	//compile the filter
	if (pcap_compile(adhandle, &fcode, "udp", 1, netmask) <0)
	{
		CString string;
		string.Format(L"\nUnable to compile the packet filter. Check the syntax.\n");
		AfxMessageBox(string);
		/* Free the device list */
		return 0;
	}

	//set the filter
	if (pcap_setfilter(adhandle, &fcode)<0)
	{
		CString string;
		string.Format(L"\nError setting the filter.\n");
		AfxMessageBox(string);
		pcap_close(adhandle);
		/* Free the device list */
		return 0;
	}

	pcap_freealldevs(alldevs);

	isWaitingForTCPInit = false;

	while ((res = pcap_next_ex(adhandle, &header, &pkt_data)) >= 0 && me->m_runTCPThread) {

		if (res == 0)
			/* Timeout elapsed */
			continue;

		/* convert the timestamp to readable format */
		local_tv_sec = header->ts.tv_sec;
		localtime_s(&ltime, &local_tv_sec);
		strftime(timestr, sizeof timestr, "%H:%M:%S", &ltime);

		CString time;
		time += timestr;
		time += ",";
		time.AppendFormat(L"%.6d", header->ts.tv_usec);
		CString toPort;
		toPort.Format(L"%d", (int)pkt_data[2] * 256 + (int)pkt_data[3]);
		CString fromPort;
		fromPort.Format(L"%d", (int)pkt_data[0] * 256 + (int)pkt_data[1]);
		CString seqNum;
		seqNum.Format(L"N.A.");
		CString type;
		type.Format(L"UDP");
		CString length;
		length.Format(L"%d", header->len);
		me->m_list.InsertItem(me->m_column, time);
		me->m_list.SetItemText(me->m_column, 1, fromPort);
		me->m_list.SetItemText(me->m_column, 2, toPort);
		me->m_list.SetItemText(me->m_column, 3, seqNum);
		me->m_list.SetItemText(me->m_column, 4, type);
		me->m_list.SetItemText(me->m_column, 5, length);
		me->m_list.EnsureVisible(me->m_column, FALSE);

		me->m_column++;
	}

	if (res == -1) {
		CString string;
		string.Format(L"Error reading the packets: %s\n", pcap_geterr(adhandle));
		AfxMessageBox(string);
		return -1;
	}

	return 0;
}

void CTransportLayer::OnBnClickedBtnstartTrans()
{
	// TODO: Add your control notification handler code here
	int index;
	index = m_deviceList->GetCurSel();
	if (index == -1)
	{
		AfxMessageBox(L"Must select a device!");
		return;
	}

	m_runTCPThread = true;
	m_TCPThread = AfxBeginThread(TCPThreadFunc, (LPVOID)this);
	m_runUDPThread = true;
	m_UDPThread = AfxBeginThread(UDPThreadFunc, (LPVOID)this);

	CButton* start = (CButton*)GetDlgItem(ID_BTNSTART_TRANS);
	start->EnableWindow(FALSE);
	CButton* stop = (CButton*)GetDlgItem(ID_BTNSTOP_TRANS);
	stop->EnableWindow(TRUE);
}


void CTransportLayer::OnBnClickedBtnstopTrans()
{
	// TODO: Add your control notification handler code here

	isWaitingForTCPInit = true;

	m_runTCPThread = false;
	m_runUDPThread = false;

	CButton* start = (CButton*)GetDlgItem(ID_BTNSTART_TRANS);
	start->EnableWindow(TRUE);
	CButton* stop = (CButton*)GetDlgItem(ID_BTNSTOP_TRANS);
	stop->EnableWindow(FALSE);
}


void CTransportLayer::setMainForm(CDialogEx* me)
{
	m_mainForm = me;
}
