#ifndef DB_SINGLETON_H
#define DB_SINGLETON_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QVariant>

class DataBase;

class DataBaseDestroyer
{
private:
    DataBase * DataBasePointer;
public:
    ~DataBaseDestroyer(){
        delete DataBasePointer;
    }
    void initialize(DataBase * p){
        DataBasePointer = p;
    }
};

class DataBase
{
private:
    static DataBase * p_instance;
    static DataBaseDestroyer destroyer;
    QSqlDatabase db;
    QStringList sendQuerry(QStringList str);
protected:
    DataBase();
    DataBase(const DataBase&) = delete;
    DataBase& operator = (DataBase &) = delete;

    friend class DataBaseDestroyer;
public:
    ~DataBase();
    static DataBase* GetInstance();
    QStringList auth(QString Loing, QString Password);
    QStringList reg(QString Loing, QString Password, QString Email);
    QStringList statistics(QString Loing);
    void update(QString task, int res, QString Login);
};



/*
         * str = {
         * "select login password from User where login=:param1 and password = :param2"
         * log,
         * pas}
         * return:
         * {"user&123"}
         * {}
         *
         * str = {
         * "select login task1 task2 from User where login = :param1"
         * log* }
         *          *
         * return:
         * {"user&12&0",
         * "qwerty&-3&2"}


DataBase * DataBase::p_instance = nullptr;
DataBaseDestroyer DataBase::destroyer;
*/
#endif // DB_SINGLETON_H
