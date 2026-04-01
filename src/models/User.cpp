#include "models/User.h"

User::User() {}

User::User(const QString &username, const QString &email, const QString &password)
    : username(username), email(email), password(password) {}

QString User::getUsername() const
{
    return username;
}

QString User::getEmail() const
{
    return email;
}

QString User::getPassword() const
{
    return password;
}