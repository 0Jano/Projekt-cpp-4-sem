#ifndef AUTHMANAGER_H
#define AUTHMANAGER_H

#include <vector>
#include "User.h"

class AuthManager
{
public:
    bool registerUser(const User &user);
    bool loginUser(const QString &email, const QString &password) const;

private:
    std::vector<User> users;
};

#endif