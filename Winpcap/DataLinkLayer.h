#pragma once


// CDataLinkLayer 对话框
#include "pcap.h"
#include "afxcmn.h"

class CDataLinkLayer : public CDialog
{
	DECLARE_DYNAMIC(CDataLinkLayer)

public:
	CDataLinkLayer(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDataLinkLayer();

// 对话框数据
	enum { IDD = IDD_DATALINKLAYER };

	CWinThread* m_LengthThread;
	CWinThread* m_bpsThread;

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

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnstartLink();
	CListCtrl m_list;
	bool m_runLengthThread;
	bool m_runBPSThread;
	afx_msg void OnBnClickedBtnstopLink();
	unsigned int m_totalLen;
	int m_column;
};

