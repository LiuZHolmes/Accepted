#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qfiledialog.h"
#include "qprocess.h"
#include "qmessagebox.h"
#include "QtDebug"
#include "QCloseEvent"
#include "qfile.h"
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // 读取文件，初始化窗口各种信息
    qDebug()<< QDir::currentPath() <<endl;
    QFile file("config.txt");
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug()<<"Can't open the file!"<<endl;
    }
    QTextStream in(&file);
    while( !in.atEnd()){
        QString line = in.readLine();
        ui->lineEdit_FilePath->setText(line);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_LookUpFile_clicked()
{
    QString FilePath= QFileDialog::getOpenFileName(this, tr("Select your program"), "/Users/retr0", tr("*"));
    if(FilePath != 0)
    {
        ui->lineEdit_FilePath->setText(FilePath);
    }
}

void MainWindow::on_pushButton_Go_clicked()
{
    QPalette pa; // 事先定义以后用来设置label的text颜色
    // 读取文件路径和输入
    QProcess process;
    QString FilePath = ui->lineEdit_FilePath->text();
    QString Input = ui->textEdit_Input->toPlainText();
    // 调用外部函数执行并写入
    process.start(FilePath);
    process.write((Input + "\r").toStdString().c_str());
    // 等待执行完毕，保证写读的先后顺序，毫秒数要设置足够
    int time = ui->lineEdit_time->text().toInt();
    process.waitForFinished(time*1000);
    // 打印输出
    QByteArray qbaOutput = process.readAllStandardOutput();
    QString Output = qbaOutput;
    // 若为空，即运行超时
    if(Output == "")
    {
        pa.setColor(QPalette::WindowText,Qt::red);
        ui->label_Result->setPalette(pa);
        ui->label_Result->setText("Result: Time limit exceed");
        return;
    }
    Output = "<pre>" + Output + "</pre>";
    ui->textEdit_YourOutput->setHtml(Output);
    // 判断输出是否正确
    QString StandardOutput = ui->textEdit_StandardOutput->toPlainText();
    QString YourOutput = ui->textEdit_YourOutput->toPlainText();
    if(YourOutput == StandardOutput)
    {
        // 正确， Accepted!
        pa.setColor(QPalette::WindowText,Qt::green);
        ui->label_Result->setPalette(pa);
        ui->label_Result->setText("Result: Accepted!");
        QMessageBox::information(NULL, tr("Congratulations"), tr("Accepted!"));
        // 输出，全染绿色
        QString text = "<pre><font color=green>" + ui->textEdit_StandardOutput->toPlainText() + "</font></pre>";
        ui->textEdit_StandardOutput->setHtml(text);
    }
    else
    {
        // 报错
        pa.setColor(QPalette::WindowText,Qt::red);
        ui->label_Result->setPalette(pa);
        ui->label_Result->setText("Result: Wrong answer...");
        // 找到错误的位置并操作
        int index = 0;
        while(1)
        {
            // 文本不匹配或长度不匹配
            if(YourOutput[index] != StandardOutput[index] || index > YourOutput.length() || index > StandardOutput.length())
            {
                // 输出，对的染绿色，后面染红色
                QString correct = StandardOutput.mid(0,index);
                QString wrong = StandardOutput.mid(index);
                QString text = "<pre><font color=green>" + correct + "</font>" + "<font color=red>" + wrong + "</font></pre>";
                ui->textEdit_StandardOutput->setHtml(text);
                // 设定光标位置
                QTextCursor tmpCursor = ui->textEdit_StandardOutput->textCursor();
                tmpCursor.setPosition(index);
                ui->textEdit_StandardOutput->setTextCursor(tmpCursor);
                break;
            }
            else index++;
        }
    }
}

void MainWindow::on_pushButton_LeftToRight_clicked()
{
    ui->textEdit_StandardOutput->setHtml("<pre>" + ui->textEdit_YourOutput->toPlainText() + "</pre>");
}

void MainWindow::on_pushButton_RightToLeft_clicked()
{
    ui->textEdit_YourOutput->setHtml("<pre>" + ui->textEdit_StandardOutput->toPlainText() + "</pre>");
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    // 关闭，存储当前文本文件
    QFile file("config.txt");
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug()<<"Can't open the file!"<<endl;
    }
    QTextStream out(&file);
    out << ui->lineEdit_FilePath->text() << endl;
    qDebug() << "close" << endl;
}
