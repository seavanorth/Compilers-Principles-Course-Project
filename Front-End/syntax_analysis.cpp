#include "stdafx.h"
#include "syntax_analysis.h"

vector<string> Terminator;//终结符号集合
vector<string> NT;//非终结符号集合
string S;//开始符号
map<string, vector<string> > production;//产生式
map<string, set<string> > FIRST;//FIRST集
map<string, set<string> > FOLLOW;//FOLLOW集
map<pair<string, string>, string> Table;//LL(1)文法分析表
QString fileName;

//求最长公共前缀
string maxPrefix(string left)
{
    string prefix = "";
    vector<string>P = production[left];
    string firstP = P[0];
    firstP += " ";

    while (firstP.find(" ") != firstP.size() - 1)
    {
        string temp = firstP.substr(0, firstP.find(" "));//left的第一个产生式中的token
        //debugcout<<"第一个产生中的第一个token："<<temp<<"\t";

        //遍历left的其他产生式
        for (int i = 1; i < P.size(); i++)
        {
            string right = P[i];
            right += " ";

            string cmp = right.substr(0, right.find(" "));
            //debugcout<<"后面产生式中第一个token："<<cmp<<"\t";

            if (cmp != temp)
            {
                break;
            }
            else
            {
                prefix += temp + " ";
            }
            P[i] = right.substr(right.find(" ") + 1);
        }
        firstP = firstP.substr(firstP.find(" ") + 1);
    }

    //去除末尾空格
    if (prefix.size() > 0)
        prefix.pop_back();
    return prefix;
}

void simplify() {
    for (int i = 0; i < NT.size(); i++) {
        for (int j = 0; j < production[NT[i]].size(); j++) {
            if (production[NT[i]][j] == NT[i]) {
                auto iter = production[NT[i]].erase(production[NT[i]].begin() + j);
                NT.erase(NT.begin() + i);
            }
            else if (returnfirst(production[NT[i]][j]) == NT[i] && production[NT[i]].size() == 1) {
                production.erase(NT[i]);
                NT.erase(NT.begin() + i);
            }
        }
    }
}

string returnfirst(string gammar) 
{
    string newch = "";
    for (int i = 0; i < gammar.size(); i++) {
        if (gammar[i] != ' ') {
            newch = newch + gammar[i];
            continue;
        }
        else {
            return newch;
        }
    }
}

//消除直接左递归
void immediateLeftRecursionRemoval(string Ai)
{
    string newNT = Ai + "'";//新的非终结符号
    NT.insert(find(NT.begin(), NT.end(), Ai) + 1, newNT);
    vector<string>newFirstRight;//新的产生式右部
    vector<string>newSecondRight;
    for (auto it = production[Ai].begin(); it < production[Ai].end(); it++)
    {
        string right = *it;
        string temp = right;
        temp += "#";

        //含有直接左递归:Ai'->aAi'|@
        if (strcmp(const_cast<char*>(Ai.c_str()), strtok(const_cast<char*>(temp.c_str()), " #")) == 0)
        {
            right = right.substr(right.find(" ") + 1) + " " + newNT;
            newSecondRight.push_back(right);
        }
        //不含：Ai->BAi'
        else
        {
            right += " " + newNT;
            newFirstRight.push_back(right);
        }
    }
    newSecondRight.push_back("@");
    production[newNT] = newSecondRight;
    production[Ai] = newFirstRight;
}

//填产生式
void addP(string left, string right)
{
    right += "#";//'#'作为每句文法结尾标志
    string pRight = "";
    for (int i = 0; i < right.size(); i++) {
        if (right[i] == '|' || right[i] == '#') {
            production[left].push_back(pRight);
            pRight = "";
        }
        else{
            pRight += right[i];
        }
    }
}

//填终结符
void addT()
{
    string temp = "";
    for (string left : NT) {
        for (string right : production[left]) {
           right += "#";
           for (int i = 0; i < right.size(); i++) {
               if (right[i] == '|' || right[i] == ' ' || right[i] == '#') {
                   //不是非终结，且不是空，则加入终结符号
                   if ((find(NT.begin(), NT.end(), temp) == NT.end()) && temp != "@") {
                        Terminator.push_back(temp);
                   }
                   temp = "";
               }
               else {
                    temp += right[i];
               }
           }
        }
    }//end left


    //终结符去重
    sort(Terminator.begin(), Terminator.end());
    Terminator.erase(unique(Terminator.begin(), Terminator.end()), Terminator.end());
}

//读取文法规则
void readGrammar(string fileName)
{
    ifstream input(fileName);
    if (!input) {
        debugcout << fileName << " Failed" << endl;
    }

    //读取文法规则
    string line;//读入的每一行
    while (getline(input, line)) {
        int i;

        //读取左部
        string left = "";
        for (i = 0; line[i] != '-' && i < line.size(); i++) {
            left += line[i];
        }

        NT.push_back(left);//左部加入非终结符号集

        //读取右部
        string right = line.substr(i + 2, line.size() - i);//获取产生式右部
        addP(left, right);//添加产生式
    }
    addT();//添加终极符
    S = *NT.begin();
    input.close();
}

//消除左递归
void leftRecursionRemoval()
{
    //遍历每一个NT
    for (int i = 0; i < NT.size(); i++)
    {
        string Ai = NT[i];
        //debugcout<<"Ai:"<<Ai<<endl;
        vector<string>newRight;//新的产生式右部

        //遍历NT的每一个产生式
        for (auto it = production[Ai].begin(); it < production[Ai].end(); it++)
        {
            string right = *it;
            //debugcout<<"right:"<<right<<endl;
            int flag = 0;//判断是不是左递归

            //遍历改变过的产生式
            for (int j = 0; j < i; j++)
            {
                string Aj = NT[j];
                //debugcout<<"Aj:"<<Aj<<endl;
                string temp = right + "#";

                //如果有Ai->AjB，替换Aj为Aj的产生式
                if (strcmp(const_cast<char*>(Aj.c_str()), strtok(const_cast<char*>(temp.c_str()), " #")) == 0)
                {
                    flag = 1;
                    debugcout << Aj << " ";
                    debugcout << temp << endl;
                    for (auto jt = production[Aj].begin(); jt < production[Aj].end(); jt++)
                    {
                        string s = *jt + " " + right.substr(right.find(" ") + 1);//substr(1)是从空格位置往后的子串
                        //debugcout<<"s:"<<s<<endl;
                        newRight.push_back(s);
                    }
                }
            }
            //没有可替换的产生式
            if (flag == 0)
                newRight.push_back(right);
        }
        if (i != 0)
            production[Ai] = newRight;

        //去除包含Ai的直接左递归
        for (int k = 0; k < production[Ai].size(); k++)
        {
            string right = production[Ai][k];
            string temp = right;
            temp += "#";

            if (strcmp(const_cast<char*>(Ai.c_str()), strtok(const_cast<char*>(temp.c_str()), " #")) == 0)
                immediateLeftRecursionRemoval(Ai);
        }
    }
}

//提取左因子
void leftFactoring()
{
    //printV();
    for (int i = 0; i < NT.size(); i++)
    {
        string left = NT[i];
        string a = maxPrefix(left);
        //debugcout<<"left:"<<left<<"\ta:"<<a<<endl;
        if (a != "")
        {
            string newNT = left + "'";
            NT.insert(find(NT.begin(), NT.end(), left), newNT);

            vector<string>newRight1;//A的产生式
            vector<string>newRight2;//A'的产生式
            for (auto it = production[left].begin(); it < production[left].end(); it++)
            {
                string right = *it;
                string newRight;

                //产生式不含a,直接放进A的产生式中
                if (right.find(a) == string::npos)
                    newRight1.push_back(right);

                //产生式含a
                else
                {
                    if (right.find(a) + a.size() != right.size())
                    {
                        newRight = right.substr(right.find(a) + a.size() + 1);
                    }
                    //a后面是空的
                    else
                    {
                        newRight = "@";
                    }
                    newRight2.push_back(newRight);
                }
            }
            //A->aA'
            newRight1.push_back(a + " " + newNT);
            production[left] = newRight1;
            production[newNT] = newRight2;
        }
    }
}

//获得FIRST集合
void getFirst()
{
    FIRST.clear();

    //终结符号或@
    FIRST["@"].insert("@");
    for (string X : Terminator)
    {
        FIRST[X].insert(X);
    }

    //非终结符号
    int j = 0;
    while (j < 5)
    {
        for (int i = 0; i < NT.size(); i++)
        {
            string A = NT[i];

            //遍历A的每个产生式
            for (int k = 0; k < production[A].size(); k++)
            {
                int Continue = 1;//是否添加@
                string right = production[A][k];

                //X是每条产生式第一个token
                string X;
                if (right.find(" ") == string::npos)
                    X = right;
                else
                    X = right.substr(0, right.find(" "));
                //debugcout<<A<<"\t"<<X<<endl;
                //FIRST[A]=FIRST[X]-@
                if (!FIRST[X].empty())
                {
                    for (string firstX : FIRST[X])
                    {
                        if (firstX == "@")
                            continue;
                        else
                        {
                            FIRST[A].insert(firstX);
                            Continue = 0;
                        }
                    }
                    if (Continue)
                        FIRST[A].insert("@");
                }
            }

        }
        j++;
    }
}

//获得FOLLOW集合
void getFollow()
{
    //将界符加入开始符号的follow集
    FOLLOW[S].insert("#");

    int j = 0;
    while (j < 5)
    {
        //遍历非终结符号
        for (string A : NT)
        {
            for (string right : production[A])
            {
                for (string B : NT)
                {
                    //A->Bb
                    if (right.find(B) != string::npos)
                    {
                        /*找B后的字符b*/
                        string b;
                        int flag = 0;
                        //识别到E'
                        if (right[right.find(B) + B.size()] != ' ' && right[right.find(B) + B.size()] != '\0')
                        {
                            string s = right.substr(right.find(B));//E'b
                            string temp = right.substr(right.find(B) + B.size());//' b

                            //A->E'
                            if (temp.find(" ") == string::npos)
                            {
                                B = s;//B:E->E'
                                FOLLOW[B].insert(FOLLOW[A].begin(), FOLLOW[A].end());//左部的FOLLOW赋给B
                                flag = 1;
                            }
                            //A->E'b
                            else
                            {
                                B = s.substr(0, s.find(" "));
                                temp = temp.substr(temp.find(" ") + 1);//b

                                //b后无字符
                                if (temp.find(" ") == string::npos)
                                    b = temp;
                                //b后有字符
                                else
                                    b = temp.substr(0, temp.find(" "));
                            }
                        }

                        //A->aEb
                        else if (right[right.find(B) + B.size()] == ' ')
                        {
                            string temp = right.substr(right.find(B) + B.size() + 1);//B后的子串

                            //b后无字符
                            if (temp.find(" ") == string::npos)
                                b = temp;
                            //b后有字符
                            else
                                b = temp.substr(0, temp.find(" "));
                        }
                        //A->aE
                        else
                        {
                            FOLLOW[B].insert(FOLLOW[A].begin(), FOLLOW[A].end());
                            flag = 1;
                        }

                        //FOLLOW[B]还没求到
                        if (flag == 0)
                        {
                            //FIRST[b]中不包含@
                            if (FIRST[b].find("@") == FIRST[b].end())
                            {
                                FOLLOW[B].insert(FIRST[b].begin(), FIRST[b].end());
                            }
                            else
                            {
                                for (string follow : FIRST[b])
                                {
                                    if (follow == "@")
                                        continue;
                                    else
                                        FOLLOW[B].insert(follow);
                                }
                                FOLLOW[B].insert(FOLLOW[A].begin(), FOLLOW[A].end());
                            }
                        }
                    }
                }
            }
        }
        j++;
    }
}

//判断两集合是否相交
int hasIntersection(set<string>first, set<string>second)
{
    for (string b : second) {
        //如果first和second有重复元素，则相交
        if (first.find(b) != first.end())
            return 1;
    }
    return 0;
}

void getTable()
{
    for (string A : NT) {
        for (string right : production[A]) {
            string first;//right里第一个token
            if (right.find(" ") == string::npos)
                first = right;
            else
                first = right.substr(0, right.find(" "));

            right = right.insert(0, A + "->");
            pair<string, string>symbol;

            //FIRST集里不含@:a来自FIRST[first]
            if (FIRST[first].find("@") == FIRST[first].end()) {
                for (string a : FIRST[first]) {
                    symbol = make_pair(A, a);
                    Table[symbol] = right;
                    //debugcout<<A<<"\t"<<a<<"\t"<<right<<endl;
                }
            }
            //FIRST集里含@:a来自FOLLOW[a]
            else
            {
                for (string a : FIRST[first]) {
                    symbol = make_pair(A, a);
                    Table[symbol] = right;
                    //debugcout<<A<<"\t"<<a<<"\t"<<right<<endl;
                }
                for (string a : FOLLOW[A]) {
                    symbol = make_pair(A, a);
                    Table[symbol] = right;
                    //debugcout<<A<<"\t"<<a<<"\t"<<right<<endl;
                }
            }
        }
    }
}
