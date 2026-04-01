#include "managers/AuthManager.h"
#include "managers/DatabaseManager.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>

bool AuthManager::registerUser(const User &user)
{
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    QSqlQuery query(db);

    query.prepare("INSERT INTO users (username, email, password) "
                  "VALUES (:username, :email, :password)");
    query.bindValue(":username", user.getUsername());
    query.bindValue(":email", user.getEmail());
    query.bindValue(":password", user.getPassword());

    if (!query.exec())
    {
        qDebug() << "Register error:" << query.lastError().text();
        return false;
    }

    return true;
}

bool AuthManager::loginUser(const QString &email, const QString &password) const
{
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    QSqlQuery query(db);

    query.prepare("SELECT id FROM users WHERE email = :email AND password = :password");
    query.bindValue(":email", email);
    query.bindValue(":password", password);

    if (!query.exec())
    {
        qDebug() << "Login error:" << query.lastError().text();
        return false;
    }

    return query.next();
}

int AuthManager::getUserIdByEmail(const QString &email) const
{
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    QSqlQuery query(db);

    query.prepare("SELECT id FROM users WHERE email = :email");
    query.bindValue(":email", email);

    if (!query.exec())
    {
        return -1;
    }

    if (query.next())
    {
        return query.value(0).toInt();
    }

    return -1;
}