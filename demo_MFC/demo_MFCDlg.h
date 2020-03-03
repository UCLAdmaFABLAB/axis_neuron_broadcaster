// demo_MFCDlg.h : header file
//
#pragma once
// Include the NeuronDataReader head file
#include "../NDRLib/NeuronDataReader.h"
#include "afxwin.h"
#include "afxdialogex.h"
#include <fstream>
#include <ctime>
#include "tinyosc.h"
using namespace std;

struct mocap_bone_t {
    int frameIndex;
    int sensor;
    float dispX;
    float dispY;
    float dispZ;
    float angX;
    float angY;
    float angZ;
    float calcAx;
    float calcAy;
    float calcAz;
    float calcPx;
    float calcPy;
    float calcPz;
    float calcVx;
    float calcVy;
    float calcVz;
    float calcQs;
    float calcQx;
    float calcQy;
    float calcQz;
    float calcGx;
    float calcGy;
    float calcGz;
};

#define OSC_BUFFER_SIZE 4096
#define OSC_PORT 8000
#define WM_UPDATE_MESSAGE (WM_USER+200)
// Cdemo_MFCDlg dialog
class Cdemo_MFCDlg : public CDialogEx
{
    // Construction
public:
    Cdemo_MFCDlg(CWnd* pParent = NULL); // standard constructor
    enum
    {
        BVHBoneCount = 59,
        CalcBoneCount = 21,
    };
    // Dialog Data
    enum { IDD = IDD_DEMO_MFC_DIALOG };
    ofstream fout;
    mocap_bone_t* curFrame;
    const double dataDelayS = 0.5;
    std::time_t lastCalcDataSent = std::time(nullptr);
    std::time_t lastBvhDataSent = std::time(nullptr);

protected:
    virtual void DoDataExchange(CDataExchange* pDX); // DDX/DDV support
    static void __stdcall bvhFrameDataReceived(void* customedObj, SOCKET_REF sender, BvhDataHeader* header,
        float* data);
    // Receive calculation data
    static void __stdcall CalcFrameDataReceive(void* customObje, SOCKET_REF sender, CalcDataHeader* header,
        float* data);
    // Implementation
protected:
    HICON m_hIcon;
    SOCKET_REF sockTCPRef;
    SOCKET_REF sockTCPCalc;
    SOCKET_REF sockUDPRef;
    void sendOscBuffer(char* buf, int len);
    void getBvhBoneInfo(SOCKET_REF sender, BvhDataHeader* header, float* data);
    void showBvhBoneInfo();
    void sendBvhBoneInfo();
    void showCalcBoneInfo();
    void getCalcBoneInfo(SOCKET_REF sender, CalcDataHeader* header, float* data);
    void sendCalcBoneInfo();
    // Generated message map functions
    virtual BOOL OnInitDialog();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    afx_msg LRESULT OnUpdateMessage(WPARAM wParam, LPARAM lParam);
    DECLARE_MESSAGE_MAP()
public:
    CComboBox m_wndComBoxBone;
    CString m_strIPAddress;
    CString m_strPort;
    CString m_strPortCalc;
    afx_msg void OnBnClickedButtonTcpConnection();
    afx_msg void OnBnClickedButtonTcpConnectionCalc();
    afx_msg void OnCbnSelchangeComboSelectionBoneIndex();
    void UpdateBvhDataShowUI();
    void UpdateCalcDataShowUI();
};