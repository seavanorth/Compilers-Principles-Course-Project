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


extern int flag;// �ж�ǰ׺
extern int flag1; // �ж�=����
extern int n;

extern FILE* fp;
extern char ch; // ��ȡ�ļ���ǰ�ַ�

extern string danci;//��ǰ�ַ���

extern int nodeNum;
extern map<string, int> nodeNUM;
extern vector<string> express;//�洢���ʽ
extern vector<string> symbol;//�洢���ʽǰ׺

void match();
void judge_danci();
void judge_number();
int compera(string oper);//�Ƚ�������ļ������ȶ�
void to_NFA();//����ȡ��������ʽת��ΪNFA
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

extern map<int, map< string, set<int> > > M;//NFA�洢
extern map<string, map<int, map< string, set<int> > > > MZ;//���б��ʽ��NFA
extern set<string> edgeData;//�߰��������ݴ洢
extern map<string, set<string> > edge;//���б��ʽ�ı����ݴ洢
extern int nhead, ntail;//ͷβ�ڵ�
extern map<string, pair<int, int> > HT; //�����б��ʽNFAͷβ�ڵ�

extern map<set<int>, map<string, set<int> > > DM;//��������DFA
extern map<string, map<set<int>, map<string, set<int> > > > DMZ;//�����б��ʽDFA
extern set<set<int> >allNode;//DFAȫ���ڵ�
extern map<string, set<set<int> > > dallNode;//�����б��ʽ���нڵ�
extern map<set<int>, int> state;//��allNode�����и��ڵ��״̬��Ϣ
extern map<string, map<set<int>, int> > dstate;//�����б��ʽallNode�����и��ڵ��״̬��Ϣ

extern map<set<int>, int> m;//��dfa�ڵ�һ���򵥵����

extern int dfanum;//dfa�ڵ���
extern map<int, map<string, int> > NM;//���ּ򻯺��dfa��Ӧ��ϵ

extern map<int, map<string, int> > SM;//��С����dfa��Ӧ��ϵ
extern map<string, map<int, map<string, int> > > SZ;//��С��dfa��ϵ��
extern vector<set<int> > T;//������ͬ�༯��
extern map<int, int> sstate;//�����״̬
extern map<string, map<int, int> > states;//�洢״̬��
extern int num;//�����ڵ���
extern map<string, int> ns; //�洢���б��ʽ����ڵ���
extern int last_danci, now_danci;

#endif // LexicalAnalysis