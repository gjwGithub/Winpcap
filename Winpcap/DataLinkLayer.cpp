// DataLinkLayer.cpp : 实现文件
//

#include "stdafx.h"
#include "Winpcap.h"
#include "DataLinkLayer.h"
#include "afxdialogex.h"



// CDataLinkLayer 对话框

IMPLEMENT_DYNAMIC(CDataLinkLayer, CDialog)

CDataLinkLayer::CDataLinkLayer(CWnd* pParent /*=NULL*/)
	: CDialog(CDataLinkLayer::IDD, pParent)
	, m_LengthThread(NULL)
	, m_bpsThread(NULL)
	, m_runLengthThread(false)
	, m_runBPSThread(false)
	, m_totalLen(0)
	, m_column(0)
{
	
}

CDataLinkLayer::~CDataLinkLayer()
{
}

void CDataLinkLayer::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_LINK, m_list);

	m_list.SetExtendedStyle(m_list.GetExtendedStyle()
		| LVS_EX_GRIDLINES
		| LVS_EX_FULLROWSELECT);
	m_list.InsertColumn(0, _T("Time"));
	m_list.InsertColumn(1, _T("Form MAC"));
	m_list.InsertColumn(2, _T("To MAC"));
	m_list.InsertColumn(3, _T("Type"));
	m_list.InsertColumn(4, _T("Length"));

	CRect rect;
	m_list.GetClientRect(rect); //获得当前客户区信息
	int columnCount = 5;
	for (int i = 0;i < columnCount;i++)
	{
		m_list.SetColumnWidth(i, rect.Width() / columnCount); //设置列的宽度。
	}

	m_deviceList = (CListBox*)m_mainForm->GetDlgItem(IDC_LIST);
}


BEGIN_MESSAGE_MAP(CDataLinkLayer, CDialog)
	ON_BN_CLICKED(ID_BTNSTART_LINK, &CDataLinkLayer::OnBnClickedBtnstartLink)
	ON_BN_CLICKED(ID_BTNSTOP_LINK, &CDataLinkLayer::OnBnClickedBtnstopLink)
END_MESSAGE_MAP()


// CDataLinkLayer 消息处理程序

UINT LengthThreadFunc(LPVOID lpParam)
{
	CDataLinkLayer* me = (CDataLinkLayer*)lpParam;

	/* Retrieve the packets */
	while ((me->res = pcap_next_ex(me->adhandle, &me->header, &me->pkt_data)) >= 0 && me->m_runLengthThread) {

		if (me->res == 0)
			/* Timeout elapsed */
			continue;

		/* convert the timestamp to readable format */
		me->local_tv_sec = me->header->ts.tv_sec;
		localtime_s(&me->ltime, &me->local_tv_sec);
		strftime(me->timestr, sizeof me->timestr, "%H:%M:%S", &me->ltime);

		CString time;
		time += me->timestr;
		time += ",";
		time.AppendFormat(L"%.6d", me->header->ts.tv_usec);
		CString toMac;
		toMac.Format(L"%02X-%02X-%02X-%02X-%02X-%02X", (int)me->pkt_data[0], (int)me->pkt_data[1], (int)me->pkt_data[2], (int)me->pkt_data[3], (int)me->pkt_data[4], (int)me->pkt_data[5]);
		CString fromMac;
		fromMac.Format(L"%02X-%02X-%02X-%02X-%02X-%02X", (int)me->pkt_data[6], (int)me->pkt_data[7], (int)me->pkt_data[8], (int)me->pkt_data[9], (int)me->pkt_data[10], (int)me->pkt_data[11]);
		CString type;
		type.Format(L"%02X%02X", me->pkt_data[12], me->pkt_data[13]);
		if (type == L"8137")
			type = L"Netware";
		else if (type == L"0600" || type == L"0807")
			type = L"XNS";
		else if (type == L"0800")
			type = L"IP";
		else if (type == L"0806")
			type = L"ARP";
		else if (type == L"8035")
			type = L"RARP";
		else if (type == L"6003")
			type = L"DRP";
		else if (type == L"6004")
			type = L"LAT";
		else if (type == L"6007")
			type = L"LAVC";
		else if (type == L"86DD")
			type = L"IPv6";
		else if (type == L"880B")
			type = L"PPP";
		else if (type == L"814C")
			type = L"SNMP";
		CString length;
		length.Format(L"%d", me->header->len);
		me->m_list.InsertItem(me->m_column, time);
		me->m_list.SetItemText(me->m_column, 1, fromMac);
		me->m_list.SetItemText(me->m_column, 2, toMac);
		me->m_list.SetItemText(me->m_column, 3, type);
		me->m_list.SetItemText(me->m_column, 4, length);
		me->m_list.EnsureVisible(me->m_column, FALSE);

		me->m_totalLen += me->header->len;
		CString strTotalLen;
		strTotalLen.Format(L"Length count: %d bytes", me->m_totalLen);
		me->SetDlgItemTextW(IDC_STATIC_LENGTH, strTotalLen);

		me->m_column++;
	}

	if (me->res == -1) {
		CString string;
		string.Format(L"Error reading the packets: %s\n", pcap_geterr(me->adhandle));
		AfxMessageBox(string);
		return -1;
	}
	return 0;
}

UINT BPSThreadFunc(LPVOID lpParam)
{
	CDataLinkLayer* me = (CDataLinkLayer*)lpParam;

	pcap_if_t *alldevs;
	pcap_if_t *d;
	int inum;
	int i = 0;
	pcap_t *adhandle;
	int res;
	char errbuf[PCAP_ERRBUF_SIZE];
	struct pcap_pkthdr *header;
	const u_char *pkt_data;

	/* Retrieve the device list on the local machine */
	if (pcap_findalldevs_ex(PCAP_SRC_IF_STRING, NULL, &alldevs, errbuf) == -1)
	{
		CString string;
		string.Format(L"Error in pcap_findalldevs: %s\n", errbuf);
		AfxMessageBox(string);
		return 0;
	}

	/* Print the list */
	for (d = alldevs; d; d = d->next)
	{
		++i;
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
	

	//if (inum < 1 || inum > i)
	//{
	//	printf("\nInterface number out of range.\n");
	//	/* Free the device list */
	//	pcap_freealldevs(alldevs);
	//	return;
	//}

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




	//printf("\nlistening on %s...\n", d->description);


	/* Don't care about netmask, it won't be used for this filter */
	//u_int netmask = 0xffffff;


	/* Put the interface in statstics mode */
	if (pcap_setmode(adhandle, MODE_STAT)<0)
	{
		CString string;
		string.Format(L"\nError setting the mode.\n");
		AfxMessageBox(string);
		pcap_close(adhandle);
		/* Free the device list */
		return 0;
	}

	/* At this point, we don't need any more the device list. Free it */
	pcap_freealldevs(alldevs);

	struct timeval old_ts;
	old_ts.tv_sec = 0;
	old_ts.tv_usec = 0;

	while ((res = pcap_next_ex(adhandle, &header, &pkt_data)) >= 0 && me->m_runBPSThread) {

		if (res == 0)
			/* Timeout elapsed */
			continue;

		u_int delay;
		LARGE_INTEGER Bps;
		delay = (header->ts.tv_sec - old_ts.tv_sec) * 1000000 - old_ts.tv_usec + header->ts.tv_usec;
		/* Get the number of Bits per second */
		Bps.QuadPart = (((*(LONGLONG*)(pkt_data + 8)) * 8 * 1000000) / (delay));
		old_ts = header->ts;
		CString strTotalLen;
		strTotalLen.Format(L"Byte per second: %I64u bps", Bps.QuadPart);
		me->SetDlgItemTextW(IDC_STATIC_BPS, strTotalLen);
	}

	return 0;
}

void CDataLinkLayer::OnBnClickedBtnstartLink()
{
	// TODO:  在此添加控件通知处理程序代码
	
	
	/* Retrieve the device list on the local machine */
	if (pcap_findalldevs_ex(PCAP_SRC_IF_STRING, NULL, &alldevs, errbuf) == -1)
	{
		CString string;
		string.Format(L"Error in pcap_findalldevs: %s\n", errbuf);
		MessageBox(string, L"ERROR");
		return;
	}

	/* Print the list */
	for (d = alldevs; d; d = d->next)
	{
		++i;
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
		MessageBox(string, L"ERROR");
		return;
	}

	//printf("Enter the interface number (1-%d):", i);
	//scanf_s("%d", &inum);
	int index;
	index = m_deviceList->GetCurSel();
	if (index == -1)
	{
		AfxMessageBox(L"Must select a device!");
		return;
	}
	else
	{
		inum = index;
	}

	//if (inum < 1 || inum > i)
	//{
	//	printf("\nInterface number out of range.\n");
	//	/* Free the device list */
	//	pcap_freealldevs(alldevs);
	//	return;
	//}

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
		MessageBox(string, L"ERROR");
		/* Free the device list */
		pcap_freealldevs(alldevs);
		return ;
	}

	//printf("\nlistening on %s...\n", d->description);

	/* At this point, we don't need any more the device list. Free it */
	pcap_freealldevs(alldevs);

	struct timeval old_ts;
	old_ts.tv_sec = 0;
	old_ts.tv_usec = 0;

	m_runLengthThread = true;
	m_LengthThread = AfxBeginThread(LengthThreadFunc, (LPVOID)this);
	m_runBPSThread = true;
	m_bpsThread = AfxBeginThread(BPSThreadFunc, (LPVOID)this);
}



void CDataLinkLayer::OnBnClickedBtnstopLink()
{
	// TODO:  在此添加控件通知处理程序代码
	m_runLengthThread = false;
	m_runBPSThread = false;
}


void CDataLinkLayer::setMainForm(CDialogEx* me)
{
	m_mainForm = me;
}
