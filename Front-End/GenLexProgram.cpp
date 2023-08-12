#include <fstream>
#include <string>
#include <vector>
using namespace std;

struct DFAUnit {
    vector<
};

//制表
string to4place(string todo) {
    ifstream ifs;
    ifs.open(todo, ios::in);
    string line;
    string text = "";
    while (!ifs.eof()) {
        getline(ifs, line);
        for (char c : line) {
            if (c == '\t') {
                text += "    ";
            }
            else {
                text.push_back(c);
            }
        }
        text.push_back('\n');
    }
    ifs.close();

    return text;
}

// dfa转为二元组
string dfamatrix(DFAUnit& dfa, int id) {
    //dfa矩阵的列数
    int colCnt = dfa.chs.size();
    vector<char> characterMap(colCnt);
    string strCharacters = "char characters" + to_string(id) + "[]={";
    for (int i = 0; i < colCnt; i++) {
        char c = dfa.chs[i];
        characterMap[i] = c;
    }
    for (char c : dfa.chs) {
        strCharacters += "'" + changechar(c) + "',";
    }
    strCharacters += "'\\0'};\n";
    string strDFA = "int dfa" + to_string(id) + "[][" + to_string(colCnt) + "]={\n{";
    for (int i = 0; i < colCnt; i++) {
        strDFA += "-1,";
    }
    //把末尾多余的逗号删掉
    strDFA.pop_back();
    strDFA += "},\n";
    int stateCnt = dfa.allstate.size();
    for (int sid = 1; sid <= stateCnt; sid++) {
        string strdfa = "{";
        for (int chi = 0; chi < colCnt; chi++) {
            char c = characterMap[chi];
            int tid = -1;
            if (dfa.maps[sid].find(c) != dfa.maps[sid].end()) {
                tid = dfa.maps[sid][c];
            }
            strdfa += to_string(tid) + ",";
        }
        strdfa.pop_back();
        strdfa += "},\n";
        strDFA += strdfa;
    }
    strDFA.pop_back();
    strDFA += "};\n";
    string strFinals = "int finals" + to_string(id) + "[]={";
    for (int sid : dfa.ends) {
        strFinals += to_string(sid) + ",";
    }
    strFinals.pop_back();
    strFinals += "};\n";
    string str = strCharacters + strDFA + strFinals;
    return str;
}

string changechar(char c) {
    string rtn;
    switch (c)
    {
    case '\n':rtn = "\\n"; break;
    case '\r':rtn = "\\r"; break;
    case '\t':rtn = "\\t"; break;
    case '\v':rtn = "\\v"; break;
    case '\'':rtn = "\\'"; break;
    case '\0':rtn = "\\0"; break;
    default:rtn = string(1, c); break;
    }
    return rtn;
}

void save(string filename, string text) {
    ofstream ofs;
    ofs.open(filename, ios::out);
    ofs << text;
    ofs.close();
}

//生成词法分析程序
string lexicalAnalysis(string templateFile, vector<Reg>& regs, vector<DFAUnit>& dfas, DFAUnit& dfa, string testFile) {
    string count$ = "${count}";
    string count = to_string(regs.size());

    string names$ = "${names}";
    string names = "{";
    for (Reg r : regs) {
        names += "\"" + r.name + "\",";
    }
    //删除多余的逗号
    names.pop_back();
    names.push_back('}');

    string dfas$ = "${dfas}";
    string dfastring = dfamatrix(dfa, 0);
    for (int i = 0; i < dfas.size(); i++) {
        string s = dfamatrix(dfas[i], i + 1);
        dfastring += s;
    }

    string finalsCases$ = "${finalsCases}";
    string finalsCases = "";
    string charactersCases$ = "${charactersCases}";
    string charactersCases = "";
    string dfasCases$ = "${dfasCases}";
    string dfasCases = "";
    for (int i = 0; i <= regs.size(); i++) {
        finalsCases += "case " + to_string(i) + ":\n";
        finalsCases += "finals=finals" + to_string(i) + ";\n";
        finalsCases += "len=sizeof(finals" + to_string(i) + ")/sizeof(finals[0]);\n";
        finalsCases += "break; \n";
        charactersCases += "case " + to_string(i) + ":characters=characters" + to_string(i) + ";break;\n";
        dfasCases += "case " + to_string(i) + ":next=dfa" + to_string(i) + "[crt][pos];break;\n";
    }

    string filename$ = "${filename}";
    string filename = "\"" + testFile + "\"";
    string lexFile$ = "${lexFile}";
    string lexFile = "\"" + testFile.substr(0, testFile.find(".") - 0) + ".lex\"";

    //c source file
    string text = to4place(templateFile);

    text.replace(text.find(count$), count$.size(), count);
    text.replace(text.find(names$), names$.size(), names);
    text.replace(text.find(dfas$), dfas$.size(), dfastring);
    text.replace(text.find(finalsCases$), finalsCases$.size(), finalsCases);
    text.replace(text.find(charactersCases$), charactersCases$.size(), charactersCases);
    text.replace(text.find(dfasCases$), dfasCases$.size(), dfasCases);
    text.replace(text.find(filename$), filename$.size(), filename);
    text.replace(text.find(lexFile$), lexFile$.size(), lexFile);

    string sourceFile = templateFile.substr(0, templateFile.find(".")) + ".c";
    save(sourceFile, text);

    return text;
}