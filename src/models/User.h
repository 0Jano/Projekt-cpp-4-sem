#ifndef USER_H
#define USER_H

#include <QString>

class User
{
public:
    User();
    User(const QString &username, const QString &email, const QString &password);

    QString getUsername() const;
    QString getEmail() const;
    QString getPassword() const;

private:
    QString username;
    QString email;
    QString password;
};

#endif