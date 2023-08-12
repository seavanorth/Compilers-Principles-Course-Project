#pragma once
#pragma execution_character_set("utf-8") // 设置执行字符集

#include <QtWidgets/QMainWindow>
#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
#include <QStandardItem>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QTextCodec>
#include <QProcess>
#include "ui_FrontEndWindow.h"



QT_BEGIN_NAMESPACE
namespace Ui { class FrontEndWindowClass; };
QT_END_NAMESPACE

class FrontEndWindow : public QMainWindow
{
    Q_OBJECT

public:
    FrontEndWindow(QWidget *parent = nullptr);
    ~FrontEndWindow();

    // 当前目录绝对路径拼接"/path/file.type"
    inline QString getAbsolutePath(const QString &path) { return QDir::cleanPath(QCoreApplication::applicationDirPath() + QDir::separator() + path); }
    inline void readSourceCode(const QString &);
    inline void readRegExp(const QString &);
    inline void readBNF(const QString &);
    inline void readGuide(void);
    inline void readTokenStream(void);
    inline void readSyntaxTree(void);

private slots:
    void on_cbx_language_currentIndexChanged(); // 输入程序设计语言改变
    void on_comboBox_currentIndexChanged() { this->showStateTable(); } // 当前状态表中查看的token改变

    void on_btn_guide_clicked() { ui->tabWidget->setCurrentWidget(ui->widget_guide); }
    void on_btn_Reg_clicked(); // 选择正则表达式
    void on_btn_BNF_clicked(); // 选择BNF
    void on_btn_SourceCode_clicked(); // 选择源程序

    void on_btn_start_scan_clicked(); // 按钮--封装--开始整个词法分析

    void on_btn_start_parse_clicked(); // 按钮--封装--开始整个语法分析

protected:
    void dragEnterEvent(QDragEnterEvent* e) { e->acceptProposedAction(); } //拖动进入事件
    void dropEvent(QDropEvent* e); //拖动放下事件

private:
    Ui::FrontEndWindowClass *ui;

    QTextCodec* mCodec = QTextCodec::codecForName("GB2312");

    QString mLanguage; //程序设计语言
    QString mTinySaveFolder = "/save_tiny/"; // 存放Tiny、Minic相关依赖文件的文件夹
    QString mMinicSaveFolder = "/save_minic/";// 如输出token-stream、Syntax-tree文件

    QFile mLexCode; // 词法分析程序源代码
    QFile mLexExe; // 词法分析程序可执行程序

    QFile mTokenStream; // 单词编码文件
    QFile mSyntaxTree; //语法树文件

    QFile mRegExp; // 正则表达式文件
    QFile mBNF; // BNF文法文件
    QFile mSourceCode; // 源程序文件

    void RegExp2MinDFA(); // 封装：正则表达式->最小化DFA
    void showStateTable(); // 显示各阶段状态转换表
    void geneateLexCode(); // 生成词法分析程序
    void createTokenStream(); // 执行词法分析并生成符号流

    void BNF2LL1(); // 封装：BNF文法->LL(1)分析表
    void createSyntaxTree(); // 生成语法树

    bool mIsFirstHandleRegExp = true; // 首次处理正则表达式
    bool mIsFirstHandBnf = true; // 首次处理BNF文法
};
