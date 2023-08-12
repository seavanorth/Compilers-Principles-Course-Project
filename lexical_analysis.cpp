#include "stdafx.h"
#include "lexical_analysis.h"

int flag = 1;// 判断前缀
int flag1 = 1; // 判断=类型
int n = 0;

FILE* fp;
char ch; // 读取文件当前字符
string danci = "";//当前字符串

int nodeNum;
map<string, int> nodeNUM;
vector<string> express;//存储表达式
vector<string> symbol;//存储表达式前缀
string set_to_str(set<int> se);
map<int, map< string, set<int> > > M;//NFA存储
map<string, map<int, map< string, set<int> > > > MZ;//所有表达式的NFA
set<string> edgeData;//边包含的内容存储
map<string, set<string> > edge;//所有表达式的边内容存储
int nhead = -1, ntail = -1;//头尾节点
map<string, pair<int, int> > HT; //存所有表达式NFA头尾节点

map<set<int>, map<string, set<int> > > DM;//用来储存DFA
map<string, map<set<int>, map<string, set<int> > > > DMZ;//存所有表达式DFA
set<set<int> >allNode;//DFA全部节点
map<string, set<set<int> > > dallNode;//存所有表达式所有节点
map<set<int>, int> state;//存allNode集合中各节点的状态信息
map<string, map<set<int>, int> > dstate;//存所有表达式allNode集合中各节点的状态信息

map<set<int>, int> m;//给dfa节点一个简单的序号

int dfanum = 0;//dfa节点数
map<int, map<string, int> > NM;//名字简化后的dfa对应关系

map<int, map<string, int> > SM;//最小化的dfa对应关系
map<string, map<int, map<string, int> > > SZ;//最小化dfa关系组
vector<set<int> > T;//用来存同类集合
map<int, int> sstate;//化简后状态
map<string, map<int, int> > states;//存储状态组
int num = -1;//化简后节点数
map<string, int> ns; //存储所有表达式化简节点数
int last_danci = 0, now_danci = 0;

void shuc() {
    for (int i = 0; i < express.size(); i++)
        debugcout << express[i] << " ";
    to_NFA();
    MZ[symbol[n]] = M;
    HT[symbol[n]] = make_pair(nhead, ntail);
    edge[symbol[n]] = edgeData;
    nodeNUM[symbol[n]] = nodeNum;
    to_DFA();
    DMZ[symbol[n]] = DM;
    dallNode[symbol[n]] = allNode;
    dstate[symbol[n]] = state;
    simplifyDFA();//节点化简
    simpDFA();//化简
    simstate();//获取状态
    SZ[symbol[n]] = SM;
    ns[symbol[n]] = num + 1;
    states[symbol[n]] = sstate;
    n++;
    M.clear();
    nodeNum = 0;
    edgeData.clear();
    nhead = -1;
    ntail = -1;
    DM.clear();
    allNode.clear();
    state.clear();
    m.clear();
    dfanum = 0;
    NM.clear();
    SM.clear();
    T.clear();
    sstate.clear();
    num = -1;
    express.clear();
}

void to_NFA() {
    stack<pair<int, int> > S;//存头尾
    stack<string> P;//存符号
    for (int i = 0; i < express.size(); i++) {
        if (express[i] == ")") {
            string expression = P.top();
            P.pop();
            while (expression != "(") {
                if (expression == "&") {
                    debugcout << "&" << " ";
                    int head1 = S.top().first, tail1 = S.top().second;//连接符后面节点
                    S.pop();
                    int head2 = S.top().first, tail2 = S.top().second;
                    S.pop();
                    M[tail2]["~"].insert(head1);
                    S.push(make_pair(head2, tail1));
                }
                else if (expression == "|") {
                    debugcout << "|" << " ";
                    for (int j = 0; j < 2; j++)
                    {
                        M[nodeNum + 1]["~"].insert(S.top().first);
                        M[S.top().second]["~"].insert(nodeNum + 2);
                        S.pop();
                    }
                    S.push(make_pair(nodeNum + 1, nodeNum + 2));
                    nodeNum = nodeNum + 2;
                }
                else if (expression == "*") {
                    debugcout << "*" << " ";
                    M[nodeNum + 1]["~"].insert(S.top().first);
                    M[S.top().second]["~"].insert(nodeNum + 2);
                    M[S.top().second]["~"].insert(S.top().first);
                    M[nodeNum + 1]["~"].insert(nodeNum + 2);
                    S.pop();
                    S.push(make_pair(nodeNum + 1, nodeNum + 2));
                    nodeNum = nodeNum + 2;
                }
                else if (expression == "?") {
                    M[nodeNum + 1]["~"].insert(S.top().first);
                    M[S.top().second]["~"].insert(nodeNum + 2);
                    M[nodeNum + 1]["~"].insert(nodeNum + 2);
                    S.pop();
                    S.push(make_pair(nodeNum + 1, nodeNum + 2));
                    nodeNum = nodeNum + 2;
                }
                else if (expression == "+") {
                    M[nodeNum + 1]["~"].insert(S.top().first);
                    M[S.top().second]["~"].insert(nodeNum + 2);
                    M[S.top().second]["~"].insert(S.top().first);
                    S.pop();
                    S.push(make_pair(nodeNum + 1, nodeNum + 2));
                    nodeNum = nodeNum + 2;
                }
                else {
                    debugcout << "error";
                    return;
                }
                expression = P.top();
                P.pop();
            }
        }
        else if (express[i] == "(") {
            debugcout << "(" << " ";
            P.push(express[i]);
        }
        else if (express[i] == "&" || express[i] == "|" || express[i] == "*" || express[i] == "+" || express[i] == "?") {
            if (P.empty()) {
                P.push(express[i]);
                continue;
            }
            while (compera(express[i]) <= compera(P.top()) && P.top() != "(") {
                if (P.empty()) {
                    P.push(express[i]);
                    break;
                }
                string expression = P.top();
                P.pop();
                if (expression == "&") {
                    debugcout << "&" << " ";
                    int head1 = S.top().first, tail1 = S.top().second;//连接符后面节点
                    S.pop();
                    int head2 = S.top().first, tail2 = S.top().second;
                    S.pop();
                    M[tail2]["~"].insert(head1);
                    S.push(make_pair(head2, tail1));
                }
                else if (expression == "|") {
                    debugcout << "|" << " ";
                    for (int j = 0; j < 2; j++)
                    {
                        M[nodeNum + 1]["~"].insert(S.top().first);
                        M[S.top().second]["~"].insert(nodeNum + 2);
                        S.pop();
                    }
                    S.push(make_pair(nodeNum + 1, nodeNum + 2));
                    nodeNum = nodeNum + 2;
                }
                else if (expression == "*") {
                    debugcout << "*" << " ";
                    M[nodeNum + 1]["~"].insert(S.top().first);
                    M[S.top().second]["~"].insert(nodeNum + 2);
                    M[S.top().second]["~"].insert(S.top().first);
                    M[nodeNum + 1]["~"].insert(nodeNum + 2);
                    S.pop();
                    S.push(make_pair(nodeNum + 1, nodeNum + 2));
                    nodeNum = nodeNum + 2;
                }
                else if (expression == "?") {
                    debugcout << "?" << " ";
                    M[nodeNum + 1]["~"].insert(S.top().first);
                    M[S.top().second]["~"].insert(nodeNum + 2);
                    M[nodeNum + 1]["~"].insert(nodeNum + 2);
                    S.pop();
                    S.push(make_pair(nodeNum + 1, nodeNum + 2));
                    nodeNum = nodeNum + 2;
                }
                else if (expression == "+") {
                    debugcout << "+" << " ";
                    M[nodeNum + 1]["~"].insert(S.top().first);
                    M[S.top().second]["~"].insert(nodeNum + 2);
                    M[S.top().second]["~"].insert(S.top().first);
                    S.pop();
                    S.push(make_pair(nodeNum + 1, nodeNum + 2));
                    nodeNum = nodeNum + 2;
                }
                if (P.empty()) {
                    P.push(express[i]);
                    break;
                }
            }
            if (compera(express[i]) > compera(P.top()))
                P.push(express[i]);
        }
        else {
            debugcout << express[i] << " ";
            M[nodeNum + 1][express[i]].insert(nodeNum + 2);
            S.push(make_pair(nodeNum + 1, nodeNum + 2));
            nodeNum = nodeNum + 2;
            edgeData.insert(express[i]);
        }
    }
    while (!P.empty()) {
        string expression = P.top();
        P.pop();
        if (expression == "&") {
            debugcout << "&" << " ";
            int head1 = S.top().first, tail1 = S.top().second;//连接符后面节点
            S.pop();
            int head2 = S.top().first, tail2 = S.top().second;
            S.pop();
            M[tail2]["~"].insert(head1);
            S.push(make_pair(head2, tail1));
        }
        else if (expression == "|") {
            debugcout << "|" << " ";
            for (int j = 0; j < 2; j++)
            {
                M[nodeNum + 1]["~"].insert(S.top().first);
                M[S.top().second]["~"].insert(nodeNum + 2);
                S.pop();
            }
            S.push(make_pair(nodeNum + 1, nodeNum + 2));
            nodeNum = nodeNum + 2;
        }
        else if (expression == "*") {
            debugcout << "*" << " ";
            M[nodeNum + 1]["~"].insert(S.top().first);
            M[S.top().second]["~"].insert(nodeNum + 2);
            M[S.top().second]["~"].insert(S.top().first);
            M[nodeNum + 1]["~"].insert(nodeNum + 2);
            S.pop();
            S.push(make_pair(nodeNum + 1, nodeNum + 2));
            nodeNum = nodeNum + 2;
        }
        else if (expression == "?") {
            debugcout << "?" << " ";
            M[nodeNum + 1]["~"].insert(S.top().first);
            M[S.top().second]["~"].insert(nodeNum + 2);
            M[nodeNum + 1]["~"].insert(nodeNum + 2);
            S.pop();
            S.push(make_pair(nodeNum + 1, nodeNum + 2));
            nodeNum = nodeNum + 2;
        }
        else if (expression == "+") {
            debugcout << "+" << " ";
            M[nodeNum + 1]["~"].insert(S.top().first);
            M[S.top().second]["~"].insert(nodeNum + 2);
            M[S.top().second]["~"].insert(S.top().first);
            S.pop();
            S.push(make_pair(nodeNum + 1, nodeNum + 2));
            nodeNum = nodeNum + 2;
        }
        else {
            debugcout << "error";
        }
    }
    nhead = S.top().first;
    ntail = S.top().second;
    S.pop();
}

void shuchu() {
    if (flag == 1)
    {
        symbol.push_back(danci);
    }
    else {
        if (danci == "=" && flag1 == 1) {
            danci = "";
            last_danci = now_danci;
            return;

        }
        if ((last_danci == 1 && now_danci == 1) || (last_danci == 2 && now_danci == 1) || (last_danci == 1 && now_danci == 3) || (last_danci == 2 && now_danci == 3)) {
            express.push_back("&");
            express.push_back(danci);
        }
        else
            express.push_back(danci);
    }
    danci = "";
    last_danci = now_danci;
}

void match() {
    switch (ch) {
    case '_':
    case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g': case 'h':
    case 'i': case 'j': case 'k': case 'l': case 'm': case 'n': case 'o': case 'p':
    case 'q': case 'r': case 's': case 't': case 'u': case 'v': case 'w': case 'x': case 'y':
    case 'z':
    case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G': case 'H':
    case 'I': case 'J': case 'K': case 'L': case 'M': case 'N': case 'O': case 'P':
    case 'Q': case 'R': case 'S': case 'T': case 'U': case 'V': case 'W': case 'X': case 'Y':
    case 'Z': {
        danci = danci + ch;
        ch = fgetc(fp);
        judge_danci();
        break;
    }
    case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9': {
        danci = danci + ch;
        ch = fgetc(fp);
        judge_number();
        break;
    }
    case '\\': {
        danci = danci + ch;
        ch = fgetc(fp);
        danci = danci + ch;
        now_danci = 1;
        shuchu();
        break;
    }
    case '-':
    case '<':
    case '>':
    case '!':
    case '#':
    case '[':
    case ']':
    case ',':
    case '^':
    case ';':
    case '{':
    case '%':
    case '}':
    case '/':
    case ':': {
        danci = danci + ch;
        now_danci = 1;
        shuchu();
        break;
    }

    case '&':
    case '|': {
        danci = danci + ch;
        now_danci = 0;
        shuchu();
        break;
    }
    case '*':
    case '+':
    case '?': {
        danci = danci + ch;
        now_danci = 2;
        shuchu();
        break;
    }
    case ')': {
        danci = danci + ch;
        now_danci = 2;
        shuchu();
        break;
    }
    case '(': {
        danci = danci + ch;
        now_danci = 3;
        shuchu();
        break;
    }
    case '.': {
        danci = danci + ch;
        now_danci = 1;
        shuchu();
        break;
    }
    case ' ': {
        break;
    }
    case '\n': {
        debugcout << endl;
        flag = 1;
        flag1 = 1;
        now_danci = 0;
        shuc();
        break;
    }
    case '=': {
        danci = danci + ch;
        if (flag == 1) {
            flag = 0;
            now_danci = 0;
        }
        else {
            flag1 = 0;
            now_danci = 1;
        }
        shuchu();
        break;
    }
    default: {
        break;
    }
    }
}

void judge_number() {
    switch (ch) {
    case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
    case '.': {
        danci = danci + ch;
        ch = fgetc(fp);
        judge_number();
        break;
    }
    default: {
        now_danci = 1;
        shuchu();
        match();
        break;
    }
    }
}

void judge_danci() {
    switch (ch) {
    case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g': case 'h':
    case 'i': case 'j': case 'k': case 'l': case 'm': case 'n': case 'o': case 'p':
    case 'q': case 'r': case 's': case 't': case 'u': case 'v': case 'w': case 'x': case 'y':
    case 'z':
    case '_':
    case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
    case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G': case 'H':
    case 'I': case 'J': case 'K': case 'L': case 'M': case 'N': case 'O': case 'P':
    case 'Q': case 'R': case 'S': case 'T': case 'U': case 'V': case 'W': case 'X': case 'Y':
    case 'Z': {
        danci = danci + ch;
        ch = fgetc(fp);
        judge_danci();
        break;
    }
    default: {
        now_danci = 1;
        shuchu();
        match();
        break;
    }
    }
}

int compera(string oper)
{
    if (oper == "*" || oper == "?" || oper == "+")
        return 3;
    else if (oper == "&")
        return 2;
    else if (oper == "|")
        return 1;
    else
        return 0;
}

void to_DFA()
{
    set<int>temp;
    queue<set<int> > Q;
    temp = M[nhead]["~"];//该S存与head相同点集合
    temp.insert(nhead);//把自己加进去
    queue<int> Qg;
    for (set<int>::iterator it = M[nhead]["~"].begin(); it != M[nhead]["~"].end(); it++)
    {
        Qg.push(*it);//将集合中的每个数字记录

    }
    while (!Qg.empty())//深度找闭包
    {
        for (set <int>::iterator it = M[Qg.front()]["~"].begin(); it != M[Qg.front()]["~"].end(); it++)
        {
            if (temp.count(*it) == 0)//该闭包不在集合中
            {
                temp.insert(*it);
                Qg.push(*it);
            }
        }
        Qg.pop();
    }
    //head的闭包找完temp
    Q.push(temp);
    allNode.insert(temp);
    temp.clear();
    while (!Q.empty())
    {
        //接下来以前面得到的集合找对应尾集合
        for (set<string>::iterator c = edgeData.begin(); c != edgeData.end(); c++)//读取每一个存在的字符
        {
            set<int> ts;
            ts.clear();
            //接下来对头节点集合中的各字符找对应尾节点
            for (set<int>::iterator it = Q.front().begin(); it != Q.front().end(); it++)
            {
                ts.insert(M[*it][*c].begin(), M[*it][*c].end());//将能够通过字符*c到达的顶点记录下来
                //接下来找ts里集合的闭包,如果为空便记录
                if (ts.size() != 0)
                {
                    //先放到队列，做深度
                    for (set<int>::iterator it = ts.begin(); it != ts.end(); it++)
                    {
                        Qg.push(*it);//将集合中的每个数字记录
                    }
                    while (!Qg.empty())
                    {
                        for (set<int>::iterator it = M[Qg.front()]["~"].begin(); it != M[Qg.front()]["~"].end(); it++)
                        {
                            if (ts.count(*it) == 0)//该闭包不在集合中
                            {
                                ts.insert(*it);
                                Qg.push(*it);
                            }

                        }
                        Qg.pop();

                    }
                }
            }
            if (ts.size() != 0)
            {
                DM[Q.front()][*c] = ts;//存进dfa表中
                if (allNode.count(ts) == 0)//判断是否已经做过起点
                {
                    allNode.insert(ts);//放入节点数组
                    Q.push(ts);//放入队列
                }
            }
        }
        Q.pop();
    }
}

string set_to_str(set<int> se)
{
    string str = "{";
    for (set<int>::iterator it = se.begin(); it != se.end(); it++)
    {
        if (it == se.begin())
        {
            str = str + to_string(*it);
        }
        else
        {
            str = str + ',' + to_string(*it);
        }
    }
    str = str + '}';
    return str;
}

//判断是不是终态
void judgeState()
{
    for (set<set<int> >::iterator it = allNode.begin(); it != allNode.end(); it++)//循环遍历未简化dfa节点
    {
        if ((*it).count(ntail) == 0)//说明不是终态
        {
            state[*it] = 0;
        }
        else
        {
            state[*it] = 1;//终态
        }
    }
}

//简化dfa节点序号
void simplifyDFA()
{
    for (set<set<int> >::iterator it = allNode.begin(); it != allNode.end(); it++) {
        dfanum = dfanum + 1;
        m[*it] = dfanum;
    }

    for (set<set<int> >::iterator it1 = allNode.begin(); it1 != allNode.end(); it1++)
        for (set<string>::iterator i = edgeData.begin(); i != edgeData.end(); i++)
        {
            if (DM[*it1][*i].size() != 0)//判断边是否存在
            {
                NM[m[*it1]][*i] = m[DM[*it1][*i]];
            }
        }
}

//查找数字a在集合数组的位置
int findpos(int a, vector<set<int> > V, int num)
{
    for (int i = 0; i <= num; i++) {
        if (V[i].count(a) != 0)
            return i;
    }
    return -1;
}

int trans(int k)
{
    for (int i = 0; i <= num; i++) {
        if (T[i].count(k) > 0)
        {
            return i;
        }
    }
    return -1;
}

//最小化DFA
void simpDFA()
{
    judgeState();
    int position;
    int i;
    set<int> t1;
    set<int> t2;

    set<int> ST;//暂存
    map<int, set<int> > temp;

    //先找到终态集合和非终态集合
    for (set<set<int> >::iterator it = allNode.begin(); it != allNode.end(); it++)
    {
        if (state[*it] == 0)
        {
            t1.insert(m[*it]);
        }
        else
        {
            t2.insert(m[*it]);
        }
    }
    debugcout << set_to_str(t1) << "  " << set_to_str(t2) << endl;
    T.push_back(t1);
    T.push_back(t2);
    num = num + 2;
    for (i = 0; i <= num; i++)//T中每个集合
    {
        if (T[i].size() == 1)//当该集合只有一个值，不用拆分
        {
            continue;
        }
        for (set<string>::iterator j = edgeData.begin(); j != edgeData.end(); j++)//每个字符
        {
            ST.clear();
            temp.clear();
            for (set<int>::iterator it = T[i].begin(); it != T[i].end(); it++)//集合中每个节点
            {
                position = findpos(NM[*it][*j], T, num);//尾节点在T中的位置

                temp[position].insert(*it);//通过映射将各不同分组放到不同集合里
                ST.insert(position);//记录分组position
            }
            if (ST.size() > 1)//说明有两个及以上的分组
            {
                for (set<int>::iterator it1 = ST.begin(); it1 != ST.end(); it1++)
                {
                    T.push_back(temp[*it1]);
                }
                T.erase(T.begin() + i);
                num = T.size() - 1;
                i = -1;//以新的集合重头开始
                break;
            }
        }
    }
    for (int i = 0; i < T.size(); i++)
    {
        if (T[i].size() == 0)
        {
            T.erase(T.begin() + i);
            num--;
        }
    }

    for (int i = 0; i <= num; i++)
    {
        debugcout << set_to_str(T[i]) << endl;
    }

    //最终放入SM中
    for (int i = 0; i <= num; i++)
    {
        for (set<string>::iterator j = edgeData.begin(); j != edgeData.end(); j++)
        {
            if (trans(NM[*(T[i]).begin()][*j]) != -1)
                SM[i + 1][*j] = trans(NM[*(T[i]).begin()][*j]) + 1;

        }
    }
}

void simstate()//化简后的节点初始状态
{
    map<int, int> bestate;
    for (set<set<int> >::iterator it = allNode.begin(); it != allNode.end(); it++)
    {
        if ((*it).count(nhead) >= 1 && (*it).count(ntail) >= 1)
        {
            bestate[m[*it]] = 3;//说明既是终态又是初态
        }
        else if ((*it).count(nhead) >= 1)
        {
            bestate[m[*it]] = 1;//说明是初态
        }
        else if ((*it).count(ntail) >= 1)
        {
            bestate[m[*it]] = 2;//说明是终态
        }
        else
        {
            bestate[m[*it]] = 0;
        }
    }
    for (int i = 0; i < T.size(); i++)
    {
        for (set<int>::iterator it = T[i].begin(); it != T[i].end(); it++)
        {
            if (sstate[i + 1] < bestate[*it])
                sstate[i + 1] = bestate[*it];
        }
    }
}
