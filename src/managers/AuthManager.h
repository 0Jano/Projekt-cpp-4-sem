#ifndef AUTHMANAGER_H
#define AUTHMANAGER_H

#include "models/User.h"
#include <QString>

/**
 * @brief Handles user registration and authentication against the SQLite database.
 *
 * @details Provides methods to register users, verify login credentials by
 * email or username, and resolve user identifiers to database IDs. Database
 * access is performed through DatabaseManager.
 */
class AuthManager
{
public:
    /**
     * @brief Registers a new user in the database.
     * @param user User object containing username, email, and password.
     * @return true if registration succeeded; false if the email is already taken.
     */
    bool registerUser(const User &user);

    /**
     * @brief Authenticates a user by email or username and password.
     * @param identifier Email address or username.
     * @param password Password value to compare against the stored record.
     * @return true if the credentials match a user record.
     */
    bool loginUser(const QString &identifier, const QString &password) const;

    /**
     * @brief Resolves an email address or username to a numeric user ID.
     * @param identifier Email address or username.
     * @return User ID if found, or -1 if no matching user exists.
     */
    int getUserId(const QString &identifier) const;
};

#endif
