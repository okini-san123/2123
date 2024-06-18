#include "db_singleton.h"
#include"sha.h"

DataBase * DataBase::p_instance = nullptr;
DataBaseDestroyer DataBase::destroyer;

DataBase::DataBase()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("../db_sinleton/Test.db");

    if(db.open())
        qDebug()<<db.lastError().text();


    if(!db.tables().contains("User")) ///????????????????????????????
    {
        QSqlQuery query(db);
        query.exec("CREATE TABLE User("
                   "login VARCHAR(20) NOT NULL, "
                   "password VARCHAR(100) NOT NULL,"
                   "email VARCHAR(20) NOT NULL,"
                   "task1 INT,"
                   "task2 INT,"
                   ")");
    }
}

DataBase::~DataBase(){db.close();}

DataBase* DataBase::GetInstance()
{
    if (!p_instance)
    {
        p_instance = new DataBase();
        destroyer.initialize(p_instance);
    }
    return p_instance;
}

QStringList DataBase::sendQuerry(QStringList str)
{
    QSqlQuery query(db);
    QStringList res;
    query.prepare(str[0]);
    int i = 1;
    while (i < str.size())
    {
        query.bindValue(str[i], str[i + 1]);
        i += 2;
    }

 //   if (str[0].contains("SELECT"))
 //   {
        if (query.exec())
        {
        int num = query.record().count();
        QString res_str;
        while(query.next())
        {
            int j = 0;
            while(j<num)
            {
                res_str+=query.value(j).toString()+"%";
                j++;
            }
            res.append(res_str);
        }
 //           res.append();///?????????????????? результат запроса
        }
 //   }
    return res;
}

QStringList DataBase::auth(QString Login, QString Password)
{
    QStringList res, res2;
    res.append("SELECT login FROM users WHERE login = :login and password = :password;");
    res.append(Login);
    res.append(SHA::get_hash(Password));
    res = DataBase::GetInstance()->sendQuerry(res);
    if (res.size() > 0)
    {
        res2.append("auth+");
        res2.append(res);
    }
    else
    {
        res2.append("auth-");
    }
    return res2;
}

QStringList DataBase::reg(QString Login, QString Password, QString Email)
{
    QStringList res, res2;
    res.append("SELECT login FROM users WHERE login = :login and password = :password;");
    res.append(Login);
    res.append(SHA::get_hash(Password));
    res = DataBase::GetInstance()->sendQuerry(res);
    if (res.size() > 0)
    {
        return res;
    }
    else
    {
        res.append("INSERT INTO users (login, password, email, task1, task2) VALUES (:login, :password, :email, 0,  0);");
        res.append(Login);
        res.append(SHA::get_hash(Password));
        res.append(Email);
        res = DataBase::GetInstance()->sendQuerry(res);
        if (res.size() > 0)
        {
            res2.append("reg-");
        }
        else
        {
            res2.append("reg+");
            res2.append(Login);
        }
        return res2;
    }
}

QStringList DataBase::statistics(QString Login)
{
    QStringList res, res2;
    res.append("SELECT login, task1, task2 FROM users WHERE login = :login;");
    res.append(Login);
    res = DataBase::GetInstance()->sendQuerry(res);
    if (res.size() > 0)
    {
        res2.append("stat+");
        res2.append(res);
        //return QByteArray("statisticsComplete\r\n");
    }
    else
    {
        res2.append("stat-");
        //return QByteArray("statisticsError\r\n");
    }
    return res2;
}

void DataBase::update(QString task, int result, QString Login)
{
    QStringList res;
    res.append("update users set "+ task+" = "+task+" + "+ QString :: number (result)+" WHERE login = :login;");
    res.append(Login);
    res = DataBase::GetInstance()->sendQuerry(res);
}
