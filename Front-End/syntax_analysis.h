#pragma once
#ifndef SYNTAX_ANALYSIS_H
#define SYNTAX_ANALYSIS_H

#include <iostream>
#include <fstream>
#include <set>
#include <map>
#include <stack>
#include <vector>
#include <QString>
using namespace std;

extern vector<string> Terminator;//�ս���ż���
extern vector<string> NT;//���ս���ż���
extern string S;//��ʼ����
extern map<string, vector<string> >production;//����ʽ
extern map<string, set<string> > FIRST;//FIRST��
extern map<string, set<string> > FOLLOW;//FOLLOW��
extern map<pair<string, string>, string> Table;//LL(1)�ķ�������
extern QString fileName;

string maxPrefix(string left);
void immediateLeftRecursionRemoval(string Ai);
void addP(string left, string right);
void addT();
void readGrammar(string fileName);
void leftRecursionRemoval();
void leftFactoring();
void getFirst();
void getFollow();
string returnfirst(string gammar);
void simplify();
int hasIntersection(set<string>first, set<string>second);
void getTable();

#endif // Syntax_Analysis
