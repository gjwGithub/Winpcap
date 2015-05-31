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
	, m_IPv4Thread(NULL)
	, m_IPv6Thread(NULL)
	, m_ARPThread(NULL)
	, m_runIPv4Thread(false)
	, m_runIPv6Thread(false)
	, m_runARPThread(false)
	, m_column(0)
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

UINT IPv4ThreadFunc(LPVOID lpParam)
{
	CNetworkLayer* me = (CNetworkLayer*)lpParam;

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
	for (d = alldevs, i = 0; i< inum - 1;d = d->next, i++);

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
	if (pcap_compile(adhandle, &fcode, "ip", 1, netmask) <0)
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

	while ((res = pcap_next_ex(adhandle, &header, &pkt_data)) >= 0 && me->m_runIPv4Thread) {

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
		CString toIP;
		toIP.Format(L"%d.%d.%d.%d", (int)pkt_data[16], (int)pkt_data[17], (int)pkt_data[18], (int)pkt_data[19]);
		CString fromIP;
		fromIP.Format(L"%d.%d.%d.%d", (int)pkt_data[12], (int)pkt_data[13], (int)pkt_data[14], (int)pkt_data[15]);
		CString TTL;
		TTL.Format(L"%d", (int)pkt_data[8]);
		CString type;
		type.Format(L"IPv4");
		CString length;
		length.Format(L"%d", header->len);
		me->m_list.InsertItem(me->m_column, time);
		me->m_list.SetItemText(me->m_column, 1, fromIP);
		me->m_list.SetItemText(me->m_column, 2, toIP);
		me->m_list.SetItemText(me->m_column, 3, TTL);
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

UINT IPv6ThreadFunc(LPVOID lpParam)
{
	CNetworkLayer* me = (CNetworkLayer*)lpParam;

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
	for (d = alldevs, i = 0; i< inum - 1;d = d->next, i++);

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
	if (pcap_compile(adhandle, &fcode, "ip6", 1, netmask) <0)
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

	while ((res = pcap_next_ex(adhandle, &header, &pkt_data)) >= 0 && me->m_runIPv6Thread) {

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
		CString toIP;
		toIP.Format(L"%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X", 
			(int)pkt_data[24], (int)pkt_data[25], (int)pkt_data[26], (int)pkt_data[27], 
			(int)pkt_data[28], (int)pkt_data[29], (int)pkt_data[30], (int)pkt_data[31], 
			(int)pkt_data[32], (int)pkt_data[33], (int)pkt_data[34], (int)pkt_data[35], 
			(int)pkt_data[36], (int)pkt_data[37], (int)pkt_data[38], (int)pkt_data[39]);
		CString fromIP;
		fromIP.Format(L"%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X",
			(int)pkt_data[8], (int)pkt_data[9], (int)pkt_data[10], (int)pkt_data[11],
			(int)pkt_data[12], (int)pkt_data[13], (int)pkt_data[14], (int)pkt_data[15],
			(int)pkt_data[16], (int)pkt_data[17], (int)pkt_data[18], (int)pkt_data[19],
			(int)pkt_data[20], (int)pkt_data[21], (int)pkt_data[22], (int)pkt_data[23]);
		CString TTL;
		TTL.Format(L"%d", (int)pkt_data[7]);
		CString type;
		type.Format(L"IPv6");
		CString length;
		length.Format(L"%d", header->len);
		me->m_list.InsertItem(me->m_column, time);
		me->m_list.SetItemText(me->m_column, 1, fromIP);
		me->m_list.SetItemText(me->m_column, 2, toIP);
		me->m_list.SetItemText(me->m_column, 3, TTL);
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

UINT ARPThreadFunc(LPVOID lpParam)
{
	CNetworkLayer* me = (CNetworkLayer*)lpParam;

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
	for (d = alldevs, i = 0; i< inum - 1;d = d->next, i++);

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
	if (pcap_compile(adhandle, &fcode, "ip6", 1, netmask) <0)
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

	while ((res = pcap_next_ex(adhandle, &header, &pkt_data)) >= 0 && me->m_runIPv6Thread) {

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
		CString toIP;
		toIP.Format(L"%d.%d.%d.%d", (int)pkt_data[24], (int)pkt_data[25], (int)pkt_data[26], (int)pkt_data[27]);
		CString fromIP;
		fromIP.Format(L"%d.%d.%d.%d",(int)pkt_data[14], (int)pkt_data[15],(int)pkt_data[16], (int)pkt_data[17]);
		CString TTL;
		TTL.Format(L"N.A.");
		CString type;
		type.Format(L"ARP");
		CString length;
		length.Format(L"%d", header->len);
		me->m_list.InsertItem(me->m_column, time);
		me->m_list.SetItemText(me->m_column, 1, fromIP);
		me->m_list.SetItemText(me->m_column, 2, toIP);
		me->m_list.SetItemText(me->m_column, 3, TTL);
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

void CNetworkLayer::OnBnClickedBtnstartNet()
{
	// TODO: Add your control notification handler code here
	int index;
	index = m_deviceList->GetCurSel();
	if (index == -1)
	{
		AfxMessageBox(L"Must select a device!");
		return;
	}

	m_runIPv4Thread = true;
	m_IPv4Thread = AfxBeginThread(IPv4ThreadFunc, (LPVOID)this);
	m_runIPv6Thread = true;
	m_IPv6Thread = AfxBeginThread(IPv6ThreadFunc, (LPVOID)this);
	m_runARPThread = true;
	m_ARPThread = AfxBeginThread(ARPThreadFunc, (LPVOID)this);

	CButton* start = (CButton*)GetDlgItem(ID_BTNSTART_NET);
	start->EnableWindow(FALSE);
	CButton* stop = (CButton*)GetDlgItem(ID_BTNSTOP_NET);
	stop->EnableWindow(TRUE);
}


void CNetworkLayer::OnBnClickedBtnstopNet()
{
	// TODO: Add your control notification handler code here

	m_runIPv4Thread = false;
	m_runIPv6Thread = false;
	m_runARPThread = false;

	CButton* start = (CButton*)GetDlgItem(ID_BTNSTART_NET);
	start->EnableWindow(TRUE);
	CButton* stop = (CButton*)GetDlgItem(ID_BTNSTOP_NET);
	stop->EnableWindow(FALSE);
}

void CNetworkLayer::setMainForm(CDialogEx* me)
{
	m_mainForm = me;
}
