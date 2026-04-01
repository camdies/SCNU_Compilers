#pragma once
#include "RustLexer.h"

class CRustLexerMFCDlg : public CDialogEx
{
public:
    CRustLexerMFCDlg(CWnd* pParent = nullptr);

#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_RUSTLEXERMFC_DIALOG };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual BOOL OnInitDialog();
    DECLARE_MESSAGE_MAP()

private:
    CString m_filePath;
    CString m_sourceText;
    CString m_resultText;

    CEdit m_editPath;
    CEdit m_editSource;
    CEdit m_editResult;

    bool LoadFileToCString(const CString& path, CString& outText);
    CString BuildResultText(const std::string& code, const TokenList& list);
    void AnalyzeCurrentCode();

private:
    afx_msg void OnBnClickedBtnOpen();
    afx_msg void OnBnClickedBtnAnalyze();
    afx_msg void OnBnClickedBtnExport();
};