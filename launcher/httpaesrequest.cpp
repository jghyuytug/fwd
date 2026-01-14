#include "httpaesrequest.h"
#include "aes_cbc.h"
#include "util.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkProxy>
#include <QDebug>
#include <QStringList>
#include <QCryptographicHash>

HttpAesRequest::HttpAesRequest(QNetworkAccessManager *parent) : QNetworkAccessManager(parent)
{
    // 禁用代理
    this->setProxy(QNetworkProxy::NoProxy);

    request = new QNetworkRequest();

    request->setUrl(QUrl("http://start.dnf.tw:8080/dnflogin/dnf.php"));
    request->setRawHeader("Content-Type","application/x-www-form-urlencoded");

}

HttpAesRequest::~HttpAesRequest()
{

}

void HttpAesRequest::sendPOST(QByteArray postData)
{
    AES_CBC aes;
    QByteArray postMd5;

    qDebug() << "原始POST数据:" << postData;

    postMd5 = QCryptographicHash::hash (
                          postData,
                          QCryptographicHash::Md5).toHex();

    postData.append("&");
    postData.append(postMd5);

    qDebug() << "添加MD5后数据:" << postData;

    postData = aes.encrypt(postData);

    qDebug() << "加密后数据:" << postData;
    qDebug() << "发送POST请求到:" << request->url().toString();

    QNetworkReply *reply = this->post(*request, postData);
    // Qt6兼容:使用新的信号连接方式
    connect(reply, &QNetworkReply::errorOccurred,
            this, &HttpAesRequest::onNetworkError);
}

void HttpAesRequest::onNetworkError(QNetworkReply::NetworkError error)
{
    qDebug() << "网络错误:" << error;
}


