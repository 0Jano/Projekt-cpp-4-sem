#ifndef CALENDAR_H
#define CALENDAR_H

#include <QString>

/**
 * @brief Represents a calendar owned by a user.
 *
 * @details Stores the database identifier, display name, and owner ID for a
 * calendar. Calendar membership and permissions are handled by manager classes.
 */
class Calendar
{
public:
    /**
     * @brief Constructs an empty calendar model.
     */
    Calendar();

    /**
     * @brief Constructs a calendar model with database fields.
     */
    Calendar(int id, const QString &name, int ownerId);

    /**
     * @brief Returns the calendar ID.
     * @return Database identifier of the calendar.
     */
    int getId() const;

    /**
     * @brief Returns the calendar name.
     * @return Display name of the calendar.
     */
    QString getName() const;

    /**
     * @brief Returns the owner user ID.
     * @return User ID of the calendar owner.
     */
    int getOwnerId() const;

private:
    int id; ///< Database identifier of the calendar.
    QString name; ///< Display name of the calendar.
    int ownerId; ///< User ID of the calendar owner.
};

#endif
