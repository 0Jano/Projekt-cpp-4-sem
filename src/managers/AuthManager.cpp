#include "managers/AuthManager.h"
#include "managers/DatabaseManager.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>
#include <QCryptographicHash>

static QString hashPassword(const QString &password)
{
    return QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256).toHex();
}

bool AuthManager::registerUser(const User &user)
{
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    QSqlQuery query(db);

    query.prepare("INSERT INTO users (username, email, password) "
                  "VALUES (:username, :email, :password)");
    query.bindValue(":username", user.getUsername());
    query.bindValue(":email", user.getEmail());
    query.bindValue(":password", hashPassword(user.getPassword()));

    if (!query.exec())
    {
        qDebug() << "Register error:" << query.lastError().text();
        return false;
    }

    return true;
}

bool AuthManager::loginUser(const QString &identifier, const QString &password) const
{
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    QSqlQuery query(db);

    query.prepare("SELECT id FROM users WHERE (email = :identifier OR username = :identifier) AND password = :password");
    query.bindValue(":identifier", identifier);
    query.bindValue(":password", hashPassword(password));

    if (!query.exec())
    {
        qDebug() << "Login error:" << query.lastError().text();
        return false;
    }

    return query.next();
}

int AuthManager::getUserId(const QString &identifier) const
{
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    QSqlQuery query(db);

    query.prepare("SELECT id FROM users WHERE email = :identifier OR username = :identifier");
    query.bindValue(":identifier", identifier);

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
