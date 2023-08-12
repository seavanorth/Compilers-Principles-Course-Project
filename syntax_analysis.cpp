#include "stdafx.h"
#include "syntax_analysis.h"

vector<string> Terminator;//�ս���ż���
vector<string> NT;//���ս���ż���
string S;//��ʼ����
map<string, vector<string> > production;//����ʽ
map<string, set<string> > FIRST;//FIRST��
map<string, set<string> > FOLLOW;//FOLLOW��
map<pair<string, string>, string> Table;//LL(1)�ķ�������
QString fileName;

//�������ǰ׺
string maxPrefix(string left)
{
    string prefix = "";
    vector<string>P = production[left];
    string firstP = P[0];
    firstP += " ";

    while (firstP.find(" ") != firstP.size() - 1)
    {
        string temp = firstP.substr(0, firstP.find(" "));//left�ĵ�һ������ʽ�е�token
        //debugcout<<"��һ�������еĵ�һ��token��"<<temp<<"\t";

        //����left����������ʽ
        for (int i = 1; i < P.size(); i++)
        {
            string right = P[i];
            right += " ";

            string cmp = right.substr(0, right.find(" "));
            //debugcout<<"�������ʽ�е�һ��token��"<<cmp<<"\t";

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

    //ȥ��ĩβ�ո�
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

//����ֱ����ݹ�
void immediateLeftRecursionRemoval(string Ai)
{
    string newNT = Ai + "'";//�µķ��ս����
    NT.insert(find(NT.begin(), NT.end(), Ai) + 1, newNT);
    vector<string>newFirstRight;//�µĲ���ʽ�Ҳ�
    vector<string>newSecondRight;
    for (auto it = production[Ai].begin(); it < production[Ai].end(); it++)
    {
        string right = *it;
        string temp = right;
        temp += "#";

        //����ֱ����ݹ�:Ai'->aAi'|@
        if (strcmp(const_cast<char*>(Ai.c_str()), strtok(const_cast<char*>(temp.c_str()), " #")) == 0)
        {
            right = right.substr(right.find(" ") + 1) + " " + newNT;
            newSecondRight.push_back(right);
        }
        //������Ai->BAi'
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

//�����ʽ
void addP(string left, string right)
{
    right += "#";//'#'��Ϊÿ���ķ���β��־
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

//���ս��
void addT()
{
    string temp = "";
    for (string left : NT) {
        for (string right : production[left]) {
           right += "#";
           for (int i = 0; i < right.size(); i++) {
               if (right[i] == '|' || right[i] == ' ' || right[i] == '#') {
                   //���Ƿ��սᣬ�Ҳ��ǿգ�������ս����
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


    //�ս��ȥ��
    sort(Terminator.begin(), Terminator.end());
    Terminator.erase(unique(Terminator.begin(), Terminator.end()), Terminator.end());
}

//��ȡ�ķ�����
void readGrammar(string fileName)
{
    ifstream input(fileName);
    if (!input) {
        debugcout << fileName << " Failed" << endl;
    }

    //��ȡ�ķ�����
    string line;//�����ÿһ��
    while (getline(input, line)) {
        int i;

        //��ȡ��
        string left = "";
        for (i = 0; line[i] != '-' && i < line.size(); i++) {
            left += line[i];
        }

        NT.push_back(left);//�󲿼�����ս���ż�

        //��ȡ�Ҳ�
        string right = line.substr(i + 2, line.size() - i);//��ȡ����ʽ�Ҳ�
        addP(left, right);//��Ӳ���ʽ
    }
    addT();//����ռ���
    S = *NT.begin();
    input.close();
}

//������ݹ�
void leftRecursionRemoval()
{
    //����ÿһ��NT
    for (int i = 0; i < NT.size(); i++)
    {
        string Ai = NT[i];
        //debugcout<<"Ai:"<<Ai<<endl;
        vector<string>newRight;//�µĲ���ʽ�Ҳ�

        //����NT��ÿһ������ʽ
        for (auto it = production[Ai].begin(); it < production[Ai].end(); it++)
        {
            string right = *it;
            //debugcout<<"right:"<<right<<endl;
            int flag = 0;//�ж��ǲ�����ݹ�

            //�����ı���Ĳ���ʽ
            for (int j = 0; j < i; j++)
            {
                string Aj = NT[j];
                //debugcout<<"Aj:"<<Aj<<endl;
                string temp = right + "#";

                //�����Ai->AjB���滻AjΪAj�Ĳ���ʽ
                if (strcmp(const_cast<char*>(Aj.c_str()), strtok(const_cast<char*>(temp.c_str()), " #")) == 0)
                {
                    flag = 1;
                    debugcout << Aj << " ";
                    debugcout << temp << endl;
                    for (auto jt = production[Aj].begin(); jt < production[Aj].end(); jt++)
                    {
                        string s = *jt + " " + right.substr(right.find(" ") + 1);//substr(1)�Ǵӿո�λ��������Ӵ�
                        //debugcout<<"s:"<<s<<endl;
                        newRight.push_back(s);
                    }
                }
            }
            //û�п��滻�Ĳ���ʽ
            if (flag == 0)
                newRight.push_back(right);
        }
        if (i != 0)
            production[Ai] = newRight;

        //ȥ������Ai��ֱ����ݹ�
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

//��ȡ������
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

            vector<string>newRight1;//A�Ĳ���ʽ
            vector<string>newRight2;//A'�Ĳ���ʽ
            for (auto it = production[left].begin(); it < production[left].end(); it++)
            {
                string right = *it;
                string newRight;

                //����ʽ����a,ֱ�ӷŽ�A�Ĳ���ʽ��
                if (right.find(a) == string::npos)
                    newRight1.push_back(right);

                //����ʽ��a
                else
                {
                    if (right.find(a) + a.size() != right.size())
                    {
                        newRight = right.substr(right.find(a) + a.size() + 1);
                    }
                    //a�����ǿյ�
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

//���FIRST����
void getFirst()
{
    FIRST.clear();

    //�ս���Ż�@
    FIRST["@"].insert("@");
    for (string X : Terminator)
    {
        FIRST[X].insert(X);
    }

    //���ս����
    int j = 0;
    while (j < 5)
    {
        for (int i = 0; i < NT.size(); i++)
        {
            string A = NT[i];

            //����A��ÿ������ʽ
            for (int k = 0; k < production[A].size(); k++)
            {
                int Continue = 1;//�Ƿ����@
                string right = production[A][k];

                //X��ÿ������ʽ��һ��token
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

//���FOLLOW����
void getFollow()
{
    //��������뿪ʼ���ŵ�follow��
    FOLLOW[S].insert("#");

    int j = 0;
    while (j < 5)
    {
        //�������ս����
        for (string A : NT)
        {
            for (string right : production[A])
            {
                for (string B : NT)
                {
                    //A->Bb
                    if (right.find(B) != string::npos)
                    {
                        /*��B����ַ�b*/
                        string b;
                        int flag = 0;
                        //ʶ��E'
                        if (right[right.find(B) + B.size()] != ' ' && right[right.find(B) + B.size()] != '\0')
                        {
                            string s = right.substr(right.find(B));//E'b
                            string temp = right.substr(right.find(B) + B.size());//' b

                            //A->E'
                            if (temp.find(" ") == string::npos)
                            {
                                B = s;//B:E->E'
                                FOLLOW[B].insert(FOLLOW[A].begin(), FOLLOW[A].end());//�󲿵�FOLLOW����B
                                flag = 1;
                            }
                            //A->E'b
                            else
                            {
                                B = s.substr(0, s.find(" "));
                                temp = temp.substr(temp.find(" ") + 1);//b

                                //b�����ַ�
                                if (temp.find(" ") == string::npos)
                                    b = temp;
                                //b�����ַ�
                                else
                                    b = temp.substr(0, temp.find(" "));
                            }
                        }

                        //A->aEb
                        else if (right[right.find(B) + B.size()] == ' ')
                        {
                            string temp = right.substr(right.find(B) + B.size() + 1);//B����Ӵ�

                            //b�����ַ�
                            if (temp.find(" ") == string::npos)
                                b = temp;
                            //b�����ַ�
                            else
                                b = temp.substr(0, temp.find(" "));
                        }
                        //A->aE
                        else
                        {
                            FOLLOW[B].insert(FOLLOW[A].begin(), FOLLOW[A].end());
                            flag = 1;
                        }

                        //FOLLOW[B]��û��
                        if (flag == 0)
                        {
                            //FIRST[b]�в�����@
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

//�ж��������Ƿ��ཻ
int hasIntersection(set<string>first, set<string>second)
{
    for (string b : second) {
        //���first��second���ظ�Ԫ�أ����ཻ
        if (first.find(b) != first.end())
            return 1;
    }
    return 0;
}

void getTable()
{
    for (string A : NT) {
        for (string right : production[A]) {
            string first;//right���һ��token
            if (right.find(" ") == string::npos)
                first = right;
            else
                first = right.substr(0, right.find(" "));

            right = right.insert(0, A + "->");
            pair<string, string>symbol;

            //FIRST���ﲻ��@:a����FIRST[first]
            if (FIRST[first].find("@") == FIRST[first].end()) {
                for (string a : FIRST[first]) {
                    symbol = make_pair(A, a);
                    Table[symbol] = right;
                    //debugcout<<A<<"\t"<<a<<"\t"<<right<<endl;
                }
            }
            //FIRST���ﺬ@:a����FOLLOW[a]
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
