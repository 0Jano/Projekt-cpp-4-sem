#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>
#include <QString>

class DatabaseManager
{
public:
    static DatabaseManager& instance();

    bool openDatabase(const QString &databaseName = "synccal.db");
    bool createTables();
    QSqlDatabase getDatabase() const;

private:
    DatabaseManager();
    QSqlDatabase db;
};

#endif
