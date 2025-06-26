// PCM-DSD_Converter.h

#pragma once

#ifndef __AFXWIN_H__
#error "PCH 'stdafx.h'"
#endif

#include "resource.h"

class CPCMDSD_ConverterApp : public CWinApp
{
public:
    CPCMDSD_ConverterApp();

public:
    virtual BOOL InitInstance();

    DECLARE_MESSAGE_MAP()
};

extern CPCMDSD_ConverterApp theApp;
