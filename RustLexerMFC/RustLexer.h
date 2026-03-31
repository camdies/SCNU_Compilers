//coding: GBK
//@author: Cairenbin
//Version: 1.0
//Only use for homework: Compilers
//fileName: RustLexer.h

#pragma once
#include <string>
#include "TokenList.h"

//词法分析器
//输入：源代码字符串
//输出：TokenList对象
//作用: 将源代码字符串分解成一个个Token对象，并将这些Token对象存储在一个TokenList对象中，供后续的语法分析使用。
class RustLexer {
private:
    bool IsKeyword(const std::string& s) const; //判断是否为关键字
    bool IsIdStart(char c) const; //判断是否为标识符的起始字符
    bool IsIdPart(char c) const; //判断是否为标识符的非起始字符
    bool IsDigit(char c) const; //判断是否为数字字符

    bool MatchDelimiter(const std::string& code, size_t pos, std::string& out, size_t& len) const; //匹配分隔符
    bool MatchOperator(const std::string& code, size_t pos, std::string& out, size_t& len) const; //匹配操作符

    void PushToken(TokenList& out, enum TokenType t, const std::string& lex, int line, int col); //将一个Token对象添加到TokenList中

public:

    //词法分析器入口函数
    void Tokenize(const std::string& code, TokenList& outTokens);

}; //RustLexer类的定义结束