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

extern vector<string> Terminator;//终结符号集合
extern vector<string> NT;//非终结符号集合
extern string S;//开始符号
extern map<string, vector<string> >production;//产生式
extern map<string, set<string> > FIRST;//FIRST集
extern map<string, set<string> > FOLLOW;//FOLLOW集
extern map<pair<string, string>, string> Table;//LL(1)文法分析表
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
