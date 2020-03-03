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
#include <ctime>

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
char oscBufferCalc[OSC_BUFFER_SIZE];
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
	, m_strPortCalc(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	sockTCPRef = NULL;
	sockUDPRef = NULL;
	fout = ofstream("neurondata.txt");
	curFrame = new mocap_bone_t[BVHBoneCount];

	if (WSAStartup(MAKEWORD(1, 1), &wsaData) == SOCKET_ERROR) {
		TRACE("Error initialising WSA.\n");
	}
	udpSocket = socket(AF_INET, SOCK_DGRAM, 0);
	TRACE("made socket. result: %d\n", udpSocket);
	
	char broadcast = '1';
	if (setsockopt(udpSocket, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast)) < 0) {
		TRACE("Error in setting Broadcast option");
		closesocket(udpSocket);
	}

	sock_addr.sin_family = AF_INET;
	hostent* localHost = gethostbyname("");

	char* localIP = inet_ntoa(*(struct in_addr*) * localHost->h_addr_list);
	TRACE("localIP: %s", localIP);
	sock_addr.sin_port = htons(OSC_PORT);
	sock_addr.sin_addr.s_addr = inet_addr("255.255.255.255");

	int len = tosc_writeMessage(oscBuffer, bufSize, "/mocap", "s", "hello world");
	if (sendto(udpSocket, oscBuffer, len, 0, (struct sockaddr*) &sock_addr, sizeof(sock_addr)) == -1) {
		TRACE("send failure. error: %d,\n", WSAGetLastError());
	}
}


void Cdemo_MFCDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_TEXT1, m_strIPAddress);
	DDX_Text(pDX, IDC_TEXT2, m_strPort);
	DDX_Text(pDX, IDC_CALC_PORT, m_strPortCalc);
	
	DDX_Control(pDX, IDC_COMBO_SELECTION_BONE_INDEX, m_wndComBoxBone);
	
}
BEGIN_MESSAGE_MAP(Cdemo_MFCDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_TCP_CONNECTION, &Cdemo_MFCDlg::OnBnClickedButtonTcpConnection)
	ON_CBN_SELCHANGE(IDC_COMBO_SELECTION_BONE_INDEX,
		&Cdemo_MFCDlg::OnCbnSelchangeComboSelectionBoneIndex)
	ON_MESSAGE(WM_UPDATE_MESSAGE, OnUpdateMessage)
	ON_BN_CLICKED(IDC_BUTTON_TCP_CONNECTION_CALC, &Cdemo_MFCDlg::OnBnClickedButtonTcpConnectionCalc)
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
	m_strPortCalc = L"7003";
	GetDlgItem(IDC_BUTTON_TCP_CONNECTION)->SetWindowText(L"Connect TCP");
	GetDlgItem(IDC_BUTTON_TCP_CONNECTION_CALC)->SetWindowText(L"Connect Calc");
	UpdateBvhDataShowUI();
	UpdateData(FALSE);
	return TRUE; // return TRUE unless you set the focus to a control
}
void __stdcall Cdemo_MFCDlg::bvhFrameDataReceived(void* customedObj, SOCKET_REF sender, BvhDataHeader*
	header, float* data)
{
	Cdemo_MFCDlg* pthis = (Cdemo_MFCDlg*)customedObj;
	pthis->getBvhBoneInfo(sender, header, data);
	pthis->showBvhBoneInfo();
	pthis->sendBvhBoneInfo();
}
void __stdcall Cdemo_MFCDlg::CalcFrameDataReceive(void* customObje, SOCKET_REF sender, CalcDataHeader*
	header, float* data)
{
	Cdemo_MFCDlg* pthis = (Cdemo_MFCDlg*)customObje;
	pthis->getCalcBoneInfo(sender, header, data);
	pthis->sendCalcBoneInfo();
	pthis->showCalcBoneInfo();

}
void Cdemo_MFCDlg::getBvhBoneInfo(SOCKET_REF sender, BvhDataHeader* header, float* data) {
	mocap_bone_t* boneData;
	for (int curSel = 0; curSel < BVHBoneCount; curSel++) {
		boneData = &curFrame[curSel];
		int dataIndex = 0;
		boneData->frameIndex = header->FrameIndex;
		if (curSel == CB_ERR) return;
		if (header->WithDisp)
		{
			dataIndex = curSel * 6;
			if (header->WithReference)
			{
				dataIndex += 6;
			}
			boneData->dispX = data[dataIndex + 0];
			boneData->dispY = data[dataIndex + 1];
			boneData->dispZ = data[dataIndex + 2];
			boneData->angX = data[dataIndex + 4];
			boneData->angY = data[dataIndex + 3];
			boneData->angZ = data[dataIndex + 5];
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
				boneData->dispX = data[dataIndex + 0];
				boneData->dispY = data[dataIndex + 1];
				boneData->dispZ = data[dataIndex + 2];
				boneData->angX = data[dataIndex + 4];
				boneData->angY = data[dataIndex + 3];
				boneData->angZ = data[dataIndex + 5];
			}
			else
			{
				dataIndex = 3 + curSel * 3;
				if (header->WithReference)
				{
					dataIndex += 6;
				}
				boneData->dispX = NULL;
				boneData->dispY = NULL;
				boneData->dispZ = NULL;
				boneData->angX = data[dataIndex + 1];
				boneData->angY = data[dataIndex + 0];
				boneData->angZ = data[dataIndex + 2];
			}
		}
	}

}
void Cdemo_MFCDlg::showBvhBoneInfo()
{
	USES_CONVERSION;
	// show frame index

	// calculate data index for selected bone
	int dataIndex = 0;
	int curSel = m_wndComBoxBone.GetCurSel();
	if (curSel == CB_ERR) return;
	mocap_bone_t* cur = &curFrame[curSel];
	char strFrameIndex[60];
	_itoa_s(cur->frameIndex, strFrameIndex, 10);
	GetDlgItem(IDC_STATIC_FRAME_INDEX)->SetWindowText(A2W(strFrameIndex));
	dataIndex = curSel * 6;
	char strBuff[32];
	sprintf_s(strBuff, sizeof(strBuff), "%0.3f", cur->dispX);
	GetDlgItem(IDC_STATIC_DISP_X)->SetWindowText(A2W(strBuff));
	sprintf_s(strBuff, sizeof(strBuff), "%0.3f", cur->dispY);
	GetDlgItem(IDC_STATIC_DISP_Y)->SetWindowText(A2W(strBuff));
	sprintf_s(strBuff, sizeof(strBuff), "%0.3f", cur->dispZ);
	GetDlgItem(IDC_STATIC_DISP_Z)->SetWindowText(A2W(strBuff));

	sprintf_s(strBuff, sizeof(strBuff), "%0.3f", cur->angX);
	GetDlgItem(IDC_STATIC_ANGLE_X)->SetWindowText(A2W(strBuff));
	sprintf_s(strBuff, sizeof(strBuff), "%0.3f", cur->angY);
	GetDlgItem(IDC_STATIC_ANGLE_Y)->SetWindowText(A2W(strBuff));
	sprintf_s(strBuff, sizeof(strBuff), "%0.3f", cur->angZ);
	GetDlgItem(IDC_STATIC_ANGLE_Z)->SetWindowText(A2W(strBuff));
}
void Cdemo_MFCDlg::sendOscBuffer(char* buf, int len) {
	if (sendto(udpSocket, buf, len, 0, (struct sockaddr*) & sock_addr, sizeof(sock_addr)) == -1) {
		TRACE("send failure. error: %d,\n", WSAGetLastError());
	}
}
void Cdemo_MFCDlg::sendBvhBoneInfo() {
	if ((double)(std::time(nullptr) - lastBvhDataSent) < dataDelayS) {
		return;
	}
	else {
		lastBvhDataSent = std::time(nullptr);
	}
	const uint16_t addressSize = 256;
	char addressBuf[addressSize];
	int len;
	mocap_bone_t* cur;
	for (int boneIndex = 0; boneIndex < BVHBoneCount; boneIndex++) {
		cur = &curFrame[boneIndex];
		sprintf_s(addressBuf, addressSize, "/b/%d/f", boneIndex);
		len = tosc_writeMessage(oscBuffer, OSC_BUFFER_SIZE, addressBuf, "i", cur->frameIndex);
		sendOscBuffer(oscBuffer, len);

		sprintf_s(addressBuf, addressSize, "/b/%d/d", boneIndex);
		len = tosc_writeMessage(oscBuffer, OSC_BUFFER_SIZE, addressBuf, "fff", cur->dispX, cur->dispY, cur->dispZ);
		sendOscBuffer(oscBuffer, len);

		sprintf_s(addressBuf, addressSize, "/b/%d/a", boneIndex);
		len = tosc_writeMessage(oscBuffer, OSC_BUFFER_SIZE, addressBuf, "fff", cur->angX, cur->angY, cur->angZ);
		sendOscBuffer(oscBuffer, len);
	}
}

LRESULT Cdemo_MFCDlg::OnUpdateMessage(WPARAM wParam, LPARAM lParam)
{
	UpdateData(FALSE);
	return 0;
}
void Cdemo_MFCDlg::getCalcBoneInfo(SOCKET_REF sender, CalcDataHeader* header, float* data)
{

	mocap_bone_t* boneData;
	for (int curSel = 0; curSel < CalcBoneCount; curSel++) {
		boneData = &curFrame[curSel];
		int dataIndex = 0;
		if (curSel == CB_ERR) return;
		dataIndex = 16 * curSel;
		boneData->calcPx = data[dataIndex + 0];
		boneData->calcPy = data[dataIndex + 1];
		boneData->calcPz = data[dataIndex + 2];
		boneData->calcVx = data[dataIndex + 3];
		boneData->calcVy = data[dataIndex + 4];
		boneData->calcVz = data[dataIndex + 5];
		boneData->calcQs = data[dataIndex + 6];
		boneData->calcQx = data[dataIndex + 7];
		boneData->calcQy = data[dataIndex + 8];
		boneData->calcQz = data[dataIndex + 9];
		boneData->calcAx = data[dataIndex + 10];
		boneData->calcAy = data[dataIndex + 11];
		boneData->calcAz = data[dataIndex + 12];
		boneData->calcGx = data[dataIndex + 13];
		boneData->calcGy = data[dataIndex + 14];
		boneData->calcGz = data[dataIndex + 15];
	}
}
void Cdemo_MFCDlg::sendCalcBoneInfo() {
	if ((double) (std::time(nullptr) - lastCalcDataSent) < dataDelayS) {
		return;
	} else {
		lastCalcDataSent = std::time(nullptr);
	}
	const uint16_t addressSize = 256;
	char addressBuf[addressSize];
	int len;
	mocap_bone_t* cur;
	
	for (int boneIndex = 0; boneIndex < CalcBoneCount; boneIndex++) {
		cur = &curFrame[boneIndex];
		
		sprintf_s(addressBuf, addressSize, "/b/%d/p", boneIndex);
		len = tosc_writeMessage(oscBufferCalc, OSC_BUFFER_SIZE, addressBuf, "fff", cur->calcPx, cur->calcPy, cur->calcPz);
		sendOscBuffer(oscBufferCalc, len);
		sprintf_s(addressBuf, addressSize, "/b/%d/v", boneIndex);
		len = tosc_writeMessage(oscBufferCalc, OSC_BUFFER_SIZE, addressBuf, "fff", cur->calcVx, cur->calcVy, cur->calcVz);
		sendOscBuffer(oscBufferCalc, len);
		sprintf_s(addressBuf, addressSize, "/b/%d/A", boneIndex);
		len = tosc_writeMessage(oscBufferCalc, OSC_BUFFER_SIZE, addressBuf, "fff", cur->calcAx, cur->calcAy, cur->calcAz);
		sendOscBuffer(oscBufferCalc, len);
	}
}
void Cdemo_MFCDlg::showCalcBoneInfo()
{
	USES_CONVERSION;
	// show frame index
	char strFrameIndex[60];
	GetDlgItem(IDC_STATIC_FRAME_INDEX)->SetWindowText(A2W(strFrameIndex));
	int curSel = m_wndComBoxBone.GetCurSel();
	if (curSel == CB_ERR) return;
	mocap_bone_t* cur = &curFrame[curSel];

	char strBuff[32];
	sprintf_s(strBuff, sizeof(strBuff), "%0.3f", cur->calcPx);
	GetDlgItem(IDC_STATIC_PX)->SetWindowText(A2W(strBuff));
	sprintf_s(strBuff, sizeof(strBuff), "%0.3f", cur->calcPy);
	GetDlgItem(IDC_STATIC_PY)->SetWindowText(A2W(strBuff));
	sprintf_s(strBuff, sizeof(strBuff), "%0.3f", cur->calcPz);
	GetDlgItem(IDC_STATIC_PZ)->SetWindowText(A2W(strBuff));
	sprintf_s(strBuff, sizeof(strBuff), "%0.3f", cur->calcAx);
	GetDlgItem(IDC_STATIC_AX)->SetWindowText(A2W(strBuff));
	sprintf_s(strBuff, sizeof(strBuff), "%0.3f", cur->calcAy);
	GetDlgItem(IDC_STATIC_AY)->SetWindowText(A2W(strBuff));
	sprintf_s(strBuff, sizeof(strBuff), "%0.3f", cur->calcAz);
	GetDlgItem(IDC_STATIC_AZ)->SetWindowText(A2W(strBuff));
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
		GetDlgItem(IDC_BUTTON_TCP_CONNECTION)->SetWindowText(L"Connect BVH");
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
			GetDlgItem(IDC_BUTTON_TCP_CONNECTION)->SetWindowText(L"Disconnect BVH");
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

void Cdemo_MFCDlg::OnBnClickedButtonTcpConnectionCalc()
{
	UpdateData();
	if (sockTCPCalc)
	{
		// close socket
		BRCloseSocket(sockTCPCalc);
		// reconnect
		sockTCPRef = NULL;
		// change the title of button
		GetDlgItem(IDC_BUTTON_TCP_CONNECTION_CALC)->SetWindowText(L"Connect Calc");
	}
	else
	{
		USES_CONVERSION;
		// get port number
		char* port = W2A(m_strPortCalc);
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
			GetDlgItem(IDC_BUTTON_TCP_CONNECTION_CALC)->SetWindowText(L"Disconnect Calc");
		}
		else
		{
			// if failed, show message
			AfxMessageBox(A2W(BRGetLastErrorMessage()), MB_OK);
		}
	}
}
