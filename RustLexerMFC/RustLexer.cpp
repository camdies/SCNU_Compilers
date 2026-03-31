#include "pch.h"
//coding: GBK
//@author: Cairenbin
//Version: 1.0
//Only use for homework: Compilers
//fileName: RustLexer.cpp

#include "RustLexer.h"
#include <cctype>
#include <set>
//-------------------------------------------------------------------------------------------

// Rust keywords
//作用: 定义 Rust 语言的关键字列表，用于词法分析器识别和区分标识符与关键字。
//输入: 无
//输出: 一个字符串数组 g_keywords，包含 Rust 语言的所有关键字，以及一个整数 g_kw_count，表示关键字的数量。
static const char* g_keywords[] = {
    "as","break","const","continue","crate","else","enum","extern","false","fn",
    "for","if","impl","in","let","loop","match","mod","move","mut","pub","ref",
    "return","self","Self","static","struct","super","trait","true","type","unsafe",
    "use","where","while","async","await","dyn"
}; // Rust 语言的关键字列表定义结束
//-------------------------------------------------------------------------------------------

static const int g_kw_count = sizeof(g_keywords) / sizeof(g_keywords[0]); // 计算关键字数量

// ---------- 新增：进制有效性判断函数 ----------
static bool IsBinDigit(char c) { return c == '0' || c == '1'; }
static bool IsOctDigit(char c) { return c >= '0' && c <= '7'; }
static bool IsHexDigit(char c) {
    return std::isdigit((unsigned char)c) ||
        (c >= 'a' && c <= 'f') ||
        (c >= 'A' && c <= 'F');
}
// ----------------------------------------------

// 判断是否为关键字
//作用: 判断给定的字符串是否是 Rust 语言的关键字，返回 true 表示是关键字，false 表示不是。
//输入: 一个字符串 s
//输出: 返回 true 表示 s 是 Rust 语言的关键字，返回 false 表示 s 不是 Rust 语言的关键字。
bool RustLexer::IsKeyword(const std::string& s) const {
    for (int i = 0; i < g_kw_count; ++i)
    {
        if (s == g_keywords[i]) return true;
    }
    return false;
} //IsKeyword 函数定义结束
//--------------------------------------------------------------------------------------------

// 判断是否为标识符的起始字符
//作用: 判断给定的字符是否可以作为 Rust 语言标识符的起
//输入: 一个字符 c
//输出: 返回 true 表示 c 是一个有效的标识符起始字符（字母或下划线），
//      返回 false 表示 c 不是一个有效的标识符起始字符。
bool RustLexer::IsIdStart(char c) const {
    return std::isalpha((unsigned char)c) || c == '_'; // 标识符的起始字符必须是字母或下划线
} //IsIdStart 函数定义结束
//--------------------------------------------------------------------------------------------

// 判断是否为标识符的非起始字符
//作用: 判断给定的字符是否可以作为 Rust 语言标识符的非起始字符
//输入: 一个字符 c
//输出: 返回 true 表示 c 是一个有效的标识符非起始字符（字母、数字或下划线），
//      返回 false 表示 c 不是一个有效的标识符非起始字符。
bool RustLexer::IsIdPart(char c) const {
    return std::isalnum((unsigned char)c) || c == '_'; // 标识符的非起始字符可以是字母、数字或下划线
} //IsIdPart 函数定义结束
//--------------------------------------------------------------------------------------------

// 判断是否为数字字符
//作用: 判断给定的字符是否是一个数字字符
//输入: 一个字符 c
//输出: 返回 true 表示 c 是一个数字字符（0-9），返回 false 表示 c 不是一个数字字符。
bool RustLexer::IsDigit(char c) const {
    return std::isdigit((unsigned char)c) != 0; // 数字字符必须是 0-9
} //IsDigit 函数定义结束
//--------------------------------------------------------------------------------------------

// 将一个 Token 对象添加到 TokenList 中
//作用: 创建一个 Token 对象并将其添加到给定的 TokenList 中
//输入: 一个 TokenList 对象 out，一个 TokenType 枚举值 t，一个字符串 lex 一个整数 line 和一个整数 col
//输出: 无返回值，但会将一个新的 Token 对象添加到 out 中，Token 对象的类型为 t，
//      文本内容为 lex，行号为 line，列号为 col。
void RustLexer::PushToken(TokenList& out, enum TokenType t, const std::string& lex, int line, int col) {
    Token tk;
    tk.type = t;
    tk.lexeme = lex;
    tk.line = line;
    tk.column = col; // 这里的 col 参数已在函数参数列表中声明
    out.Add(tk);
} //PushToken 函数定义结束
//--------------------------------------------------------------------------------------------

// 匹配分隔符
//作用: 判断给定位置的字符是否是 Rust 语言的分隔符，并返回匹配结果
//输入: 一个字符串 code，一个位置 pos，一个字符串引用 out 和一个整数引用 len
//输出: 返回 true 表示在 code 的 pos 位置匹配到了一个分隔
//      符，并将匹配到的分隔符存储在 out 中，匹配到的分隔符长度存储在 len 中；
//      返回 false 表示在 code 的 pos 位置没有匹配到分隔符。
bool RustLexer::MatchDelimiter(const std::string& code, size_t pos, std::string& out, size_t& len) const {
    const char c = code[pos];
    const char* delims = "(){}[];,:#"; // Rust 语言的分隔符列表
    for (const char* p = delims; *p; ++p) {
        if (c == *p) {
            out.assign(1, c); // 匹配成功，设置输出字符串为匹配到的分隔符
            len = 1;
            return true; // 匹配成功，返回 true
        }
    }
    return false; // 没有匹配到分隔符，返回 false
} //MatchDelimiter 函数定义结束
//--------------------------------------------------------------------------------------------

// 匹配操作符
//作用: 判断给定位置的字符是否是 Rust 语言的操作符，并返回匹配结果
//输入: 一个字符串 code，一个位置 pos，一个字符串引用 out 和一个整数引用 len
//输出: 返回 true 表示在 code 的 pos 位置匹配到了一个操作符，
//      并将匹配到的操作符存储在 out 中，匹配到的操作符长度存储在 len 中；
//      返回 false 表示在 code 的 pos 位置没有匹配到操作符。
bool RustLexer::MatchOperator(const std::string& code, size_t pos, std::string& out, size_t& len) const {
    // Rust 语言的操作符列表
    // 注意：越长的操作符必须写在越前面，确保匹配优先级（最大吞噬原则）
    // 为了解决 Option<Option<i32>> 泛型解析冲突
    // 将 ">>", "<<", "<<=", ">>=" 这些移出词法算符数组。它们将在解析时被拆分为单字符 '<' 和 '>'
    static const char* ops[] = { 
        "..=",
        "==","!=",">=","<=","+=","-=","*=","/=","%=","&&","||","::","..",
        "->","=>", 
        "=","+","-","*","/","%","!","<",">","&","|","^",".","?","@" 
    };
    const int n = sizeof(ops) / sizeof(ops[0]); // 计算操作符数量
    for (int i = 0; i < n; ++i) {
        std::string op = ops[i];
        if (pos + op.size() <= code.size() && code.compare(pos, op.size(), op) == 0) {
            out = op; // 匹配成功，设置输出字符串为匹配到的操作符
            len = op.size();
            return true; // 匹配成功，返回 true
        }
    }
    return false; // 没有匹配到操作符，返回 false
} //MatchOperator 函数定义结束
//--------------------------------------------------------------------------------------------

// 词法分析器入口函数
//作用: 将输入的源代码字符串分解成一个个 Token 对象，并将这些 Token 对象存储在一个 TokenList 对象中
//输入: 一个字符串 code 和一个 TokenList 对象 outTokens
//输出: 无返回值，但会将 code 中的所有 Token 对象添加到 outTokens 中，供后续的语法分析使用。
void RustLexer::Tokenize(const std::string& code, TokenList& outTokens) {
    outTokens.Clear(); // 清空输出的 TokenList，准备存储新的 Token 对象

    size_t  i = 0;
    int line = 1, col = 1;
    while (i < code.size()) {
        char ch = code[i];

        if (ch == ' ' || ch == '\t' || ch == '\r') { ++i; ++col; continue; } // 跳过空格、制表符和回车符
        if (ch == '\n') { ++i; ++line; col = 1; continue; } // 跳过换行符，并更新行号和列号

        int sLine = line, sCol = col; // 记录当前 Token 的起始行号和列号

        // ... 之前的代码保持不变 ...

        //处理注释
        if (ch == '/') {
            if (i + 1 < code.size() && code[i + 1] == '/') { // 单行注释
                size_t st = i;
                i += 2; col += 2;
                while (i < code.size() && code[i] != '\n') { ++i; ++col; } // 跳过单行注释内容

                // 计算长度，剔除可能包含的行尾 \r 字符
                size_t len = i - st;
                if (len > 0 && code[st + len - 1] == '\r') {
                    len--;
                }

                PushToken(outTokens, TT_Comment, code.substr(st, len), sLine, sCol); // 将单行注释作为一个 Token 添加到输出列表中
                continue;
            }
            if (i + 1 < code.size() && code[i + 1] == '*') { // 多行注释
                size_t st = i;
                i += 2; col += 2;
                int commentLines = 0; // 记录多行注释中包含的换行数
                while (i + 1 < code.size())
                {
                    if (code[i] == '\n') {
                        ++line;
                        col = 1;
                        ++i;
                        ++commentLines; // 记录换行
                        continue;
                    } // 跳过换行符，并更新行号和列号
                    if (code[i] == '*' && code[i + 1] == '/') {
                        i += 2;
                        col += 2;
                        break;
                    } // 匹配到多行注释的结束标志，跳出循环
                    ++i; ++col; // 继续扫描多行注释内容
                }

                // 修复多行注释输出问题：
                // 多行注释内部可能包含多行文本，我们在输出结果时为了不打乱格式，
                // 可以将其中的换行符替换为空格或其他占位符，或者保持原样输出
                // 这里我们将原样输出，但需要注意的是你的 BuildResultText 可能在按行解析结果时遇到了麻烦。
                PushToken(outTokens, TT_Comment, code.substr(st, i - st), sLine, sCol); // 将多行注释作为一个 Token 添加到输出列表中
                continue;
            }
        }

        // ... 后续代码保持不变 ...
        /*
        //处理注释
        if (ch == '/') {
            if (i + 1 < code.size() && code[i + 1] == '/') { // 单行注释
                size_t st = i;
                i += 2; col += 2;
                while (i < code.size() && code[i] != '\n') { ++i; ++col; } // 跳过单行注释内容

                // 计算长度，剔除可能包含的行尾 \r 字符
                size_t len = i - st;
                if (len > 0 && code[st + len - 1] == '\r') {
                    len--;
                }

                PushToken(outTokens, TT_Comment, code.substr(st, len), sLine, sCol); // 将单行注释作为一个 Token 添加到输出列表中
                continue;
            }
            if (i + 1 < code.size() && code[i + 1] == '*') { // 多行注释
                size_t st = i;
                i += 2; col += 2;
                while (i + 1 < code.size())
                {
                    if (code[i] == '\n') { ++line; col = 1; ++i; continue; } // 跳过换行符，并更新行号和列号
                    if (code[i] == '*' && code[i + 1] == '/') { i += 2; col += 2; break; } // 匹配到多行注释的结束标志，跳出循环
                    ++i; ++col; // 继续扫描多行注释内容
                }
                PushToken(outTokens, TT_Comment, code.substr(st, i - st), sLine, sCol); // 将多行注释作为一个 Token 添加到输出列表中
                continue;
            }
        }
        */

        //处理字符串
        if (ch == '"') {
            size_t st = i;
            ++i; ++col;
            bool esc = false;
            while (i < code.size()) {
                char c = code[i];
                if (c == '\n') { ++line; col = 1; ++i; esc = false; continue; } // 跳过换行符，并更新行号和列号)
                if (!esc && c == '"') { ++i; col += 1; break; } // 匹配到字符串的结束标志，跳出循环
                if (!esc && c == '\\') esc = true; else esc = false; // 处理转义字符
                ++i; ++col; // 继续扫描字符串内容
            }
            PushToken(outTokens, TT_StringLiteral, code.substr(st, i - st), sLine, sCol); // 将字符串作为一个 Token 添加到输出列表中
            continue;
        } //
        /*
        //处理数字
        if (IsDigit(ch)) {
            size_t st = i;
            bool isFloat = false;

            if (ch == '0' && i + 1 < code.size()) { //处理数字的不同进制表示
                char p = code[i + 1];
                if (p == 'x' || p == 'X' || p == 'o' || p == 'O' || p == 'b' || p == 'B') {
                    i += 2; col += 2; // 跳过进制前缀
                    while (i < code.size()) {
                        char d = code[i];
                        bool valid = false;
                        if (p == 'x' || p == 'X') valid = IsHexDigit(d);
                        else if (p == 'o' || p == 'O') valid = IsOctDigit(d);
                        else valid = IsBinDigit(d);

                        if (valid || d == '_') {
                            ++i; ++col;
                        }
                        else {
                            break; // 遇到不属于该进制范围的内容立刻停止
                        }
                    }
                    PushToken(outTokens, TT_IntegerLiteral, code.substr(st, i - st), sLine, sCol);
                    continue;
                }
            }

            while (i < code.size() && (IsDigit(code[i]) || code[i] == '_')) { ++i; ++col; } // 继续扫描数字内容，允许数字和下划线

            if (i < code.size() && code[i] == '.' && !(i + 1 < code.size() && code[i + 1] == '.')) { // 处理浮点数
                isFloat = true;
                ++i; ++col; // 跳过小数点
                while (i < code.size() && (IsDigit(code[i]) || code[i] == '_')) { ++i; ++col; } // 继续扫描浮点数内容，允许数字和下划线
            }

            if (i < code.size() && (code[i] == 'e' || code[i] == 'E')) { //处理科学计数法
                size_t expPos = i;
                size_t j = i + 1;
                if (j < code.size() && (code[j] == '+' || code[j] == '-')) {
                    ++j;
                }

                // 只有 e/E 后至少一个数字，才当作科学计数法
                if (j < code.size() && IsDigit(code[j])) {
                    isFloat = true;
                    ++i; ++col; // 吃掉 e/E
                    if (i < code.size() && (code[i] == '+' || code[i] == '-')) {
                        ++i; ++col;
                    }
                    while (i < code.size() && (IsDigit(code[i]) || code[i] == '_')) {
                        ++i; ++col;
                    }
                }
                else {
                    // 不合法指数，回退为普通整数/浮点，不吃 e
                    i = expPos;
                }
            }

            PushToken(outTokens, isFloat ? TT_FloatLiteral : TT_IntegerLiteral, code.substr(st, i - st), sLine, sCol);
            continue;
        }
        */
        //处理数字
        if (IsDigit(ch)) {
            size_t st = i;
            bool isFloat = false;
            bool isScientific = false;

            if (ch == '0' && i + 1 < code.size()) { //处理数字的不同进制表示
                char p = code[i + 1];
                if (p == 'x' || p == 'X' || p == 'o' || p == 'O' || p == 'b' || p == 'B') {
                    i += 2; col += 2; // 跳过进制前缀
                    enum TokenType baseType = TT_IntegerLiteral_Dec;
                    while (i < code.size()) {
                        char d = code[i];
                        bool valid = false;
                        if (p == 'x' || p == 'X') {
                            valid = IsHexDigit(d);
                            baseType = TT_IntegerLiteral_Hex;
                        }
                        else if (p == 'o' || p == 'O') {
                            valid = IsOctDigit(d);
                            baseType = TT_IntegerLiteral_Oct;
                        }
                        else {
                            valid = IsBinDigit(d);
                            baseType = TT_IntegerLiteral_Bin;
                        }

                        if (valid || d == '_') {
                            ++i; ++col;
                        }
                        else {
                            break; // 遇到不属于该进制范围的内容立刻停止
                        }
                    }
                    PushToken(outTokens, baseType, code.substr(st, i - st), sLine, sCol);
                    continue;
                }
            }

            // 十进制部分
            while (i < code.size() && (IsDigit(code[i]) || code[i] == '_')) { ++i; ++col; } // 继续扫描数字内容，允许数字和下划线

            if (i < code.size() && code[i] == '.' && !(i + 1 < code.size() && code[i + 1] == '.')) { // 处理浮点数
                isFloat = true;
                ++i; ++col; // 跳过小数点
                while (i < code.size() && (IsDigit(code[i]) || code[i] == '_')) { ++i; ++col; } // 继续扫描浮点数内容，允许数字和下划线
            }

            if (i < code.size() && (code[i] == 'e' || code[i] == 'E')) { //处理科学计数法
                size_t expPos = i;
                size_t j = i + 1;
                if (j < code.size() && (code[j] == '+' || code[j] == '-')) {
                    ++j;
                }

                // 只有 e/E 后至少一个数字，才当作科学计数法
                if (j < code.size() && IsDigit(code[j])) {
                    isFloat = true;
                    isScientific = true; // 标记为科学计数法
                    ++i; ++col; // 吃掉 e/E
                    if (i < code.size() && (code[i] == '+' || code[i] == '-')) {
                        ++i; ++col;
                    }
                    while (i < code.size() && (IsDigit(code[i]) || code[i] == '_')) {
                        ++i; ++col;
                    }
                }
                else {
                    // 不合法指数，回退为普通整数/浮点，不吃 e
                    i = expPos;
                }
            }

            // 根据标记判断最终的类型
            enum TokenType numType = TT_IntegerLiteral_Dec;
            if (isScientific) {
                numType = TT_FloatLiteral_Sci;
            }
            else if (isFloat) {
                numType = TT_FloatLiteral;
            }

            PushToken(outTokens, numType, code.substr(st, i - st), sLine, sCol);
            continue;
        }

        /*
        // 处理原生标识符: r#ident
        if (ch == 'r' && i + 2 < code.size() && code[i + 1] == '#' && IsIdStart(code[i + 2])) {
            size_t st = i;
            i += 3;          // 吃掉 r# 和首字符
            col += 3;

            while (i < code.size() && IsIdPart(code[i])) {
                ++i; ++col;
            }

            std::string rawWord = code.substr(st, i - st);

            // 可选：支持 r#name! 作为宏调用
            if (i < code.size() && code[i] == '!') {
                ++i; ++col;
                rawWord.push_back('!');
                PushToken(outTokens, TT_MacroIdentifier, rawWord, sLine, sCol);
            }
            else {
                PushToken(outTokens, TT_RawIdentifier, rawWord, sLine, sCol);
            }
            continue;
        }
        */

        // 处理特殊前缀词：b"...", b'...', br"...", r"...", 以及原生标识符 r#ident
        if (ch == 'b' || ch == 'r') {
            size_t j = i + 1;
            bool isByte = (ch == 'b');
            if (isByte && j < code.size() && code[j] == 'r') {
                j++; // 匹配 br 前缀
            }

            if (j < code.size()) {
                if (code[j] == '"' || code[j] == '#') {
                    // 看是否是 r"...", r#"..." 或者是 br"...", br#"..."
                    size_t hashCount = 0;
                    size_t k = j;
                    while (k < code.size() && code[k] == '#') {
                        hashCount++;
                        k++;
                    }

                    if (k < code.size() && code[k] == '"') {
                        // 确定是 原始字符串(Raw String) 或者 原始字节字符串
                        size_t st = i;
                        i = k + 1;
                        col += (int)(i - st);

                        while (i < code.size()) {
                            if (code[i] == '\n') {
                                ++line; col = 1; ++i; continue;
                            }
                            if (code[i] == '"') {
                                size_t matchK = i + 1;
                                size_t matchH = 0;
                                // 验证结尾的 # 数量必须全对应
                                while (matchH < hashCount && matchK < code.size() && code[matchK] == '#') {
                                    matchH++; matchK++;
                                }
                                if (matchH == hashCount) {
                                    col += (int)(matchK - i);
                                    i = matchK;
                                    break;
                                }
                            }
                            ++i; ++col;
                        }
                        PushToken(outTokens, TT_StringLiteral, code.substr(st, i - st), sLine, sCol);
                        continue;
                    }
                    else if (!isByte && hashCount == 1 && k < code.size() && IsIdStart(code[k])) {
                        // 提取：原生的非关键字标识符 r#ident
                        size_t st = i;
                        i = k;
                        col += (int)(i - st);
                        while (i < code.size() && IsIdPart(code[i])) {
                            ++i; ++col;
                        }
                        std::string rawWord = code.substr(st, i - st);
                        if (i < code.size() && code[i] == '!') {
                            ++i; ++col;
                            rawWord.push_back('!');
                            PushToken(outTokens, TT_MacroIdentifier, rawWord, sLine, sCol);
                        }
                        else {
                            PushToken(outTokens, TT_RawIdentifier, rawWord, sLine, sCol);
                        }
                        continue;
                    }
                }
                else if (isByte && code[j] == '"') {
                    // b"..." 字节字符串
                    size_t st = i;
                    i = j + 1;
                    col += (int)(i - st);
                    bool esc = false;
                    while (i < code.size()) {
                        char c = code[i];
                        if (c == '\n') { ++line; col = 1; ++i; esc = false; continue; }
                        if (!esc && c == '"') { ++i; col += 1; break; }
                        esc = (!esc && c == '\\');
                        ++i; ++col;
                    }
                    PushToken(outTokens, TT_StringLiteral, code.substr(st, i - st), sLine, sCol);
                    continue;
                }
                else if (isByte && code[j] == '\'') {
                    // b'...' 字节字符
                    size_t st = i;
                    i = j + 1;
                    col += (int)(i - st);
                    bool esc = false;
                    while (i < code.size()) {
                        char c = code[i];
                        if (c == '\n') break;
                        if (!esc && c == '\'') { ++i; col += 1; break; }
                        esc = (!esc && c == '\\');
                        ++i; ++col;
                    }
                    PushToken(outTokens, TT_CharLiteral, code.substr(st, i - st), sLine, sCol);
                    continue;
                }
            }
        }

        // 处理生命周期/标签 ('outer, 'a) 以及普通字符字面量 ('a', '\n', '中')
        if (ch == '\'') {
            size_t st = i;

            // 先尝试匹配生命周期或标签 (例如 'outer)
            // 它由单引号开头，紧接着是一个起始有效标识符符
            if (i + 1 < code.size() && IsIdStart(code[i + 1])) {
                size_t j = i + 1;
                while (j < code.size() && IsIdPart(code[j])) {
                    ++j;
                }

                // 在标签后如果有单引号闭合，如 'a'，那它是个字符字面量，如果没闭合，那是个生命周期/标签
                if (j >= code.size() || code[j] != '\'') {
                    size_t len = j - st;
                    i = j;
                    col += (int)(len);

                    std::string lfword = code.substr(st, len);
                    enum TokenType lftype = TT_Lifetime;

                    // 启发式：如果长度大于2(形如 'ab) 或后面跟着冒号，一般视为标签(Label)，否则视为生命周期
                    // 词法层面精确区分很难，因为 'a 在某些地方也是生命周期声明
                    // 如果源文件中有 ':' 这个紧跟着，那肯定是声明标签

                    // 跳过空白并向前预读看有没有冒号
                    size_t peek = j;
                    while (peek < code.size() && (code[peek] == ' ' || code[peek] == '\t')) { peek++; }
                    if ((peek < code.size() && code[peek] == ':') || len > 2) {
                        lftype = TT_Label;
                    }

                    PushToken(outTokens, lftype, lfword, sLine, sCol);
                    continue;
                }
            }

            // 如果不是生命周期，而是字符字面量，走这一步
            ++i; ++col;
            bool esc = false;
            bool closed = false;

            while (i < code.size()) {
                char c = code[i];
                if (c == '\n') break; // 换行通常说明没闭合
                if (!esc && c == '\'') {
                    ++i; ++col; // 吃掉结尾的单引号
                    closed = true;
                    break;
                }
                esc = (!esc && c == '\\');
                ++i; ++col;
            }

            if (closed) {
                PushToken(outTokens, TT_CharLiteral, code.substr(st, i - st), sLine, sCol);
            }
            else {
                // 如果单引号没有正确闭合（错误情况），我们将它标记为未知或部分提取
                PushToken(outTokens, TT_Unknown, code.substr(st, i - st), sLine, sCol);
            }
            continue;
        }

        //处理标识符, 关键字, 宏标识 （这里的代码不需要改）

        /*
        // 原始字符串字面量：r"...", r#"..."#, r##"..."##
        if (ch == 'r') {
            size_t j = i + 1;
            size_t hashCount = 0;

            // 统计 '#' 的数量
            while (j < code.size() && code[j] == '#') {
                ++hashCount;
                ++j;
            }

            // 如果 '#' 之后是 '"'，说明是原始字符串
            if (j < code.size() && code[j] == '"') {
                size_t st = i;

                // 跳过 r###"
                i = j + 1;
                col += (int)(i - st);

                while (i < code.size()) {
                    if (code[i] == '\n') {
                        ++line;
                        col = 1;
                        ++i;
                        continue;
                    }

                    if (code[i] == '"') {
                        size_t k = i + 1;
                        size_t h = 0;
                        // 匹配闭合的 '#' 数量
                        while (h < hashCount && k < code.size() && code[k] == '#') {
                            ++h;
                            ++k;
                        }

                        // 如果匹配了相同数量的 '#'，说明原始字符串结束
                        if (h == hashCount) {
                            col += (int)(k - i);
                            i = k; // 移动到最后一个 '#' 之后
                            break;
                        }
                    }

                    // 遇到普通中文字符或普通字符
                    // 这里简单处理：由于 UTF-8 的中文字符是多字节的，直接推进指针即可，不影响跳过
                    ++i;
                    ++col;
                }

                PushToken(outTokens, TT_StringLiteral, code.substr(st, i - st), sLine, sCol);
                continue;
            }
        }

        // 处理字符字面量: 'a', '\n', '中'
        if (ch == '\'') {
            size_t st = i;
            ++i; ++col;
            bool esc = false;

            while (i < code.size()) {
                char c = code[i];

                // 如果遇到换行，说明字符字面量未闭合而已换行
                if (c == '\n') {
                    break;
                }

                if (!esc && c == '\'') {
                    ++i; ++col; // 吃掉结尾的引号
                    break;
                }

                if (!esc && c == '\\') {
                    esc = true;
                }
                else {
                    esc = false;
                }

                ++i; ++col;
            }

            PushToken(outTokens, TT_CharLiteral, code.substr(st, i - st), sLine, sCol);
            continue;
        }

        // 处理原生标识符: r#ident
        if (ch == 'r' && i + 2 < code.size() && code[i + 1] == '#' && IsIdStart(code[i + 2])) {
            size_t st = i;
            i += 3;          // 吃掉 r# 和首字符
            col += 3;

            while (i < code.size() && IsIdPart(code[i])) {
                ++i; ++col;
            }

            std::string rawWord = code.substr(st, i - st);

            // 可选：支持 r#name! 作为宏调用
            if (i < code.size() && code[i] == '!') {
                ++i; ++col;
                rawWord.push_back('!');
                PushToken(outTokens, TT_MacroIdentifier, rawWord, sLine, sCol);
            }
            else {
                PushToken(outTokens, TT_RawIdentifier, rawWord, sLine, sCol);
            }
            continue;
        }
        */
        //处理标识符, 关键字, 宏标识
        if (IsIdStart(ch)) {
            size_t st = i;
            ++i; ++col;
            while (i < code.size() && IsIdPart(code[i])) { ++i; ++col; } // 继续扫描标识符内容，允许字母、数字和下划线

            std::string word = code.substr(st, i - st); // 提取标识符文本

            if (i < code.size() && code[i] == '!') { //处理宏标识符
                ++i; ++col; // 跳过感叹号
                word.push_back('!'); // 将感叹号添加到标识符文本中，形成宏标识符
                PushToken(outTokens, TT_MacroIdentifier, word, sLine, sCol); // 将宏标识符作为一个 Token 添加到输出列表中
                continue;
            }
            if (word == "true" || word == "false") {
                PushToken(outTokens, TT_BooleanLiteral, word, sLine, sCol);
            }
            else {
                PushToken(outTokens, IsKeyword(word) ? TT_Keyword : TT_Identifier, word, sLine, sCol);
            }
            // 根据是否是关键字将标识符作为一个 Token 添加到输出列表中
            continue;
        }

        //处理分隔符
        {
            std::string d; size_t len = 0;
            if (MatchDelimiter(code, i, d, len)) { //处理分隔符
                PushToken(outTokens, TT_Delimiter, d, sLine, sCol); // 将分隔符作为一个 Token 添加到输出列表中
                i += len; col += (int)len; // 跳过匹配到的分隔符
                continue;
            }
        }

        //处理操作符
        {
            std::string op; size_t len = 0;
            if (MatchOperator(code, i, op, len)) { //处理操作符
                PushToken(outTokens, TT_Operator, op, sLine, sCol); // 将操作符作为一个 Token 添加到输出列表中
                i += len; col += (int)len; // 跳过匹配到的操作符
                continue;
            }
        }

        // 未知
        PushToken(outTokens, TT_Unknown, std::string(1, ch), sLine, sCol);
        ++i; ++col;
    }
} //Tokenize函数定义结束
//--------------------------------------------------------------------------------------------