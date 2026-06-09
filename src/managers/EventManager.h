#ifndef EVENTMANAGER_H
#define EVENTMANAGER_H

#include <vector>
#include <QDateTime>
#include "models/Event.h"

/**
 * @brief Handles CRUD operations for events stored in the events table.
 *
 * @details Creates, updates, deletes, and queries calendar events through the
 * SQLite database. It also provides conflict checks for event time ranges in a
 * single calendar.
 */
class EventManager
{
public:
    /**
     * @brief Inserts a new event row.
     * @param event Event model containing the values to insert.
     * @return true if the event was created successfully.
     */
    bool createEvent(const Event &event);

    /**
     * @brief Updates an existing event row by event ID.
     * @param event Event model containing the updated values and target ID.
     * @return true if the event was updated successfully.
     */
    bool updateEvent(const Event &event);

    /**
     * @brief Deletes an event row by ID.
     * @param eventId Database identifier of the event to delete.
     * @return true if the event was deleted successfully.
     */
    bool deleteEvent(int eventId);

    /**
     * @brief Returns all events belonging to a calendar.
     * @param calendarId ID of the calendar whose events should be loaded.
     * @return List of events for the given calendar.
     */
    std::vector<Event> getEventsForCalendar(int calendarId) const;

    /**
     * @brief Checks whether a time range conflicts with existing events.
     * @param calendarId ID of the calendar to check.
     * @param start Start date and time of the proposed event.
     * @param end End date and time of the proposed event.
     * @param ignoredEventId Event ID to ignore during edit checks.
     * @return true if an overlapping event exists.
     */
    bool hasConflict(int calendarId, const QDateTime &start, const QDateTime &end, int ignoredEventId = -1);
};

#endif
