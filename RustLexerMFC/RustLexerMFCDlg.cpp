#include "pch.h"
#include "framework.h"
#include "RustLexerMFC.h"
#include "RustLexerMFCDlg.h"
#include "afxdialogex.h"

#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <atlconv.h>
#include <locale>
#include <codecvt>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CRustLexerMFCDlg::CRustLexerMFCDlg(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_RUSTLEXERMFC_DIALOG, pParent)
{
}

void CRustLexerMFCDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EDIT_PATH, m_editPath);
    DDX_Control(pDX, IDC_EDIT_SOURCE, m_editSource);
    DDX_Control(pDX, IDC_EDIT_RESULT, m_editResult);
}

BEGIN_MESSAGE_MAP(CRustLexerMFCDlg, CDialogEx)
    ON_BN_CLICKED(IDC_BTN_OPEN, &CRustLexerMFCDlg::OnBnClickedBtnOpen)
    ON_BN_CLICKED(IDC_BTN_ANALYZE, &CRustLexerMFCDlg::OnBnClickedBtnAnalyze)
    ON_BN_CLICKED(IDC_BTN_EXPORT, &CRustLexerMFCDlg::OnBnClickedBtnExport)
END_MESSAGE_MAP()

BOOL CRustLexerMFCDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    m_editPath.SetReadOnly(TRUE);
    m_editSource.SetReadOnly(TRUE);
    m_editResult.SetReadOnly(TRUE);
    return TRUE;
}

bool CRustLexerMFCDlg::LoadFileToCString(const CString& path, CString& outText)
{
    CFile file;
    if (!file.Open(path, CFile::modeRead | CFile::typeBinary))
        return false;

    ULONGLONG len = file.GetLength();
    std::vector<char> buffer((size_t)len + 1, 0);

    if (len > 0)
        file.Read(buffer.data(), (UINT)len);

    file.Close();

    int wlen = MultiByteToWideChar(CP_UTF8, 0, buffer.data(), -1, nullptr, 0);
    if (wlen > 0)
    {
        std::wstring ws(wlen, L'\0');
        MultiByteToWideChar(CP_UTF8, 0, buffer.data(), -1, &ws[0], wlen);
        outText = ws.c_str();
        return true;
    }

    wlen = MultiByteToWideChar(CP_ACP, 0, buffer.data(), -1, nullptr, 0);
    if (wlen <= 0) return false;

    std::wstring ws(wlen, L'\0');
    MultiByteToWideChar(CP_ACP, 0, buffer.data(), -1, &ws[0], wlen);
    outText = ws.c_str();
    return true;
}

CString CRustLexerMFCDlg::BuildResultText(const std::string& code, const TokenList& list)
{
    std::istringstream iss(code);
    std::string line;
    int line_num = 1;
    CString result;

    while (std::getline(iss, line))
    {
        size_t first_non_space = line.find_first_not_of(" \t\r");
        if (first_non_space == std::string::npos)
        {
            ++line_num;
            continue;
        }

        std::string trimmed_line = line.substr(first_non_space);

        // 获取该行最后一个有效字符（剔除 \r）
        if (!trimmed_line.empty() && trimmed_line.back() == '\r') {
            trimmed_line.pop_back();
        }

        // 输出源代码行
        result += CString(CA2W(trimmed_line.c_str(), CP_UTF8)) + _T("\r\n");

        int max_line_jump = line_num; // 记录如果有跨行Token，最多跨到了哪一行

        for (int i = 0; i < list.Size(); ++i)
        {
            const Token& tk = list.Get(i);
            if (tk.line == line_num)
            {
                CString wLex(CA2W(tk.lexeme.c_str(), CP_UTF8));

                // 我们算一下这个 token 自己包含几个换行符
                int tokenLines = 0;
                for (char c : tk.lexeme) {
                    if (c == '\n') tokenLines++;
                }

                if (line_num + tokenLines > max_line_jump) {
                    max_line_jump = line_num + tokenLines;
                }

                if (tk.type == TT_Comment)
                {
                    result += _T("    ") + wLex + _T(" : 注释\r\n");
                }
                else
                {
                    CString wType(TokenTypeToCN(tk.type));
                    result += _T("    ") + wLex + _T(" : ") + wType + _T("\r\n");
                }
            }
        }

        result += _T("\r\n");

        // 关键逻辑：如果这一行的 token 有多行（比如多行注释或原始字符串跨行），我们把剩下的对应源码行快速消耗掉
        while (line_num < max_line_jump) {
            std::string dummy;
            std::getline(iss, dummy);
            line_num++;
        }

        ++line_num;
    }

    return result;
}
/*
CString CRustLexerMFCDlg::BuildResultText(const std::string& code, const TokenList& list)
{
    std::istringstream iss(code);
    std::string line;
    int line_num = 1;
    CString result;

    while (std::getline(iss, line))
    {
        size_t first_non_space = line.find_first_not_of(" \t\r");
        if (first_non_space == std::string::npos)
        {
            ++line_num;
            continue;
        }

        std::string trimmed_line = line.substr(first_non_space);
        result += CString(trimmed_line.c_str()) + _T("\r\n");

        for (int i = 0; i < list.Size(); ++i)
        {
            const Token& tk = list.Get(i);
            if (tk.line == line_num)
            {
                CString wLex(tk.lexeme.c_str());
                if (tk.type == TT_Comment)
                {
                    // 你期望格式：// ... : 注释
                    result += _T("    ") + wLex + _T(": 注释\r\n");
                }
                else
                {
                    CString wType(TokenTypeToCN(tk.type));
                    result += _T("    ") + wLex + _T(" : ") + wType + _T("\r\n");
                }
            }
        }

        result += _T("\r\n");
        ++line_num;
    }

    return result;
}
*/
/*
CString CRustLexerMFCDlg::BuildResultText(const std::string& code, const TokenList& list)
{
    std::istringstream iss(code);
    std::string line;
    int line_num = 1;
    CString result;

    while (std::getline(iss, line))
    {
        size_t first_non_space = line.find_first_not_of(" \t\r");
        if (first_non_space == std::string::npos)
        {
            ++line_num;
            continue;
        }

        std::string trimmed_line = line.substr(first_non_space);

        // 【修改点 1】：显式指定使用 CP_UTF8 将 UTF-8 std::string 转为 CString(Unicode)
        result += CString(CA2W(trimmed_line.c_str(), CP_UTF8)) + _T("\r\n");

        for (int i = 0; i < list.Size(); ++i)
        {
            const Token& tk = list.Get(i);
            if (tk.line == line_num)
            {
                // 【修改点 2】：显式指定使用 CP_UTF8 将 Token 内的 lexeme 转为 CString
                CString wLex(CA2W(tk.lexeme.c_str(), CP_UTF8));

                if (tk.type == TT_Comment)
                {
                    // 你期望格式：// ... : 注释
                    result += _T("    ") + wLex + _T(": 注释\r\n");
                }
                else
                {
                    // TokenTypeToCN 返回的是你的源码文件的 hardcode 字符串（由于你说是GBK编码保存的源码，这里默认转换不用变）
                    CString wType(TokenTypeToCN(tk.type));
                    result += _T("    ") + wLex + _T(" : ") + wType + _T("\r\n");
                }
            }
        }

        result += _T("\r\n");
        ++line_num;
    }

    return result;
}
*/

void CRustLexerMFCDlg::AnalyzeCurrentCode()
{
    if (m_sourceText.IsEmpty())
    {
        AfxMessageBox(_T("请先选择Rust源文件。"));
        return;
    }

    CW2A utf8(m_sourceText, CP_UTF8);
    std::string code((LPCSTR)utf8);

    RustLexer lexer;
    TokenList list;
    lexer.Tokenize(code, list);

    m_resultText = BuildResultText(code, list);
    m_editResult.SetWindowTextW(m_resultText);
}

void CRustLexerMFCDlg::OnBnClickedBtnOpen()
{
    CFileDialog dlg(
        TRUE, _T("rs"), nullptr,
        OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
        _T("Rust Files (*.rs)|*.rs|All Files (*.*)|*.*||"),
        this);

    if (dlg.DoModal() != IDOK) return;

    m_filePath = dlg.GetPathName();
    m_editPath.SetWindowTextW(m_filePath);

    if (!LoadFileToCString(m_filePath, m_sourceText))
    {
        AfxMessageBox(_T("读取文件失败。"));
        return;
    }

    m_editSource.SetWindowTextW(m_sourceText);
    m_editResult.SetWindowTextW(_T(""));
    m_resultText.Empty();
}

void CRustLexerMFCDlg::OnBnClickedBtnAnalyze()
{
    AnalyzeCurrentCode();
}

void CRustLexerMFCDlg::OnBnClickedBtnExport()
{
    if (m_resultText.IsEmpty())
    {
        AfxMessageBox(_T("没有可导出的结果，请先执行词法分析。"));
        return;
    }

    CFileDialog dlg(
        FALSE, _T("txt"), _T("analysis_result.txt"),
        OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY,
        _T("Text Files (*.txt)|*.txt|All Files (*.*)|*.*||"),
        this);

    if (dlg.DoModal() != IDOK) return;

    CString outPath = dlg.GetPathName();

    // 用CStdioFile以UTF-8编码写文件
    CStdioFile file;
    if (!file.Open(outPath, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary))
    {
        AfxMessageBox(_T("导出失败：无法创建文件。"));
        return;
    }

    // 写入UTF-8 BOM
    BYTE bom[] = { 0xEF, 0xBB, 0xBF };
    file.Write(bom, 3);

    // CString -> UTF-8
    CT2CA utf8str(m_resultText, CP_UTF8);
    file.Write(utf8str, (UINT)strlen(utf8str));
    file.Close();

    AfxMessageBox(_T("导出成功。"));
}