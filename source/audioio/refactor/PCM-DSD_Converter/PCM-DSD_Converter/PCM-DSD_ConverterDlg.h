// PCM-DSD_ConverterDlg.h

#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include <string>
#include"ProgressDlg.h"
#include "fftw3.h"
#include <omp.h>
#include <fstream>
#include <complex>
#define _USE_MATH_DEFINES
#include <math.h>
#include <locale.h>

#include <malloc.h>
#include <stdlib.h>

using namespace std;

class CPCMDSD_ConverterDlg : public CDialogEx
{
public:
    CPCMDSD_ConverterDlg(CWnd* pParent = NULL);
    string flag_Bottun = "";
    enum { IDD = IDD_PCMDSD_CONVERTER_DIALOG };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);

private:
    ProgressDlg m_dProgress;
    static UINT __cdecl WorkThread(LPVOID pParam);

protected:
    HICON m_hIcon;
    virtual BOOL OnInitDialog();
    virtual void ListInit();
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    DECLARE_MESSAGE_MAP()

public:
    virtual void OnCancel();
    CMFCListCtrl m_lFileList;
    CButton m_bAllRun;
    CButton m_bAllListDelete;
    CButton m_bRun;
    CButton m_bListDelete;
    CStatic m_scPrecision;
    CComboBox m_ccPrecision;
    CStatic m_scPath;
    CEdit m_ecPath;
    CButton m_bcPath;
    CComboBox m_cSamplingRate;
    CStatic m_sSamplingRate;
    CString m_evPath;
    afx_msg void OnBnClickedAllrun();
    afx_msg void OnBnClickedAlllistdelete();
    afx_msg void OnBnClickedRun();
    afx_msg void OnBnClickedListdelete();
    afx_msg void OnBnClickedPathcheck();
    afx_msg void OnDropFiles(HDROP hDropInfo);
    afx_msg void OnClose();
    afx_msg void OnDestroy();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void WAV_FileRead(TCHAR *FileName);
    afx_msg void DirectoryFind(TCHAR *DirectoryPath);
    afx_msg bool WAV_Metadata(TCHAR *filepath, wstring *metadata);
    afx_msg bool WAV_Convert(TCHAR *filepath, int number);
    afx_msg bool DSD_Write(FILE *LData, FILE *RData, FILE *WriteData, int number);
    afx_msg bool RequireWriteData(TCHAR *filepath, CString flag, wchar_t *FileMode, FILE **WriteDatadsd);
    afx_msg bool TrushFile(TCHAR *filepath, CString flag);
    afx_msg bool TmpWriteData(TCHAR *filepath, FILE *tmpl, FILE *tmpr, int Times);
    afx_msg bool WAV_Filter_Renew(FILE *UpSampleData, FILE *OrigData, unsigned int Times, omp_lock_t *myLock, unsigned int DSDsamplingRate);
    afx_msg bool WAV_FilterLight_Renew(FILE *UpSampleData, FILE *OrigData, unsigned int Times);
    void WorkThread();
    void Disable();
    void Enable();
    afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
    void FFTInit(fftw_plan *plan, unsigned int fftsize, int Times, double *fftin, fftw_complex *ifftout);
    void iFFTInit(fftw_plan *plan, unsigned int fftsize, int Times, fftw_complex *ifftin, double *fftout);
    void PathCheck();
};
