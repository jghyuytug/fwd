#include "widget.h"
#include "ui_widget.h"
#include "aes_cbc.h"
#include "httpaesrequest.h"
#include "util.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QMessageBox>
#include <QCryptographicHash>
#include <QProcess>
#include <QFile>
#include <QDebug>
#include <QCoreApplication>
#include <QPushButton>
#include <QTimer>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    QByteArray dnfInfoData;
    QList<QByteArray> dnfInfoList;
    AES_CBC aes;

    // 必须先设置UI，然后再访问UI控件
    ui->setupUi(this);

    // 现在可以安全地设置tab顺序
    this->setTabOrder(ui->text_user, ui->text_passwd);
    this->setTabOrder(ui->text_passwd, ui->btn_login);

    // 智能查找DNF.exe - 检查多个可能的位置
    QStringList possiblePaths;
    possiblePaths << "./DNF.exe"           // 当前目录
                  << "./dnf.exe"           // 当前目录（小写）
                  << "../dnf/DNF.exe"      // 父目录的dnf子目录
                  << "../dnf/dnf.exe"      // 父目录的dnf子目录（小写）
                  << "../../dnf/DNF.exe";  // 上两级目录的dnf子目录

    QString dnfPath;
    bool found = false;

    for(const QString &path : possiblePaths) {
        QFile file(path);
        if (file.exists()) {
            // 检查不是占位文件
            if(file.open(QIODevice::ReadOnly)) {
                QString content = file.readAll();
                file.close();
                if(!content.contains("dummy")) {
                    dnfExePath = path;  // 保存找到的路径
                    found = true;
                    qDebug() << "找到DNF.exe于:" << path;
                    break;
                }
            }
        }
    }

    if (!found) {
        QMessageBox::warning(this,tr("Warning"),
            tr("没有找到DNF.exe！\n\n请确保：\n1. 已安装DNF游戏\n2. 将登录器放到游戏目录\n\n或手动指定游戏路径"),
            QMessageBox::Ok);
        exit(1);
    }

    // 静默检查hosts文件，失败也继续运行（用户可能已手动配置）
    if(!changeHostFile()){
        qDebug() << "hosts文件修改失败（可能需要管理员权限），程序继续运行";
        // 不阻塞界面，用户可能已手动配置hosts或以管理员运行
    }

    QFile saveFile("./dnfinfo.dat");
    QTextStream textRead(&saveFile);

    if(saveFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        dnfInfoData = aes.decrypt(textRead.readAll().toLocal8Bit());
        dnfInfoList = dnfInfoData.split('|');
        if(dnfInfoList.size()>1 && dnfInfoList.at(0).size()>6 && dnfInfoList.at(1).size()>6){
            ui->text_user->setText(dnfInfoList[0]);
            ui->text_passwd->setText(dnfInfoList[1]);
            ui->checkBox->setChecked(true);
        }
    }

    connect(ui->lab_regedit, SIGNAL(clicked()), this, SLOT(lab_regedit_clicked()));
    connect(ui->lab_reset, SIGNAL(clicked()), this, SLOT(lab_reset_clicked()));

    // Qt会自动连接on_btn_login_clicked槽,无需手动连接

    qDebug() << "Widget构造函数完成，按钮连接已建立";
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_btn_login_clicked()
{
    qDebug() << "登录按钮被点击！";

    QString macAddr;
    QByteArray postData;
    QByteArray dnfInfoData;
    AES_CBC aes;
    QFile saveFile("./dnfinfo.dat");

    if(ui->text_passwd->text().isEmpty()
            || ui->text_user->text().isEmpty()){
        QMessageBox::warning(this,tr("Error"),tr("请输入用户名和密码"),QMessageBox::Ok);
        return;
    }

    if (processExistsByName(TEXT("DNF.exe"))) {
        QMessageBox::warning(this,tr("Warning"), tr("DNF运行中，请关闭后重试!"), QMessageBox::Ok);
        return;
    }

    if(!ui->text_passwd->isValidity() && !ui->text_passwd->isValidity()) {
        QMessageBox::warning(this,tr("Warning"), tr("用户名或密码不合法，只能包含a-z、A-Z、0-9"), QMessageBox::Ok);
        return;
    }

    // 立即禁用登录按钮，防止重复点击
    ui->btn_login->setEnabled(false);
    ui->btn_login->setText(tr("验证账号中..."));

    if(ui->checkBox->isChecked()) {
        dnfInfoData.append(ui->text_user->text().toUtf8());
        dnfInfoData.append("|");
        dnfInfoData.append(ui->text_passwd->text().toUtf8());

        dnfInfoData = aes.encrypt(dnfInfoData);

        if(!saveFile.open(QIODevice::WriteOnly | QIODevice::Text))
        {
             QMessageBox::warning(this,tr("waring"),tr("无法保存用户名和密码！"),QMessageBox::Yes);
        }else{
            QTextStream in(&saveFile);
            in << dnfInfoData;
            saveFile.close();
        }
    }else{
        saveFile.remove();
    }

    macAddr = getMacAddr();

    postData.append("login|");
    postData.append(ui->text_user->text().toUtf8());
    postData.append("|");
    postData.append(QCryptographicHash::hash (
                        ui->text_passwd->text().toLatin1(),
                        QCryptographicHash::Md5).toHex());
    postData.append("|");
    postData.append(macAddr.toUtf8());

    HttpAesRequest *loginRequest = new HttpAesRequest();

    connect(loginRequest,SIGNAL(finished(QNetworkReply*)),
            this,SLOT(requestFinished(QNetworkReply*)));

    loginRequest->sendPOST(postData);
}


void Widget::requestFinished(QNetworkReply *reply)
{
    QStringList dnfArgs;
    QProcess process;
    AES_CBC aes;

    QByteArray rawResponse = reply->readAll();
    reply->deleteLater();

    qDebug() << "登录响应(hex):" << rawResponse.toHex();

    // 解密为QByteArray
    QByteArray decrypted = aes.decrypt(rawResponse);

    // 截取到第一个NULL字节前的内容
    int nullPos = decrypted.indexOf('\0');
    QByteArray cleanBytes = (nullPos >= 0) ? decrypted.left(nullPos) : decrypted;

    // 转换为QString并去除空白
    QString response = QString::fromUtf8(cleanBytes).trimmed();

    qDebug() << "服务器响应:" << response;

    // 检查是否是错误响应
    if(response.contains("Error", Qt::CaseInsensitive) || response.length() < 20) {
        ui->btn_login->setEnabled(true);
        ui->btn_login->setText(tr("登录"));
        QString errorMsg = tr("用户名或密码错误！");
        QMessageBox::warning(this, tr("登录失败"), errorMsg, QMessageBox::Ok);
        return;
    }

    // 验证成功
    ui->btn_login->setText(tr("启动游戏中..."));
    dnfArgs.append(response);

    qDebug() << "准备启动游戏，参数:" << dnfArgs;
    qDebug() << "DNF路径:" << dnfExePath;

    if(!process.startDetached(dnfExePath, dnfArgs)) {
        ui->btn_login->setEnabled(true);
        ui->btn_login->setText(tr("登录"));
        QMessageBox::warning(this,tr("Error"),
            tr("启动失败！\n\nDNF路径: %1\n登录token: %2\n\n请检查游戏文件是否完整。")
            .arg(dnfExePath).arg(response),
            QMessageBox::Ok);
        qDebug() << "进程启动失败";
        return;
    }

    qDebug() << "游戏进程启动成功";

    // 启动成功，3秒后自动关闭登录器
    QTimer::singleShot(3000, this, &QWidget::close);
}

void Widget::lab_regedit_clicked()
{
    rgDialog = new regeditDialog();
    rgDialog->show();
}

void Widget::lab_reset_clicked()
{
    reDialog = new repasswdDialog();
    reDialog->show();
}

void Widget::on_checkBox_clicked()
{

    ui->checkBox->setChecked(ui->checkBox->isChecked());
}
