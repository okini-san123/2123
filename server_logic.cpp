#include <QDebug>
#include<QCoreApplication>
#include <QTcpSocket>
#include "server_logic.h"
#include "db_singleton.h"

void ServerLogic::requestAnalyzer(QString request_string, QTcpSocket *socket)
{
    switch(requests[parse_request(request_string)])
    {
    case 1:
        qDebug()<<"login";
        socket->write(login(request_string).toUtf8());
        break;
    case 2:
        qDebug()<<"get_solution";
        //check();
        break;
    case 3:
        qDebug()<<"signup";
        socket->write(signup(request_string).toUtf8());
        break;
    case 4:
        qDebug()<<"logout";
        socket->write(logout(request_string).toUtf8());
        break;

    default:
        qDebug()<<"not found";

    }
}
QString ServerLogic::login(QString text)
{
    qDebug()<<parse_auth(text);
    ServerLogic::user = text;
    return "Successful";
}

QString ServerLogic::logout(QString text)
{
    qDebug()<<parse_auth(text);
    return "Successful";
}

QString ServerLogic::signup(QString text)
{
    qDebug()<<parse_auth(text);
    return "Successful";
}

void ServerLogic::check(QStringList tasks)//tasks = {task_number, answer, params}
{
    float res;
    if (tasks[0] == "task_1")
    {
        res = task_1(tasks[2]);
    }
    else if (tasks[0] == "task_2")
    {
        res = task_2(tasks[2]);
    }
    else
    {
        qDebug()<<"Not valid task name";
        return;
    }
    if(tasks[1].toFloat()==res)
    {
        res = 1;
    }
    else
    {
        res = -1;
    }
    DataBase::GetInstance()->update(tasks[0], int(res), ServerLogic::user);
}

float ServerLogic::task_1(QString params)//params = param1|param2|param3...
{
    if (params == "1")
    {
        return 1;
    }
    else
    {
        return -1;
    }
}

float ServerLogic::task_2(QString params)//params = param1|param2|param3...
{
    if (params == "2")
    {
        return 1;
    }
    else
    {
        return -1;
    }
}

QStringList ServerLogic::parse_auth(QString request_params)
{
    QStringList list;
    int pos = 0;
    while ((pos = request_params.indexOf("=", pos)) != -1) {
        pos++;
        int endPos = request_params.indexOf("&", pos);
        if (endPos == -1) {
            endPos = request_params.length();
        }
        QString value = request_params.mid(pos, endPos - pos);
        list.append(value);
        pos = endPos;
    }
    return list;
}

QString ServerLogic::parse_request(QString request)
{
    QString request_route = request.split("?")[0];
    return request_route;
}
