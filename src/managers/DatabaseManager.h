#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>

class DatabaseManager
{
public:
    static DatabaseManager& instance();

    bool openDatabase();
    bool createTables();
    QSqlDatabase getDatabase() const;

private:
    DatabaseManager();
    QSqlDatabase db;
};

#endif