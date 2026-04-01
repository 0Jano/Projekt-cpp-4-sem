#ifndef AUTHMANAGER_H
#define AUTHMANAGER_H

#include "models/User.h"
#include <QString>

class AuthManager
{
public:
    bool registerUser(const User &user);
    bool loginUser(const QString &email, const QString &password) const;
    int getUserIdByEmail(const QString &email) const;
};

#endif