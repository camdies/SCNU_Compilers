//coding: GBK
//@author: Cairenbin
//Version: 1.0
//Only use for homework: Compilers
//fileName: TokenList.h

#pragma once
#include "Token.h"

//TokenList class
//作用: 
class TokenList {
public:

    TokenList(); //构造函数
    ~TokenList(); //析构函数

    void Clear();                       //清空记号列表(类似逻辑清空???)
    void Add(const Token& tk);          //追加

    //常函数开始
    int Size() const;                   //返回记号列表的大小(记号数量or元素个数)
    const Token& Get(int i) const;      //按照下标获取记号
    //常函数结束

private:

    //记号列表的动态数组实现
    Token* m_data;                      //记号列表的动态数组指针
    int m_size;                         //当前记号列表的大小(记号数量or元素个数)
    int m_capacity;                     //当前动态数组的容量
    //记号列表的动态数组实现定义结束

    //私有函数
    void Expand();                     //扩容函数

}; //TokenList类定义结束