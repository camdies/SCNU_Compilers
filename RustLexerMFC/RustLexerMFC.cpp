#include "pch.h"
#include "framework.h"
#include "RustLexerMFC.h"
#include "RustLexerMFCDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CRustLexerMFCApp, CWinApp)
END_MESSAGE_MAP()

CRustLexerMFCApp::CRustLexerMFCApp()
{
}

CRustLexerMFCApp theApp;

BOOL CRustLexerMFCApp::InitInstance()
{
    CWinApp::InitInstance();

    AfxEnableControlContainer();

    CRustLexerMFCDlg dlg;
    m_pMainWnd = &dlg;
    INT_PTR nResponse = dlg.DoModal();
    if (nResponse == IDOK)
    {
    }
    else if (nResponse == IDCANCEL)
    {
    }

    return FALSE;
}