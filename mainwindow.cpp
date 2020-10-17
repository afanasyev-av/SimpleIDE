#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QRegExp>

#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(&mProcessCompile,SIGNAL(readyReadStandardOutput()),this,SLOT(showOutput()));
    connect(&mProcessCompile,SIGNAL(readyReadStandardError()),this,SLOT(showOutput()));

    ui->textEditDebug->setStyleSheet("QPlainTextEdit {background-color: #000000; color: #00FF00;}");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionOpen_triggered()
{
    mFileName = QFileDialog::getOpenFileName(this, tr("Open File"),"",tr("C++ file (*.cpp)"));
    if(mFileName.isEmpty())return;

    QFile file(mFileName);
    file.open(QFile::ReadOnly | QFile::Text);
    QTextStream ReadFile(&file);

    ui->textEditCode->clear();
    ui->textEditCode->appendPlainText(ReadFile.readAll());
    file.close();
}

void MainWindow::showOutput()
{
    QString strout = QString::fromLocal8Bit(mProcessCompile.readAllStandardOutput());
    ui->textEditDebug->appendPlainText(strout);
    QString strerr = QString::fromLocal8Bit(mProcessCompile.readAllStandardError());
    ui->textEditDebug->appendPlainText(strerr);


    //regexp : (.*):(\d+):(\d+): error: (.*)
    //find error
    QRegExp re( "([^\\n]*):(\\d+):(\\d+): error: ([^\\n]*)" );
    int pos = re.indexIn(strerr);

    if(pos != -1){
        QStringList list = re.capturedTexts();
        int i = ui->tableWidget->rowCount();

        ui->tableWidget->setRowCount(i+1);
        ui->tableWidget->setItem(i, 0, new QTableWidgetItem(list[2]));
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem(list[3]));
        ui->tableWidget->setItem(i, 2, new QTableWidgetItem(list[4]));
        ui->tableWidget->setItem(i, 3, new QTableWidgetItem(list[1]));
    }
}

void MainWindow::on_actionCompile_triggered()
{
    ui->textEditDebug->clear();
    QString prog = "g++ " + mFileName;
    mProcessCompile.start(prog);
    ui->textEditDebug->appendPlainText(prog);

    ui->tableWidget->setRowCount(0);
    ui->tableWidget->setColumnCount(4);
}

void MainWindow::on_actionRun_triggered()
{
    QString exec = "xterm";
    QStringList params;
    params << "./a.out";
    mProcessRun.setWorkingDirectory(QCoreApplication::applicationDirPath());
    mProcessRun.start(exec, params);
}

void MainWindow::on_tableWidget_cellClicked(int row, int column)
{
    int i = ui->tableWidget->item(row,0)->text().toInt();
    int j = ui->tableWidget->item(row,1)->text().toInt();

    qDebug() << i << j <<  "\n";

    ui->textEditCode->setFocus();

    QTextCursor Cursor = ui->textEditCode->textCursor();
    Cursor.movePosition(QTextCursor::Start);
    Cursor.movePosition(QTextCursor::NextBlock,QTextCursor::MoveAnchor,i-1);

    ui->textEditCode->setTextCursor(Cursor);
}

void MainWindow::on_actionDebug_triggered()
{
    //compile
    ui->textEditDebug->clear();
    QString prog = "g++ -g " + mFileName;
    mProcessCompile.start(prog);
    ui->textEditDebug->appendPlainText(prog);
    mProcessCompile.waitForFinished();
    //start gdb
    QString exec = "gdb";
    QStringList params;
    params << "a.out";
    mProcessCompile.setWorkingDirectory(QCoreApplication::applicationDirPath());
    mProcessCompile.start(exec, params);

    mProcessCompile.write("b 1\n");
    mProcessCompile.waitForBytesWritten();

    mProcessCompile.write("r\n");
    mProcessCompile.waitForBytesWritten();
}

void MainWindow::on_actionStep_triggered()
{
    mProcessCompile.write("n\n");
    mProcessCompile.waitForBytesWritten();

    mProcessCompile.write("i r\n");
    mProcessCompile.waitForBytesWritten();
}

void MainWindow::on_actionSave_triggered()
{
    mFileName = QFileDialog::getSaveFileName(this, tr("Save File"),"",tr("C++ file (*.cpp)"));
    if(mFileName.isEmpty())return;

    QFile file(mFileName);
    file.open(QFile::WriteOnly | QFile::Text);
    QTextStream WriteFile(&file);

    WriteFile << ui->textEditCode->toPlainText();
    file.close();
}
