#pragma once
#pragma execution_character_set("utf-8") // ����ִ���ַ���

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

    // ��ǰĿ¼����·��ƴ��"/path/file.type"
    inline QString getAbsolutePath(const QString &path) { return QDir::cleanPath(QCoreApplication::applicationDirPath() + QDir::separator() + path); }
    inline void readSourceCode(const QString &);
    inline void readRegExp(const QString &);
    inline void readBNF(const QString &);
    inline void readGuide(void);
    inline void readTokenStream(void);
    inline void readSyntaxTree(void);

private slots:
    void on_cbx_language_currentIndexChanged(); // �������������Ըı�
    void on_comboBox_currentIndexChanged() { this->showStateTable(); } // ��ǰ״̬���в鿴��token�ı�

    void on_btn_guide_clicked() { ui->tabWidget->setCurrentWidget(ui->widget_guide); }
    void on_btn_Reg_clicked(); // ѡ��������ʽ
    void on_btn_BNF_clicked(); // ѡ��BNF
    void on_btn_SourceCode_clicked(); // ѡ��Դ����

    void on_btn_start_scan_clicked(); // ��ť--��װ--��ʼ�����ʷ�����

    void on_btn_start_parse_clicked(); // ��ť--��װ--��ʼ�����﷨����

protected:
    void dragEnterEvent(QDragEnterEvent* e) { e->acceptProposedAction(); } //�϶������¼�
    void dropEvent(QDropEvent* e); //�϶������¼�

private:
    Ui::FrontEndWindowClass *ui;

    QTextCodec* mCodec = QTextCodec::codecForName("GB2312");

    QString mLanguage; //�����������
    QString mTinySaveFolder = "/save_tiny/"; // ���Tiny��Minic��������ļ����ļ���
    QString mMinicSaveFolder = "/save_minic/";// �����token-stream��Syntax-tree�ļ�

    QFile mLexCode; // �ʷ���������Դ����
    QFile mLexExe; // �ʷ����������ִ�г���

    QFile mTokenStream; // ���ʱ����ļ�
    QFile mSyntaxTree; //�﷨���ļ�

    QFile mRegExp; // ������ʽ�ļ�
    QFile mBNF; // BNF�ķ��ļ�
    QFile mSourceCode; // Դ�����ļ�

    void RegExp2MinDFA(); // ��װ��������ʽ->��С��DFA
    void showStateTable(); // ��ʾ���׶�״̬ת����
    void geneateLexCode(); // ���ɴʷ���������
    void createTokenStream(); // ִ�дʷ����������ɷ�����

    void BNF2LL1(); // ��װ��BNF�ķ�->LL(1)������
    void createSyntaxTree(); // �����﷨��

    bool mIsFirstHandleRegExp = true; // �״δ���������ʽ
    bool mIsFirstHandBnf = true; // �״δ���BNF�ķ�
};
