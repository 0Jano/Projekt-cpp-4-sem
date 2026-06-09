#ifndef CALENDARMANAGER_H
#define CALENDARMANAGER_H

#include <vector>
#include <QString>
#include "models/Calendar.h"

/**
 * @brief Handles CRUD operations for calendars and calendar membership.
 *
 * @details Creates calendars, manages membership rows and roles, and queries
 * calendars visible to a user. Database operations are kept in this manager
 * instead of UI classes.
 */
class CalendarManager
{
public:
    /**
     * @brief Creates a calendar owned by a user.
     * @param name Display name of the calendar.
     * @param ownerId User ID of the calendar owner.
     * @return true if the calendar and owner membership were created successfully.
     */
    bool createCalendar(const QString &name, int ownerId);

    /**
     * @brief Adds a user to a calendar with the given role.
     * @param calendarId ID of the calendar to join.
     * @param userId ID of the user to add.
     * @param role Role assigned to the user in the calendar.
     * @return true if the insert completed or the duplicate membership was ignored.
     */
    bool addUserToCalendar(int calendarId, int userId, const QString &role);

    /**
     * @brief Returns a user's role in a calendar.
     * @param calendarId ID of the calendar to inspect.
     * @param userId ID of the user whose role should be returned.
     * @return Role string if a membership exists, or an empty string otherwise.
     */
    QString getUserRoleInCalendar(int calendarId, int userId);

    /**
     * @brief Deletes a calendar when requested by an authorized user.
     * @param calendarId ID of the calendar to delete.
     * @param userId ID of the user requesting deletion.
     * @return true if the calendar was deleted successfully.
     */
    bool deleteCalendar(int calendarId, int userId);

    /**
     * @brief Removes a user from a calendar membership.
     * @param calendarId ID of the calendar to leave.
     * @param userId ID of the user leaving the calendar.
     * @return true if the membership was removed successfully.
     */
    bool leaveCalendar(int calendarId, int userId);

    /**
     * @brief Marks a calendar as a user's favorite calendar.
     * @param userId ID of the user whose favorite calendar should be set.
     * @param calendarId ID of the calendar to mark as favorite.
     * @return true if the favorite calendar was saved successfully.
     */
    bool setFavoriteCalendar(int userId, int calendarId);

    /**
     * @brief Clears a user's favorite calendar selection.
     * @param userId ID of the user whose favorite calendar should be cleared.
     * @return true if the favorite calendar was cleared successfully.
     */
    bool clearFavoriteCalendar(int userId);

    /**
     * @brief Returns a user's favorite calendar ID.
     * @param userId ID of the user whose favorite calendar should be returned.
     * @return Favorite calendar ID if set, or -1 if no favorite exists.
     */
    int getFavoriteCalendarId(int userId);

    /**
     * @brief Returns all calendars where the user is a member.
     * @param userId ID of the user whose calendars should be loaded.
     * @return List of calendars visible to the user for any membership role.
     */
    std::vector<Calendar> getCalendarsForUser(int userId) const;
};

#endif
