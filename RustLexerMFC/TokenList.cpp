#include "pch.h"
//coding: GBK
//@author: Cairenbin
//Version: 1.0
//Only use for homework: Compilers
//fileName: TokenList.cpp

#include "TokenList.h"
#include <stdexcept> //引入异常处理

//构造函数
TokenList::TokenList() {
    m_capacity = 1280;                  //初始容量
    m_size = 0;                         //初始大小
    m_data = new Token[m_capacity];     //分配动态数组
} //TokenList构造函数定义结束

//析构函数
TokenList::~TokenList() {
    delete[] m_data;                    //释放动态数组
    m_data = 0;
} //TokenList析构函数定义结束

//清空记号列表
void TokenList::Clear() {
    //清空记号列表的逻辑实现, 保留内存不释放后续使用
    m_size = 0;                         //重置大小
} //TokenList::Clear函数定义结束

//获取记号列表的大小(记号数量or元素个数)
int TokenList::Size() const { return m_size; } //TokenList::Size函数定义结束

//按照下标获取记号
const Token& TokenList::Get(int i) const {
    if (i < 0 || i >= m_size) {
        throw std::out_of_range("TokeenList index out of range!"); //抛出越界异常
    }
    return m_data[i];                   //返回对应下标的记号
} //TokenList::Get函数定义结束

//扩容函数
void TokenList::Expand() {
    int newCap = m_capacity * 2;
    Token* pNew = new Token[newCap];

    for (int i = 0; i < m_size; ++i)
    {
        pNew[i] = m_data[i];
    }

    delete[] m_data;                    //释放旧的动态数组
    m_data = pNew;                      //指向新的动态数组q
    m_capacity = newCap;                //更新容量
} //TokenList::Expand函数定义结束

void TokenList::Add(const Token& tk) {
    if (m_size >= m_capacity) {          //如果当前大小超过容量, 则扩容
        Expand();                       //调用扩容函数
    }
    m_data[m_size] = tk;                //追加记号到列表末尾
    m_size++;                           //增加大小
} //TokenList::Add函数定义结束