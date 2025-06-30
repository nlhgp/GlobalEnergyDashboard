#include "my_thread.h"
my_thread::my_thread(QObject *parent)
    : QThread{parent}
{}

void my_thread::run()
{
    connect(socket,&QTcpSocket::readyRead,this,&my_thread::client_info_slot);
}

void my_thread::get_socket(QTcpSocket *s)//获取socket对象
{
    this->socket=s;
    this->IP=s->peerAddress().toString();
}

void my_thread::client_info_slot()
{
    QString m=socket->readAll();
    std::string s = m.toStdString();
    if(s.substr(0,4)=="init")
        this->username=s.substr(s.find('#'),s.length()).c_str();
    emit message(IP+"#"+m.toUtf8());
}

void my_thread::deconned()
{
    emit message((IP+"#deconned").toUtf8());
}

QString my_thread::get_ip()
{
    return this->IP;
}

QString my_thread::get_name()
{
    return this->username;
}
