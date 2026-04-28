#include "managers/DatabaseManager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

DatabaseManager::DatabaseManager() {}

DatabaseManager& DatabaseManager::instance()
{
    static DatabaseManager instance;
    return instance;
}

bool DatabaseManager::openDatabase()
{
    if (QSqlDatabase::contains("synccal_connection"))
    {
        db = QSqlDatabase::database("synccal_connection");
    }
    else
    {
        db = QSqlDatabase::addDatabase("QSQLITE", "synccal_connection");
        db.setDatabaseName("synccal.db");
    }

    if (!db.open())
    {
        qDebug() << "Database error:" << db.lastError().text();
        return false;
    }

    QSqlQuery pragma(db);
    pragma.exec("PRAGMA foreign_keys = ON");

    return createTables();
}

bool DatabaseManager::createTables()
{
    QSqlQuery query(db);

    if (!query.exec(
        "CREATE TABLE IF NOT EXISTS users ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "username TEXT NOT NULL,"
        "email TEXT NOT NULL UNIQUE,"
        "password TEXT NOT NULL)"
    ))
    {
        qDebug() << query.lastError().text();
        return false;
    }

    if (!query.exec(
        "CREATE TABLE IF NOT EXISTS calendars ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "name TEXT NOT NULL,"
        "owner_id INTEGER NOT NULL,"
        "type TEXT NOT NULL,"
        "FOREIGN KEY(owner_id) REFERENCES users(id))"
    ))
    {
        qDebug() << query.lastError().text();
        return false;
    }

    if (!query.exec(
        "CREATE TABLE IF NOT EXISTS calendar_members ("
        "calendar_id INTEGER NOT NULL,"
        "user_id INTEGER NOT NULL,"
        "role TEXT NOT NULL,"
        "PRIMARY KEY(calendar_id, user_id),"
        "FOREIGN KEY(calendar_id) REFERENCES calendars(id),"
        "FOREIGN KEY(user_id) REFERENCES users(id))"
    ))
    {
        qDebug() << query.lastError().text();
        return false;
    }

    if (!query.exec(
        "CREATE TABLE IF NOT EXISTS events ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "calendar_id INTEGER NOT NULL,"
        "title TEXT NOT NULL,"
        "description TEXT,"
        "location TEXT,"
        "start_datetime TEXT NOT NULL,"
        "end_datetime TEXT NOT NULL,"
        "created_by INTEGER NOT NULL,"
        "FOREIGN KEY(calendar_id) REFERENCES calendars(id),"
        "FOREIGN KEY(created_by) REFERENCES users(id))"
    ))
    {
        qDebug() << query.lastError().text();
        return false;
    }

    if (!query.exec(
        "CREATE TABLE IF NOT EXISTS calendar_invitations ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "calendar_id INTEGER NOT NULL,"
        "inviter_id INTEGER NOT NULL,"
        "invitee_id INTEGER NOT NULL,"
        "role TEXT NOT NULL,"
        "status TEXT NOT NULL DEFAULT 'pending',"
        "created_at TEXT NOT NULL,"
        "FOREIGN KEY(calendar_id) REFERENCES calendars(id),"
        "FOREIGN KEY(inviter_id) REFERENCES users(id),"
        "FOREIGN KEY(invitee_id) REFERENCES users(id))"
    ))
    {
        qDebug() << query.lastError().text();
        return false;
    }

    if (!query.exec(
        "CREATE UNIQUE INDEX IF NOT EXISTS idx_unique_pending_invitation "
        "ON calendar_invitations(calendar_id, invitee_id) "
        "WHERE status = 'pending'"
    ))
    {
        qDebug() << query.lastError().text();
        return false;
    }

    return true;
}

QSqlDatabase DatabaseManager::getDatabase() const
{
    return db;
}
