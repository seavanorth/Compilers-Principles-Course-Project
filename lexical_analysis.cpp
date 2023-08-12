#include "stdafx.h"
#include "lexical_analysis.h"

int flag = 1;// �ж�ǰ׺
int flag1 = 1; // �ж�=����
int n = 0;

FILE* fp;
char ch; // ��ȡ�ļ���ǰ�ַ�
string danci = "";//��ǰ�ַ���

int nodeNum;
map<string, int> nodeNUM;
vector<string> express;//�洢���ʽ
vector<string> symbol;//�洢���ʽǰ׺
string set_to_str(set<int> se);
map<int, map< string, set<int> > > M;//NFA�洢
map<string, map<int, map< string, set<int> > > > MZ;//���б��ʽ��NFA
set<string> edgeData;//�߰��������ݴ洢
map<string, set<string> > edge;//���б��ʽ�ı����ݴ洢
int nhead = -1, ntail = -1;//ͷβ�ڵ�
map<string, pair<int, int> > HT; //�����б��ʽNFAͷβ�ڵ�

map<set<int>, map<string, set<int> > > DM;//��������DFA
map<string, map<set<int>, map<string, set<int> > > > DMZ;//�����б��ʽDFA
set<set<int> >allNode;//DFAȫ���ڵ�
map<string, set<set<int> > > dallNode;//�����б��ʽ���нڵ�
map<set<int>, int> state;//��allNode�����и��ڵ��״̬��Ϣ
map<string, map<set<int>, int> > dstate;//�����б��ʽallNode�����и��ڵ��״̬��Ϣ

map<set<int>, int> m;//��dfa�ڵ�һ���򵥵����

int dfanum = 0;//dfa�ڵ���
map<int, map<string, int> > NM;//���ּ򻯺��dfa��Ӧ��ϵ

map<int, map<string, int> > SM;//��С����dfa��Ӧ��ϵ
map<string, map<int, map<string, int> > > SZ;//��С��dfa��ϵ��
vector<set<int> > T;//������ͬ�༯��
map<int, int> sstate;//�����״̬
map<string, map<int, int> > states;//�洢״̬��
int num = -1;//�����ڵ���
map<string, int> ns; //�洢���б��ʽ����ڵ���
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
    simplifyDFA();//�ڵ㻯��
    simpDFA();//����
    simstate();//��ȡ״̬
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
    stack<pair<int, int> > S;//��ͷβ
    stack<string> P;//�����
    for (int i = 0; i < express.size(); i++) {
        if (express[i] == ")") {
            string expression = P.top();
            P.pop();
            while (expression != "(") {
                if (expression == "&") {
                    debugcout << "&" << " ";
                    int head1 = S.top().first, tail1 = S.top().second;//���ӷ�����ڵ�
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
                    int head1 = S.top().first, tail1 = S.top().second;//���ӷ�����ڵ�
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
            int head1 = S.top().first, tail1 = S.top().second;//���ӷ�����ڵ�
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
    temp = M[nhead]["~"];//��S����head��ͬ�㼯��
    temp.insert(nhead);//���Լ��ӽ�ȥ
    queue<int> Qg;
    for (set<int>::iterator it = M[nhead]["~"].begin(); it != M[nhead]["~"].end(); it++)
    {
        Qg.push(*it);//�������е�ÿ�����ּ�¼

    }
    while (!Qg.empty())//����ұհ�
    {
        for (set <int>::iterator it = M[Qg.front()]["~"].begin(); it != M[Qg.front()]["~"].end(); it++)
        {
            if (temp.count(*it) == 0)//�ñհ����ڼ�����
            {
                temp.insert(*it);
                Qg.push(*it);
            }
        }
        Qg.pop();
    }
    //head�ıհ�����temp
    Q.push(temp);
    allNode.insert(temp);
    temp.clear();
    while (!Q.empty())
    {
        //��������ǰ��õ��ļ����Ҷ�Ӧβ����
        for (set<string>::iterator c = edgeData.begin(); c != edgeData.end(); c++)//��ȡÿһ�����ڵ��ַ�
        {
            set<int> ts;
            ts.clear();
            //��������ͷ�ڵ㼯���еĸ��ַ��Ҷ�Ӧβ�ڵ�
            for (set<int>::iterator it = Q.front().begin(); it != Q.front().end(); it++)
            {
                ts.insert(M[*it][*c].begin(), M[*it][*c].end());//���ܹ�ͨ���ַ�*c����Ķ����¼����
                //��������ts�Ｏ�ϵıհ�,���Ϊ�ձ��¼
                if (ts.size() != 0)
                {
                    //�ȷŵ����У������
                    for (set<int>::iterator it = ts.begin(); it != ts.end(); it++)
                    {
                        Qg.push(*it);//�������е�ÿ�����ּ�¼
                    }
                    while (!Qg.empty())
                    {
                        for (set<int>::iterator it = M[Qg.front()]["~"].begin(); it != M[Qg.front()]["~"].end(); it++)
                        {
                            if (ts.count(*it) == 0)//�ñհ����ڼ�����
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
                DM[Q.front()][*c] = ts;//���dfa����
                if (allNode.count(ts) == 0)//�ж��Ƿ��Ѿ��������
                {
                    allNode.insert(ts);//����ڵ�����
                    Q.push(ts);//�������
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

//�ж��ǲ�����̬
void judgeState()
{
    for (set<set<int> >::iterator it = allNode.begin(); it != allNode.end(); it++)//ѭ������δ��dfa�ڵ�
    {
        if ((*it).count(ntail) == 0)//˵��������̬
        {
            state[*it] = 0;
        }
        else
        {
            state[*it] = 1;//��̬
        }
    }
}

//��dfa�ڵ����
void simplifyDFA()
{
    for (set<set<int> >::iterator it = allNode.begin(); it != allNode.end(); it++) {
        dfanum = dfanum + 1;
        m[*it] = dfanum;
    }

    for (set<set<int> >::iterator it1 = allNode.begin(); it1 != allNode.end(); it1++)
        for (set<string>::iterator i = edgeData.begin(); i != edgeData.end(); i++)
        {
            if (DM[*it1][*i].size() != 0)//�жϱ��Ƿ����
            {
                NM[m[*it1]][*i] = m[DM[*it1][*i]];
            }
        }
}

//��������a�ڼ��������λ��
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

//��С��DFA
void simpDFA()
{
    judgeState();
    int position;
    int i;
    set<int> t1;
    set<int> t2;

    set<int> ST;//�ݴ�
    map<int, set<int> > temp;

    //���ҵ���̬���Ϻͷ���̬����
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
    for (i = 0; i <= num; i++)//T��ÿ������
    {
        if (T[i].size() == 1)//���ü���ֻ��һ��ֵ�����ò��
        {
            continue;
        }
        for (set<string>::iterator j = edgeData.begin(); j != edgeData.end(); j++)//ÿ���ַ�
        {
            ST.clear();
            temp.clear();
            for (set<int>::iterator it = T[i].begin(); it != T[i].end(); it++)//������ÿ���ڵ�
            {
                position = findpos(NM[*it][*j], T, num);//β�ڵ���T�е�λ��

                temp[position].insert(*it);//ͨ��ӳ�佫����ͬ����ŵ���ͬ������
                ST.insert(position);//��¼����position
            }
            if (ST.size() > 1)//˵�������������ϵķ���
            {
                for (set<int>::iterator it1 = ST.begin(); it1 != ST.end(); it1++)
                {
                    T.push_back(temp[*it1]);
                }
                T.erase(T.begin() + i);
                num = T.size() - 1;
                i = -1;//���µļ�����ͷ��ʼ
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

    //���շ���SM��
    for (int i = 0; i <= num; i++)
    {
        for (set<string>::iterator j = edgeData.begin(); j != edgeData.end(); j++)
        {
            if (trans(NM[*(T[i]).begin()][*j]) != -1)
                SM[i + 1][*j] = trans(NM[*(T[i]).begin()][*j]) + 1;

        }
    }
}

void simstate()//�����Ľڵ��ʼ״̬
{
    map<int, int> bestate;
    for (set<set<int> >::iterator it = allNode.begin(); it != allNode.end(); it++)
    {
        if ((*it).count(nhead) >= 1 && (*it).count(ntail) >= 1)
        {
            bestate[m[*it]] = 3;//˵��������̬���ǳ�̬
        }
        else if ((*it).count(nhead) >= 1)
        {
            bestate[m[*it]] = 1;//˵���ǳ�̬
        }
        else if ((*it).count(ntail) >= 1)
        {
            bestate[m[*it]] = 2;//˵������̬
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
