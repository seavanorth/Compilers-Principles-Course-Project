#pragma once
#ifndef LEXICAL_ANALYSIS_H
#define LEXICAL_ANALYSIS_H

#include <stdio.h>
#include <iostream>
#include <cstring>
#include <string>
#include <map>
#include <set>
#include <stack>
#include <vector>
#include <queue>
#include <algorithm>
using namespace std;


extern int flag;// 判断前缀
extern int flag1; // 判断=类型
extern int n;

extern FILE* fp;
extern char ch; // 读取文件当前字符

extern string danci;//当前字符串

extern int nodeNum;
extern map<string, int> nodeNUM;
extern vector<string> express;//存储表达式
extern vector<string> symbol;//存储表达式前缀

void match();
void judge_danci();
void judge_number();
int compera(string oper);//比较运算符的计算优先度
void to_NFA();//将读取的正则表达式转化为NFA
int trans(int k);
void shuc();
void shuchu();
void to_DFA();
void simstate();
void simpDFA();
int findpos(int a, vector<set<int> > V, int num);
void simplifyDFA();
void judgeState();
string set_to_str(set<int> se);

extern map<int, map< string, set<int> > > M;//NFA存储
extern map<string, map<int, map< string, set<int> > > > MZ;//所有表达式的NFA
extern set<string> edgeData;//边包含的内容存储
extern map<string, set<string> > edge;//所有表达式的边内容存储
extern int nhead, ntail;//头尾节点
extern map<string, pair<int, int> > HT; //存所有表达式NFA头尾节点

extern map<set<int>, map<string, set<int> > > DM;//用来储存DFA
extern map<string, map<set<int>, map<string, set<int> > > > DMZ;//存所有表达式DFA
extern set<set<int> >allNode;//DFA全部节点
extern map<string, set<set<int> > > dallNode;//存所有表达式所有节点
extern map<set<int>, int> state;//存allNode集合中各节点的状态信息
extern map<string, map<set<int>, int> > dstate;//存所有表达式allNode集合中各节点的状态信息

extern map<set<int>, int> m;//给dfa节点一个简单的序号

extern int dfanum;//dfa节点数
extern map<int, map<string, int> > NM;//名字简化后的dfa对应关系

extern map<int, map<string, int> > SM;//最小化的dfa对应关系
extern map<string, map<int, map<string, int> > > SZ;//最小化dfa关系组
extern vector<set<int> > T;//用来存同类集合
extern map<int, int> sstate;//化简后状态
extern map<string, map<int, int> > states;//存储状态组
extern int num;//化简后节点数
extern map<string, int> ns; //存储所有表达式化简节点数
extern int last_danci, now_danci;

#endif // LexicalAnalysis