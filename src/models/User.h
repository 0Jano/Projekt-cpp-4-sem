#ifndef USER_H
#define USER_H

#include <QString>

/**
 * @brief Represents a registered user of the application.
 *
 * @details Stores the username, email address, and password value used by
 * the authentication layer. The class is a lightweight data model passed
 * between UI and manager code.
 */
class User
{
public:
    /**
     * @brief Constructs an empty user model.
     */
    User();

    /**
     * @brief Constructs a user model with account credentials.
     */
    User(const QString &username, const QString &email, const QString &password);

    /**
     * @brief Returns the user's username.
     * @return Username stored in this model.
     */
    QString getUsername() const;

    /**
     * @brief Returns the user's email address.
     * @return Email address stored in this model.
     */
    QString getEmail() const;

    /**
     * @brief Returns the user's password value.
     * @return Password value stored in this model.
     */
    QString getPassword() const;

private:
    QString username; ///< Username used for display and login.
    QString email; ///< Email address associated with the user.
    QString password; ///< Password value used by authentication.
};

#endif
