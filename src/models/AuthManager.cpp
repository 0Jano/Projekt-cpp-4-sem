#include "AuthManager.h"

bool AuthManager::registerUser(const User &user)
{
    for (const User &existingUser : users)
    {
        if (existingUser.getEmail() == user.getEmail())
        {
            return false;
        }
    }

    users.push_back(user);
    return true;
}

bool AuthManager::loginUser(const QString &email, const QString &password) const
{
    for (const User &user : users)
    {
        if (user.getEmail() == email && user.getPassword() == password)
        {
            return true;
        }
    }

    return false;
}