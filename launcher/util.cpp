#include "windows.h"
#include "TlHelp32.h"
#include <QString>
#include <QFile>
#include <QNetworkInterface>

static QString GetSysFolder ()
{
    TCHAR szPath[100] ={0};
    GetSystemDirectory( szPath, 100 ) ;
    return QString::fromWCharArray(szPath);
}

bool processExistsByName(LPCTSTR findProcessName)//根据进程名查找进程PID
{
    HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
    if(hSnapShot == INVALID_HANDLE_VALUE)
    {
        return false;
    }

    PROCESSENTRY32 pe32;//声明进程入口对象
    pe32.dwSize = sizeof(PROCESSENTRY32);//填充进程入口对象大小
    Process32First(hSnapShot,&pe32);//遍历进程列表
    do{
        if(!lstrcmp(pe32.szExeFile, findProcessName))//查找指定进程名的PID
        {
            return true;
        }
    } while (Process32Next(hSnapShot,&pe32));
    CloseHandle(hSnapShot);
    return false;
}

QString getMacAddr()
{
    QString macAddr;
    QList<QNetworkInterface> interfaceList = QNetworkInterface::allInterfaces();

    foreach(QNetworkInterface interfaceItem, interfaceList)
    {
        if(interfaceItem.flags().testFlag(QNetworkInterface::IsUp)
                &&interfaceItem.flags().testFlag(QNetworkInterface::IsRunning)
                &&interfaceItem.flags().testFlag(QNetworkInterface::CanBroadcast)
                &&interfaceItem.flags().testFlag(QNetworkInterface::CanMulticast)
                &&!interfaceItem.flags().testFlag(QNetworkInterface::IsLoopBack))
        {
            QList<QNetworkAddressEntry> addressEntryList=interfaceItem.addressEntries();
            foreach(QNetworkAddressEntry addressEntryItem, addressEntryList)
            {
                if(addressEntryItem.ip().protocol()==QAbstractSocket::IPv4Protocol)
                {
                    macAddr = interfaceItem.hardwareAddress();
                }
            }
        }
    }

    return macAddr;
}

bool changeHostFile()
{
    QString hostPath = GetSysFolder();
    QString hostLine, hostData;
    QString backPath = "./hosts.bak";

    bool hostIsChanged = false;
    bool firstDnf = true;
    bool hasDnfEntry = false;

    hostPath.append("\\drivers\\etc\\hosts");

    QFile hostFile(hostPath);
    QTextStream hostStream(&hostFile);

    if(hostFile.open(QIODevice::ReadOnly| QIODevice::Text))
    {
        while(!hostStream.atEnd()){
            hostLine = hostStream.readLine();

            qDebug() << "读取hosts行:" << hostLine;
            
            // 保留空行和注释行
            QString trimmedLine = hostLine.trimmed();
            if(hostLine.isEmpty() || trimmedLine.isEmpty() || trimmedLine.startsWith('#')){
                hostData.append(hostLine);
                hostData.append("\r\n");
                continue;
            }

            hostLine = hostLine.simplified();

            // 检查是否包含start.dnf.tw的条目
            if(hostLine.indexOf("start.dnf.tw") != -1){
                hasDnfEntry = true;
                // 接受127.0.0.1或192.168.200.131作为正确配置
                if(firstDnf && (hostLine.indexOf("127.0.0.1") != -1 || hostLine.indexOf("192.168.200.131") != -1)){
                    qDebug() << "找到正确的hosts条目:" << hostLine;
                    hostFile.close();
                    return true;
                }
                firstDnf = false;
                // 跳过错误的dnf条目，不添加到hostData中
                continue;
            }

            // 保留其他正常条目
            hostData.append(hostLine);
            hostData.append("\r\n");
        }

        hostFile.close();
    } else {
        qDebug() << "无法读取hosts文件:" << hostPath;
    }

    // 如果没有找到正确的条目，需要写入新条目
    if(!hostIsChanged){
        
        // 备份原始hosts文件
        if(hostFile.exists()){
            qDebug() << "备份hosts文件到:" << backPath;
            if(CopyFile((LPCWSTR)hostPath.utf16(), (LPCWSTR)backPath.utf16(), false) == 0){
                qDebug() << "备份hosts文件失败";
                return false;
            }
        }

        // 添加新的DNS条目
        if(!hasDnfEntry) {
            hostData.append("192.168.200.131 start.dnf.tw");
        }

        // 写入更新后的hosts文件
        if(hostFile.open(QIODevice::WriteOnly | QIODevice::Text)){
            QTextStream writeStream(&hostFile);
            writeStream << hostData;
            hostFile.close();
            qDebug() << "hosts文件更新成功";
            return true;
        } else {
            qDebug() << "无法写入hosts文件，可能需要管理员权限";
        }
    }

    return false;
}
