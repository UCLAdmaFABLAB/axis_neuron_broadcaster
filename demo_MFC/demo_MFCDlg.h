// demo_MFCDlg.h : header file
//
#pragma once
// Include the NeuronDataReader head file
#include "../NDRLib/NeuronDataReader.h"
#include "afxwin.h"
#include "afxdialogex.h"
#include <fstream>
using namespace std;


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
    SOCKET_REF sockUDPRef;
    void showBvhBoneInfo(SOCKET_REF sender, BvhDataHeader* header, float* data);
    void saveBvhBoneInfo(SOCKET_REF sender, BvhDataHeader* header, float* data);
    void showCalcBoneInfo(SOCKET_REF sender, CalcDataHeader* header, float* data);
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
    CString m_strUDPPort;
    afx_msg void OnBnClickedOk();
    afx_msg void OnBnClickedCancel();
    afx_msg void OnBnClickedButtonTcpConnection();
    afx_msg void OnBnClickedButtonUdpConnection();
    afx_msg void OnCbnSelchangeComboSelectionBoneIndex();
    CButton m_radioBvh;
    afx_msg void OnBnClickedRadio();
    void UpdateBvhDataShowUI();
    void UpdateCalcDataShowUI();
    CString m_clacAx;
    CString m_calcAy;
    CString m_calcAz;
    CString m_calcGx;
    CString m_calcGy;
    CString m_calcGz;
    CString m_calcPx;
    CString m_calcPy;
    CString m_calcQs;
    CString m_calcQx;
    CString m_calcQy;
    CString m_calcQz;
    CString m_calcVx;
    CString m_calcVy;
    CString m_calcVz;
    CString m_calcPz;
    CString m_bvhAngleZ;
    afx_msg void OnStnClickedStaticDispX();
};