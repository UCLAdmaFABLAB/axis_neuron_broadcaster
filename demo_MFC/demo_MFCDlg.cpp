// demo_MFCDlg.cpp : implementation file
#include "stdafx.h"
#include "demo_MFC.h"
#include "demo_MFCDlg.h"
#include "afxdialogex.h"

#include <iostream>
#include <sstream>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include "tinyosc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
// Load SDK library
#pragma comment(lib, "../NDRLib/NeuronDataReader.lib")//Add Lib

//Load winsock libs
// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

std::size_t bufSize = OSC_BUFFER_SIZE;
char oscBuffer[OSC_BUFFER_SIZE];
struct sockaddr_in sock_addr;
SOCKET udpSocket;
WSADATA wsaData;

// CAboutDlg dialog used for App About
class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();
	// Dialog Data
	enum { IDD = IDD_ABOUTBOX };
protected:
	virtual void DoDataExchange(CDataExchange* pDX); // DDX/DDV support
	// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};
CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{

}
void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}
BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()
// Cdemo_MFCDlg dialog
Cdemo_MFCDlg::Cdemo_MFCDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(Cdemo_MFCDlg::IDD, pParent)
	, m_strIPAddress(_T(""))
	, m_strPort(_T(""))
	, m_strUDPPort(_T(""))
	// Calc data
	, m_calcPx(_T(""))
	, m_calcPy(_T(""))
	, m_calcPz(_T(""))

	, m_calcVx(_T(""))
	, m_calcVy(_T(""))
	, m_calcVz(_T(""))

	, m_calcQs(_T(""))
	, m_calcQx(_T(""))
	, m_calcQy(_T(""))
	, m_calcQz(_T(""))

	, m_clacAx(_T(""))
	, m_calcAy(_T(""))
	, m_calcAz(_T(""))

	, m_calcGx(_T(""))
	, m_calcGy(_T(""))
	, m_calcGz(_T(""))

	, m_bvhAngleZ(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	sockTCPRef = NULL;
	sockUDPRef = NULL;
	fout = ofstream("neurondata.txt");

	if (WSAStartup(MAKEWORD(1, 1), &wsaData) == SOCKET_ERROR) {
		TRACE("Error initialising WSA.\n");
	}
	udpSocket = socket(AF_INET, SOCK_DGRAM, 0);
	TRACE("made socket. result: %d\n", udpSocket);
	
	//char broadcast = '1';
	//if (setsockopt(udpSocket, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast)) < 0) {
	//	TRACE("Error in setting Broadcast option");
	//	closesocket(udpSocket);
	//}

	sock_addr.sin_family = AF_INET;
	hostent* localHost = gethostbyname("");

	char* localIP = inet_ntoa(*(struct in_addr*) * localHost->h_addr_list);
	TRACE("localIP: %s", localIP);
	sock_addr.sin_port = htons(OSC_PORT);
	sock_addr.sin_addr.s_addr = inet_addr(localIP);
	if (bind(udpSocket, (struct sockaddr*) &sock_addr, sizeof(sock_addr)) == -1) {
		TRACE("bind failure. error: %d,\n", WSAGetLastError());
	}

	//if (connect(udpSocket, (struct sockaddr*) &sock_addr, sizeof(sock_addr)) == -1) {
	//	TRACE("connect failure. error: %d,\n", WSAGetLastError());
	//}

	int len = tosc_writeMessage(oscBuffer, bufSize, "/mocap", "s", "hello world");
	if (sendto(udpSocket, oscBuffer, len, 0, (struct sockaddr*) &sock_addr, sizeof(sock_addr)) == -1) {
		TRACE("send failure. error: %d,\n", WSAGetLastError());
	}
}

void Cdemo_MFCDlg::sendBvhBoneInfo(SOCKET_REF sender, BvhDataHeader* header, float* data) {

	int len = tosc_writeMessage(oscBuffer, bufSize, "/mocap", "s", "hello world");
	fd_set readSet;
	FD_ZERO(&readSet);
	FD_SET(udpSocket, &readSet);
	struct timeval timeout = { 1, 0 }; // select times out after 1 second
	if (select(udpSocket + 1, &readSet, NULL, NULL, &timeout) > 0) {
		if (sendto(udpSocket, oscBuffer, len, 0, (struct sockaddr*) & sock_addr, sizeof(sock_addr)) == -1) {
			TRACE("send failure. error: %d,\n", WSAGetLastError());
		}
		TRACE("Sent data\n");
	} else {
		TRACE("Could not select socket \n");
	}

}
void Cdemo_MFCDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_TEXT1, m_strIPAddress);
	DDX_Text(pDX, IDC_TEXT2, m_strPort);
	DDX_Text(pDX, IDC_UDP, m_strUDPPort);
	DDX_Control(pDX, IDC_RADIO_BVH, m_radioBvh);
	/*DDX_Text(pDX, IDC_STATIC_AX, m_clacAx);
	DDX_Text(pDX, IDC_STATIC_AY, m_calcAy);
	DDX_Text(pDX, IDC_STATIC_AZ, m_calcAz);
	DDX_Text(pDX, IDC_STATIC_GX, m_calcGx);
	DDX_Text(pDX, IDC_STATIC_GY, m_calcGy);
	DDX_Text(pDX, IDC_STATIC_GZ, m_calcGz);
	DDX_Text(pDX, IDC_STATIC_PX, m_calcPx);
	DDX_Text(pDX, IDC_STATIC_PY, m_calcPy);
	DDX_Text(pDX, IDC_STATIC_QS, m_calcQs);
	DDX_Text(pDX, IDC_STATIC_QX, m_calcQx);
	DDX_Text(pDX, IDC_STATIC_QY, m_calcQy);
	DDX_Text(pDX, IDC_STATIC_QZ, m_calcQz);
	DDX_Text(pDX, IDC_STATIC_VX, m_calcVx);
	DDX_Text(pDX, IDC_STATIC_VY, m_calcVy);
	DDX_Text(pDX, IDC_STATIC_VZ, m_calcVz);
	DDX_Text(pDX, IDC_STATIC_PZ, m_calcPz);
	*/
	DDX_Control(pDX, IDC_COMBO_SELECTION_BONE_INDEX, m_wndComBoxBone);
	DDX_Text(pDX, IDC_STATIC_ANGLE_Z, m_bvhAngleZ);
}
BEGIN_MESSAGE_MAP(Cdemo_MFCDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &Cdemo_MFCDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &Cdemo_MFCDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_TCP_CONNECTION, &Cdemo_MFCDlg::OnBnClickedButtonTcpConnection)
	ON_BN_CLICKED(IDC_BUTTON_UDP_CONNECTION, &Cdemo_MFCDlg::OnBnClickedButtonUdpConnection)
	ON_CBN_SELCHANGE(IDC_COMBO_SELECTION_BONE_INDEX,
		&Cdemo_MFCDlg::OnCbnSelchangeComboSelectionBoneIndex)
	ON_BN_CLICKED(IDC_RADIO_BVH, &Cdemo_MFCDlg::OnBnClickedRadio)
	ON_BN_CLICKED(IDC_RADIO_CALC, &Cdemo_MFCDlg::OnBnClickedRadio)
	ON_MESSAGE(WM_UPDATE_MESSAGE, OnUpdateMessage)
	ON_STN_CLICKED(IDC_STATIC_DISP_X, &Cdemo_MFCDlg::OnStnClickedStaticDispX)
END_MESSAGE_MAP()
// Cdemo_MFCDlg message handlers
BOOL Cdemo_MFCDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	// Add "About..." menu item to system menu.
	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);
	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}
	// Set the icon for this dialog. The framework does this automatically
	// when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE); // Set big icon
	SetIcon(m_hIcon, FALSE); // Set small icon
	// TODO: Add extra initialization here
	BRRegisterFrameDataCallback(this, bvhFrameDataReceived);
	BRRegisterCalculationDataCallback(this, CalcFrameDataReceive);
	m_strIPAddress = L"127.0.0.1";//Default IP Address
	m_strPort = L"7001";//Default Port
	m_strUDPPort = L"7001"; // Default UDP Port
	m_radioBvh.SetCheck(BST_CHECKED);
	GetDlgItem(IDC_BUTTON_TCP_CONNECTION)->SetWindowText(L"Connect TCP");
	GetDlgItem(IDC_BUTTON_UDP_CONNECTION)->SetWindowText(L"Connect UDP");
	UpdateBvhDataShowUI();
	UpdateData(FALSE);
	return TRUE; // return TRUE unless you set the focus to a control
}
void __stdcall Cdemo_MFCDlg::bvhFrameDataReceived(void* customedObj, SOCKET_REF sender, BvhDataHeader*
	header, float* data)
{
	Cdemo_MFCDlg* pthis = (Cdemo_MFCDlg*)customedObj;
	pthis->showBvhBoneInfo(sender, header, data);
	pthis->saveBvhBoneInfo(sender, header, data);
	pthis->sendBvhBoneInfo(sender, header, data);
}
void __stdcall Cdemo_MFCDlg::CalcFrameDataReceive(void* customObje, SOCKET_REF sender, CalcDataHeader*
	header, float* data)
{
	Cdemo_MFCDlg* pthis = (Cdemo_MFCDlg*)customObje;
	pthis->showCalcBoneInfo(sender, header, data);

}
void Cdemo_MFCDlg::saveBvhBoneInfo(SOCKET_REF sender, BvhDataHeader* header, float* data)
{
	char strFrameIndex[60];
	_itoa_s(header->FrameIndex, strFrameIndex, 10);

	for (int curSel = 0; curSel < BVHBoneCount; curSel++) {
		int dataIndex = 0;

		fout << strFrameIndex << "," << curSel << ",";
		if (curSel == CB_ERR) return;
		if (header->WithDisp)
		{
			dataIndex = curSel * 6;
			if (header->WithReference)
			{
				dataIndex += 6;
			}
			float dispX = data[dataIndex + 0];
			float dispY = data[dataIndex + 1];
			float dispZ = data[dataIndex + 2];
			fout << dispX << "," << dispY << "," << dispZ << ",";

			float angX = data[dataIndex + 4];
			float angY = data[dataIndex + 3];
			float angZ = data[dataIndex + 5];
			fout << angX << "," << angY << "," << angZ << ",";

			fout << "\n";
		}
		else
		{
			if (curSel == 0)
			{
				dataIndex = 0;
				fout << strFrameIndex << "," << curSel << ",";

				if (header->WithReference)
				{
					dataIndex += 6;
				}
				// show hip's displacement

				float dispX = data[dataIndex + 0];
				float dispY = data[dataIndex + 1];
				float dispZ = data[dataIndex + 2];
				fout << dispX << "," << dispY << "," << dispZ << ",";

				float angX = data[dataIndex + 4];
				float angY = data[dataIndex + 3];
				float angZ = data[dataIndex + 5];
				fout << angX << "," << angY << "," << angZ << ",";
			}
			else
			{
				dataIndex = 3 + curSel * 3;
				if (header->WithReference)
				{
					dataIndex += 6;
				}
				fout << strFrameIndex << "," << curSel << ",";

				// show displacement
				fout << ",,,";

				// show angle
				float angX = data[dataIndex + 1];
				float angY = data[dataIndex + 0];
				float angZ = data[dataIndex + 2];
				fout << angX << "," << angY << "," << angZ << ",";
			}
		}
	}
}
void Cdemo_MFCDlg::showBvhBoneInfo(SOCKET_REF sender, BvhDataHeader* header, float* data)
{
	USES_CONVERSION;
	// show frame index
	char strFrameIndex[60];
	_itoa_s(header->FrameIndex, strFrameIndex, 10);
	GetDlgItem(IDC_STATIC_FRAME_INDEX)->SetWindowText(A2W(strFrameIndex));
	// calculate data index for selected bone
	int dataIndex = 0;
	int curSel = m_wndComBoxBone.GetCurSel();
	if (curSel == CB_ERR) return;
	if (header->WithDisp)
	{
		dataIndex = curSel * 6;
		if (header->WithReference)
		{
			dataIndex += 6;
		}
		float dispX = data[dataIndex + 0];
		float dispY = data[dataIndex + 1];
		float dispZ = data[dataIndex + 2];
		char strBuff[32];
		sprintf_s(strBuff, sizeof(strBuff), "%0.3f", dispX);
		GetDlgItem(IDC_STATIC_DISP_X)->SetWindowText(A2W(strBuff));
		sprintf_s(strBuff, sizeof(strBuff), "%0.3f", dispY);
		GetDlgItem(IDC_STATIC_DISP_Y)->SetWindowText(A2W(strBuff));
		sprintf_s(strBuff, sizeof(strBuff), "%0.3f", dispZ);
		GetDlgItem(IDC_STATIC_DISP_Z)->SetWindowText(A2W(strBuff));
		float angX = data[dataIndex + 4];
		float angY = data[dataIndex + 3];
		float angZ = data[dataIndex + 5];
		sprintf_s(strBuff, sizeof(strBuff), "%0.3f", angX);
		GetDlgItem(IDC_STATIC_ANGLE_X)->SetWindowText(A2W(strBuff));
		sprintf_s(strBuff, sizeof(strBuff), "%0.3f", angY);
		GetDlgItem(IDC_STATIC_ANGLE_Y)->SetWindowText(A2W(strBuff));
		sprintf_s(strBuff, sizeof(strBuff), "%0.3f", angZ);
		GetDlgItem(IDC_STATIC_ANGLE_Z)->SetWindowText(A2W(strBuff));
	}
	else
	{
		if (curSel == 0)
		{
			dataIndex = 0;
			if (header->WithReference)
			{
				dataIndex += 6;
			}
			// show hip's displacement
			float dispX = data[dataIndex + 0];
			float dispY = data[dataIndex + 1];
			float dispZ = data[dataIndex + 2];
			char strBuff[32];
			sprintf_s(strBuff, sizeof(strBuff), "%0.3f", dispX);
			GetDlgItem(IDC_STATIC_DISP_X)->SetWindowText(A2W(strBuff));
			sprintf_s(strBuff, sizeof(strBuff), "%0.3f", dispY);
			GetDlgItem(IDC_STATIC_DISP_Y)->SetWindowText(A2W(strBuff));
			sprintf_s(strBuff, sizeof(strBuff), "%0.3f", dispZ);
			GetDlgItem(IDC_STATIC_DISP_Z)->SetWindowText(A2W(strBuff));
			// show hip's angle
			float angX = data[dataIndex + 4];
			float angY = data[dataIndex + 3];
			float angZ = data[dataIndex + 5];
			sprintf_s(strBuff, sizeof(strBuff), "%0.3f", angX);
			//GetDlgItem(IDC_STATIC_ANGLE_X)->SetWindowText(A2W(strBuff));
			sprintf_s(strBuff, sizeof(strBuff), "%0.3f", angY);
			//GetDlgItem(IDC_STATIC_ANGLE_Y)->SetWindowText(A2W(strBuff));
			sprintf_s(strBuff, sizeof(strBuff), "%0.3f", angZ);
			//GetDlgItem(IDC_STATIC_ANGLE_Z)->SetWindowText(A2W(strBuff));
		}
		else
		{
			dataIndex = 3 + curSel * 3;
			if (header->WithReference)
			{
				dataIndex += 6;
			}
			// show displacement
			char strBuff[32];
			sprintf_s(strBuff, sizeof(strBuff), "NaN");
			GetDlgItem(IDC_STATIC_DISP_X)->SetWindowText(A2W(strBuff));
			GetDlgItem(IDC_STATIC_DISP_Y)->SetWindowText(A2W(strBuff));
			GetDlgItem(IDC_STATIC_DISP_Z)->SetWindowText(A2W(strBuff));
			// show angle
			float angX = data[dataIndex + 1];
			float angY = data[dataIndex + 0];
			float angZ = data[dataIndex + 2];
			sprintf_s(strBuff, sizeof(strBuff), "%0.3f", angX);
			GetDlgItem(IDC_STATIC_ANGLE_X)->SetWindowText(A2W(strBuff));
			sprintf_s(strBuff, sizeof(strBuff), "%0.3f", angY);
			GetDlgItem(IDC_STATIC_ANGLE_Y)->SetWindowText(A2W(strBuff));
			sprintf_s(strBuff, sizeof(strBuff), "%0.3f", angZ);
			GetDlgItem(IDC_STATIC_ANGLE_Z)->SetWindowText(A2W(strBuff));
		}
	}
}
LRESULT Cdemo_MFCDlg::OnUpdateMessage(WPARAM wParam, LPARAM lParam)
{
	UpdateData(FALSE);
	return 0;
}
void Cdemo_MFCDlg::showCalcBoneInfo(SOCKET_REF sender, CalcDataHeader* header, float* data)
{
	USES_CONVERSION;
	// show frame index
	char strFrameIndex[60];
	_itoa_s(header->FrameIndex, strFrameIndex, 10);
	GetDlgItem(IDC_STATIC_FRAME_INDEX)->SetWindowText(A2W(strFrameIndex));

	int dataIndex = 0;
	int curSel = m_wndComBoxBone.GetCurSel();
	if (curSel == CB_ERR) return;
	if (curSel > CalcBoneCount) return;
	dataIndex = 16 * curSel;
	//CString tmptmp;
	//tmptmp.Format( L"%d\n", dataIndex );
	//OutputDebugString( tmptmp );
	CString tmpData;
	tmpData.Format(L"%0.3f", data[dataIndex + 0]);
	m_calcPx = tmpData;
	tmpData.Format(L"%0.3f", data[dataIndex + 1]);
	m_calcPy = tmpData;
	tmpData.Format(L"%0.3f", data[dataIndex + 2]);
	m_calcPz = tmpData;
	tmpData.Format(L"%0.3f", data[dataIndex + 3]);
	m_calcVx = tmpData;
	tmpData.Format(L"%0.3f", data[dataIndex + 4]);
	m_calcVy = tmpData;
	tmpData.Format(L"%0.3f", data[dataIndex + 5]);
	m_calcVz = tmpData;
	tmpData.Format(L"%0.3f", data[dataIndex + 6]);
	m_calcQs = tmpData;
	tmpData.Format(L"%0.3f", data[dataIndex + 7]);
	m_calcQx = tmpData;
	tmpData.Format(L"%0.3f", data[dataIndex + 8]);
	m_calcQy = tmpData;
	tmpData.Format(L"%0.3f", data[dataIndex + 9]);
	m_calcQz = tmpData;
	tmpData.Format(L"%0.3f", data[dataIndex + 10]);
	m_clacAx = tmpData;
	tmpData.Format(L"%0.3f", data[dataIndex + 11]);
	m_calcAy = tmpData;
	tmpData.Format(L"%0.3f", data[dataIndex + 12]);
	m_calcAz = tmpData;
	tmpData.Format(L"%0.3f", data[dataIndex + 13]);
	m_calcGx = tmpData;
	tmpData.Format(L"%0.3f", data[dataIndex + 14]);
	m_calcGy = tmpData;
	tmpData.Format(L"%0.3f", data[dataIndex + 15]);
	m_calcGz = tmpData;
	PostMessage(WM_UPDATE_MESSAGE, 0, 0);//OK － UpdateDate

}
void Cdemo_MFCDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}
// If you add a minimize button to your dialog, you will need the code below
// to draw the icon. For MFC applications using the document/view model,
// this is automatically done for you by the framework.
void Cdemo_MFCDlg::OnPaint()
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
// the minimized window.
HCURSOR Cdemo_MFCDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
void Cdemo_MFCDlg::OnBnClickedOk()
{
	//CDialogEx::OnOK();
	fout.close();
}
void Cdemo_MFCDlg::OnBnClickedCancel()
{
	CDialogEx::OnCancel();
	fout.close();
}
void Cdemo_MFCDlg::OnBnClickedButtonTcpConnection()
{
	UpdateData();
	if (sockTCPRef)
	{
		// close socket
		BRCloseSocket(sockTCPRef);
		// reconnect
		sockTCPRef = NULL;
		// change the title of button
		GetDlgItem(IDC_BUTTON_TCP_CONNECTION)->SetWindowText(L"Connect");
	}
	else
	{
		USES_CONVERSION;
		// get port number
		char* port = W2A(m_strPort);
		long nPort = 0;
		try
		{
			nPort = atoi(port);
		}
		catch (CException * e)
		{
			AfxMessageBox(L"Port number error", MB_OK);
			return;
		}
		// connect to remote server
		sockTCPRef = BRConnectTo(W2A(m_strIPAddress), nPort);
		// if success, change the title of button
		if (sockTCPRef)
		{
			GetDlgItem(IDC_BUTTON_TCP_CONNECTION)->SetWindowText(L"Disconnect");
		}
		else
		{
			// if failed, show message
			AfxMessageBox(A2W(BRGetLastErrorMessage()), MB_OK);
		}
	}
}
void Cdemo_MFCDlg::OnBnClickedButtonUdpConnection()
{
	UpdateData();
	if (sockUDPRef)
	{
		// close socket
		BRCloseSocket(sockUDPRef);
		// reconnect
		sockUDPRef = NULL;
		// change the title of button
		GetDlgItem(IDC_BUTTON_UDP_CONNECTION)->SetWindowText(L"Connect");
	}
	else
	{
		USES_CONVERSION;
		// get port number
		char* UDPport = W2A(m_strUDPPort);
		long nUDPPort = 0;
		try
		{
			nUDPPort = atoi(UDPport);
		}
		catch (CException * e)
		{
			AfxMessageBox(L"UDPPort number error", MB_OK);
			return;
		}
		// connect to remote server
		sockUDPRef = BRStartUDPServiceAt(nUDPPort);
		// if success, change the title of button
		if (sockUDPRef)
		{
			GetDlgItem(IDC_BUTTON_UDP_CONNECTION)->SetWindowText(L"Disconnect");
		}
		else
		{
			// if failed, show message
			AfxMessageBox(A2W(BRGetLastErrorMessage()), MB_OK);
		}
	}
}
void Cdemo_MFCDlg::OnCbnSelchangeComboSelectionBoneIndex()
{
	UpdateData();
}
void Cdemo_MFCDlg::OnBnClickedRadio()
{
	if (m_radioBvh.GetCheck() == BST_CHECKED)
	{
		UpdateBvhDataShowUI();
	}
	else
	{
		UpdateCalcDataShowUI();
	}
}
void Cdemo_MFCDlg::UpdateBvhDataShowUI()
{
	CString BoneID;
	m_wndComBoxBone.ResetContent();
	for (int i = 0; i < BVHBoneCount; i++)
	{
		BoneID.Format(L"%s%d", L"Bone", i);
		m_wndComBoxBone.AddString(BoneID);
	}
	m_wndComBoxBone.SetCurSel(0);
}
void Cdemo_MFCDlg::UpdateCalcDataShowUI()
{
	CString BoneID;
	m_wndComBoxBone.ResetContent();
	for (int i = 0; i < CalcBoneCount; i++)
	{
		BoneID.Format(L"%s%d", L"Bone", i);
		m_wndComBoxBone.AddString(BoneID);
	}
	m_wndComBoxBone.SetCurSel(0);
}

void Cdemo_MFCDlg::OnStnClickedStaticDispX()
{
	// TODO: Add your control notification handler code here
}
