#include "stdafx.h"
#include "FrontEndWindow.h"
#include "lexical_analysis.h"
#include "syntax_analysis.h"

FrontEndWindow::FrontEndWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::FrontEndWindowClass())
{
    ui->setupUi(this);
    this->setAcceptDrops(true);
    this->readGuide();
    ui->tabWidget->setCurrentWidget(ui->widget_in_out);
    ui->tbw_LL1->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tbw_LL1->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

FrontEndWindow::~FrontEndWindow() // 析构函数
{
    delete ui;
}

// 1.0按钮--封装--开始整个词法分析
void FrontEndWindow::on_btn_start_scan_clicked()
{
    if (mRegExp.fileName() == NULL || mLexExe.fileName() == NULL || mSourceCode.fileName() == NULL) {
        QMessageBox::warning(this, "禁止", "缺少词法分析的必要输入", QMessageBox::Close);
        ui->tabWidget->setCurrentWidget(ui->widget_in_out); return;
    }
    if (mIsFirstHandleRegExp) {
        this->RegExp2MinDFA(); // 正则表达式-》MinDFA
        this->showStateTable(); // 查看不同单词状态表
        this->geneateLexCode();  // 生成词法分析程序
        mIsFirstHandleRegExp = false;
    }
    this->createTokenStream(); // 进行词法分析并生成token table
    ui->tabWidget->setCurrentWidget(ui->widget_lexical_analysis);
    ui->cbx_language->setEnabled(false);
}

// 1.1处理正则表达式，NFA，DFA，Min DFA
void FrontEndWindow::RegExp2MinDFA()
{
    danci = "";
    flag = 1;
    flag1 = 1;
    last_danci = 0;
    now_danci = 0;
    n = 0;
    MZ.clear();
    symbol.clear(); symbol.shrink_to_fit();
    express.clear(); express.shrink_to_fit();
    T.clear(); T.shrink_to_fit();
    nodeNUM.clear();
    edge.clear();
    HT.clear();
    DMZ.clear();
    dallNode.clear();
    dstate.clear();
    SZ.clear();
    states.clear();
    ns.clear(); // 以上为初始化

    std::string str = mCodec->fromUnicode(mRegExp.fileName()).data(); // QString to string
    const char* ptr = str.c_str();//将string to const char*
    fp = fopen(ptr, "r");
    ch = fgetc(fp);
    while (!feof(fp)) {
        if (ch == EOF)
            break;
        match();
        ch = fgetc(fp);
    }
    debugcout << endl;
    shuc();
    debugcout << "succful";
    ui->comboBox->clear();
    QStringList lab;
    lab.clear();
    for (int i = 0; i < symbol.size(); i++)
        lab << QString::fromStdString(symbol[i]);
    ui->comboBox->addItems(lab);
}

// 1.2显示NFA、DFA、MinDFA状态表
void FrontEndWindow::showStateTable()
{
    int q = ui->comboBox->currentIndex();

    // NFA以状态转换表呈现
    int line = nodeNUM[symbol[q]];
    int column = edge[symbol[q]].size();
    ui->tbw_nfa->setRowCount(line);
    ui->tbw_nfa->setColumnCount(column + 1);
    QStringList labels;
    labels.clear();
    set<string> NS;
    NS.clear();
    NS.insert(edge[symbol[q]].begin(), edge[symbol[q]].end());
    NS.insert("~");
    for (set<string>::iterator it = NS.begin(); it != NS.end(); it++) {
        labels << QString::fromStdString(*it);
    }
    ui->tbw_nfa->setHorizontalHeaderLabels(labels);
    for (int j = 1; j <= nodeNUM[symbol[q]]; j++) {
        int i = 0;
        for (set<string>::iterator it = NS.begin(); it != NS.end(); it++) {
            ui->tbw_nfa->setItem(j - 1, i++, new QTableWidgetItem(QString::fromStdString(set_to_str(MZ[symbol[q]][j][*it]))));
        }
    }

    //QTableWidget* ui->tbw_dfa = ui->ui->tbw_dfa;
    // DFA以状态转换表呈现
    line = dallNode[symbol[q]].size();
    column = edge[symbol[q]].size();
    ui->tbw_dfa->setRowCount(line);
    ui->tbw_dfa->setColumnCount(column);
    QStringList labels1;
    labels.clear();
    labels1.clear();
    NS.clear();
    NS.insert(edge[symbol[q]].begin(), edge[symbol[q]].end());
    for (set<string>::iterator it = NS.begin(); it != NS.end(); it++) {
        labels << QString::fromStdString(*it);
    }
    ui->tbw_dfa->setHorizontalHeaderLabels(labels);
    for (set<set<int>>::iterator it = dallNode[symbol[q]].begin(); it != dallNode[symbol[q]].end(); it++) {
        labels1 << QString::fromStdString(set_to_str(*it));
    }
    ui->tbw_dfa->setVerticalHeaderLabels(labels1);
    int j = 0;
    for (set<set<int>>::iterator item = dallNode[symbol[q]].begin(); item != dallNode[symbol[q]].end(); item++) {
        int i = 0;
        for (set<string>::iterator it = edge[symbol[q]].begin(); it != edge[symbol[q]].end(); it++) {
            ui->tbw_dfa->setItem(j, i++, new QTableWidgetItem(QString::fromStdString(set_to_str(DMZ[symbol[q]][*item][*it]))));
        }
        j++;
    }

    //QTableWidget* tbw_min_dfa = ui->tbw_min_dfa;
    // MinDFA以状态转换表呈现
    line = ns[symbol[q]];
    column = edge[symbol[q]].size();
    ui->tbw_min_dfa->setRowCount(line);
    ui->tbw_min_dfa->setColumnCount(column + 1);
    labels.clear();
    labels1.clear();
    NS.clear();
    NS.insert(edge[symbol[q]].begin(), edge[symbol[q]].end());
    for (set<string>::iterator it = NS.begin(); it != NS.end(); it++) {
        labels << QString::fromStdString(*it);
    }
    labels << QString::fromStdString("state");
    ui->tbw_min_dfa->setHorizontalHeaderLabels(labels);
    for (int i = 0; i <= line; i++) {
        labels1 << QString::number(i + 1);
    }
    ui->tbw_min_dfa->setVerticalHeaderLabels(labels1);
    int sta;
    j = 0;
    for (int i = 0; i <= line; i++)
    {
        j = 0;
        for (set<string>::iterator it = edge[symbol[q]].begin(); it != edge[symbol[q]].end(); it++)
        {
            if (SZ[symbol[q]][i + 1][*it] != 0)
            {
                ui->tbw_min_dfa->setItem(i, j++, new QTableWidgetItem(QString::number(SZ[symbol[q]][i + 1][*it])));//加入尾
            }
            else
            {
                ui->tbw_min_dfa->setItem(i, j++, new QTableWidgetItem(""));//加入尾
            }

            //下面是标状态
            sta = states[symbol[q]][i + 1];
            if (sta == 3)
            {
                ui->tbw_min_dfa->setItem(i, j, new QTableWidgetItem("+-"));
                ui->tbw_min_dfa->item(i, j)->setForeground(QBrush(QColor(255, 0, 0)));//设置为红色
                ui->tbw_min_dfa->item(i, j)->setFont(QFont("Times", 20, QFont::Black));
            }
            else if (sta == 1)
            {
                ui->tbw_min_dfa->setItem(i, j, new QTableWidgetItem("-"));
                ui->tbw_min_dfa->item(i, j)->setForeground(QBrush(QColor(255, 0, 0)));//设置为红色
                ui->tbw_min_dfa->item(i, j)->setFont(QFont("Times", 20, QFont::Black));
            }
            else if (sta == 2)
            {
                ui->tbw_min_dfa->setItem(i, j, new QTableWidgetItem("+"));
                ui->tbw_min_dfa->item(i, j)->setForeground(QBrush(QColor(255, 0, 0)));//设置为红色
                ui->tbw_min_dfa->item(i, j)->setFont(QFont("Times", 20, QFont::Black));
            }
        }
    }
}

// 1.3生成词法分析代码，并显示
void FrontEndWindow::geneateLexCode()
{
    mLexCode.open(QIODevice::ReadOnly); // “生成”显示词法分析程序
    QTextStream in(&mLexCode);
    ui->txb_lex_code->setText(in.readAll());
    mLexCode.close();
}

// 1.4执行词法分析程序并生成tokenstream
void FrontEndWindow::createTokenStream()
{
    QStringList argus;    // 构造命令行参数
    argus.append(mSourceCode.fileName());
    argus.append(mTokenStream.fileName());
    argus.append("0"); // 0-词法分析
    QProcess qLexPro;
    qLexPro.execute(mLexExe.fileName(), argus);// 执行程序
    
    this->readTokenStream();// 生成单词编码并显示
}

// 2.0按钮--封装--开始整个语法分析
void FrontEndWindow::on_btn_start_parse_clicked()
{
    if (mBNF.fileName() == NULL || mSourceCode.fileName() == NULL || mLexExe.fileName() == NULL) {
        QMessageBox::warning(this, "禁止", "缺少语法分析的必要输入", QMessageBox::Close); 
        ui->tabWidget->setCurrentWidget(ui->widget_in_out); return;
    }
    if (mIsFirstHandBnf) { this->BNF2LL1(); mIsFirstHandBnf = false; }
    this->createSyntaxTree();
    ui->tabWidget->setCurrentWidget(ui->widget_syntax_analysis);
    ui->cbx_language->setEnabled(false);
}

// 2.1处理，从BNF文法到LL(1)分析表
void FrontEndWindow::BNF2LL1()
{
    Terminator.clear();
    NT.clear();
    production.clear();
    Table.clear();
    FIRST.clear();
    FOLLOW.clear();
    ui->list_simplify->clear();
    ui->list_eliminate->clear();
    ui->list_first_set->clear();
    ui->list_follow_set->clear(); // 以上为初始化数据结构
    std::string pathStr = mCodec->fromUnicode(mBNF.fileName()).data(); // QString to string
    readGrammar(pathStr);

    // BNF文法化简
    simplify();
    for (string left : NT) {
        string gram = left + "->";
        for (auto it = production[left].begin(); it != production[left].end(); it++) {
            if (it != production[left].end() - 1)
                gram = gram + *it + "|";
            else
                gram = gram + *it;
        }
        ui->list_simplify->addItem(QString::fromStdString(gram));
    }

    // 消除左递归、提取左公因子
    leftRecursionRemoval();//去除左递归
    leftFactoring();//提取左公共因子
    for (string left : NT) {
        string gram = left + "->";
        for (auto it = production[left].begin(); it != production[left].end(); it++) {
            if (it != production[left].end() - 1)
                gram = gram + *it + "|";
            else
                gram = gram + *it;
        }
        ui->list_eliminate->addItem(QString::fromStdString(gram));
    }
    
    // 求First、Follow集
    getFirst();
    getFollow();
    getTable();
    for (string non_terminal : NT) {
        string first_set = non_terminal + ": ";
        for (string first : FIRST[non_terminal])
            first_set += first + ", ";
        first_set.pop_back();
        first_set.pop_back();
        ui->list_first_set->addItem(QString::fromStdString(first_set));
    }
    for (string non_terminal : NT) {
        string follow_set = non_terminal + ": ";
        for (string follow : FOLLOW[non_terminal])
            follow_set += follow + ", ";
        follow_set.pop_back();
        follow_set.pop_back();
        ui->list_follow_set->addItem(QString::fromStdString(follow_set));
    }
    
    // 求LL(1)分析表
    ui->tbw_LL1->clear();
    int q = T.size();
    int p = NT.size();
    ui->tbw_LL1->setRowCount(p);
    ui->tbw_LL1->setColumnCount(q + 1);
    vector<string>x = Terminator;
    x.push_back("#");
    QStringList labels;
    labels.clear();
    for (string it1 : x)
    {
        labels << QString::fromStdString(it1);
    }
    ui->tbw_LL1->setHorizontalHeaderLabels(labels);
    labels.clear();
    for (string A : NT) {
        labels << QString::fromStdString(A);
    }
    ui->tbw_LL1->setVerticalHeaderLabels(labels);
    int i = 0;
    for (string A : NT) {
        int j = 0;
        for (string a : x) {
            pair<string, string>symbol;
            symbol = make_pair(A, a);
            if (!Table[symbol].empty())
                ui->tbw_LL1->setItem(i, j, new QTableWidgetItem(QString::fromStdString(Table[symbol])));
            j++;
        }
        i++;
    }
}

// 2.2LL(1)分析法生成语法树
void FrontEndWindow::createSyntaxTree()
{
    QStringList argus;    // 构造命令行参数
    argus.append(mSourceCode.fileName());
    argus.append(mSyntaxTree.fileName());
    argus.append("1"); // 1-语法分析
    QProcess qLexPro;
    qLexPro.execute(mLexExe.fileName(), argus);// 执行程序
    //// 构造命令参数，执行程序
    //QString qcmd = mLexExe.fileName() + " " + mSourceCode.fileName() + " " + mSyntaxTree.fileName() + " 1"; // 分析exe + 源程序 + 输出文件
    //std::string cmd = mCodec->fromUnicode(qcmd).data();
    ////std::string cmd = qcmd.toStdString();
    //system(cmd.c_str()); // 执行生成的词法分析程序

    this->readSyntaxTree(); // 生成语法树并显示
}

// 0.0输入程序设计语言改变
void FrontEndWindow::on_cbx_language_currentIndexChanged()
{
    mLanguage = ui->cbx_language->currentText();
    mIsFirstHandBnf = true; mIsFirstHandleRegExp = true;
    if (mLanguage == "tiny") {
        mLexCode.setFileName(getAbsolutePath(mTinySaveFolder + "tiny-lex.c"));
        mLexExe.setFileName(getAbsolutePath(mTinySaveFolder + "tiny-lex.exe"));
        mTokenStream.setFileName(getAbsolutePath(mTinySaveFolder + "tiny-token.txt"));
        mSyntaxTree.setFileName(getAbsolutePath(mTinySaveFolder + "tiny-tree.txt"));
    }
    else if (mLanguage == "minic") {
        mLexCode.setFileName(getAbsolutePath(mMinicSaveFolder + "minic-lex.c"));
        mLexExe.setFileName(getAbsolutePath(mMinicSaveFolder + "minic-lex.exe"));
        mTokenStream.setFileName(getAbsolutePath(mMinicSaveFolder + "minic-token.txt"));
        mSyntaxTree.setFileName(getAbsolutePath(mMinicSaveFolder + "minic-tree.txt"));
    }
    //qDebug() << mLexCode.fileName() << endl;
    //qDebug() << mLexExe.fileName() << endl;
    //qDebug() << mTokenStream.fileName() << endl;
    //qDebug() << mSyntaxTree.fileName() << endl;
    QMessageBox::information(this, "标志已更改", mLanguage, QMessageBox::Ok);
}

// 0.1选择正则表达式文件
void FrontEndWindow::on_btn_Reg_clicked()
{
	QString name = QFileDialog::getOpenFileName(this, "", "../", "RegExp Files(*.re);;Text Files(*.txt)");
    if (name.isEmpty()) // 判断是否选择了新文件
        return;
    this->readRegExp(name);
}

// 0.2选择BNF文法文件
void FrontEndWindow::on_btn_BNF_clicked()
{
    QString name = QFileDialog::getOpenFileName(this, "", "../", "BNF Files(*.bnf);;Text Files(*.txt)");
    if (name.isEmpty()) // 判断是否选择了新文件
        return;
    this->readBNF(name);
}

// 0.3选择源程序文件
void FrontEndWindow::on_btn_SourceCode_clicked()
{
    QString name = QFileDialog::getOpenFileName(this, "", "../");
    if (name.isEmpty()) // 判断是否选择了新文件
        return;
    this->readSourceCode(name);
}

// 0.4重写拖动进入事件
void FrontEndWindow::dropEvent(QDropEvent* event)
{
    QList<QUrl> urls = event->mimeData()->urls();
    if (urls.empty()) return;
    QString qstrUrl = urls.first().toLocalFile();
    if (qstrUrl.isEmpty()) return; // 获取文件绝对路径
    if (ui->txb_SourceCode->geometry().contains(this->mapFromGlobal(QCursor::pos())))
        this->readSourceCode(qstrUrl);
    else if (ui->txb_Reg->geometry().contains(this->mapFromGlobal(QCursor::pos())))
        this->readRegExp(qstrUrl);
    else if (ui->txb_BNF->geometry().contains(this->mapFromGlobal(QCursor::pos())))
        this->readBNF(qstrUrl);
}

// 读取文件并显示
inline void FrontEndWindow::readSourceCode(const QString &name)
{
    mSourceCode.setFileName(name); 
    mSourceCode.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream in(&mSourceCode);
    in.setCodec("UTF-8");
    ui->le_SourceCode->setText(name);
    ui->txb_SourceCode->setText(in.readAll());
    mSourceCode.close();
    //qDebug() << mSourceCode.fileName() << endl;
}
inline void FrontEndWindow::readRegExp(const QString& name)
{
    mRegExp.setFileName(name); // 读取文件并显示
    mRegExp.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream in(&mRegExp);
    in.setCodec("UTF-8");
    ui->le_Reg->setText(name);
    ui->txb_Reg->setText(in.readAll());
    mRegExp.close();
    //qDebug() << mRegExp.fileName() << endl;
}
inline void FrontEndWindow::readBNF(const QString& name)
{
    mBNF.setFileName(name); // 读取文件并显示
    mBNF.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream in(&mBNF);
    in.setCodec("UTF-8");
    ui->le_BNF->setText(name);
    ui->txb_BNF->setText(in.readAll());
    mBNF.close();
    //qDebug() << mBNF.fileName() << endl;
}
inline void FrontEndWindow::readGuide(void)
{
    //读取使用说明书并显示
    QFile guide(getAbsolutePath("使用说明书.txt")); // 读取文件并显示
    if (!guide.exists())
        return;
    guide.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream in(&guide);
    in.setCodec("UTF-8");
    ui->txb_guide->setText(in.readAll());
    guide.close();
}
inline void FrontEndWindow::readTokenStream(void)
{
    mTokenStream.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream in(&mTokenStream);
    in.setCodec("UTF-8");
    ui->txb_token->setText(in.readAll());
    mTokenStream.close();
}
inline void FrontEndWindow::readSyntaxTree(void)
{
    mSyntaxTree.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream in(&mSyntaxTree);
    in.setCodec("UTF-8");
    ui->txb_tree->setText(in.readAll());
    mSyntaxTree.close();
}