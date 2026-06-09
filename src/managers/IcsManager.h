#ifndef ICSMANAGER_H
#define ICSMANAGER_H

#include <QString>

/**
 * @brief Imports and exports calendar events using ICS files.
 *
 * @details Provides file-based interoperability for calendar events by writing
 * events to iCalendar format and importing VEVENT entries into an existing
 * calendar. Event persistence is delegated to EventManager.
 */
class IcsManager
{
public:
    /**
     * @brief Exports all events from a calendar to an ICS file.
     * @param calendarId ID of the calendar whose events should be exported.
     * @param filePath Destination file path for the generated ICS file.
     * @return true if the file was written successfully.
     */
    bool exportCalendarToIcs(int calendarId, const QString &filePath) const;

    /**
     * @brief Imports events from an ICS file into a calendar.
     * @param calendarId ID of the calendar that should receive imported events.
     * @param userId ID of the user recorded as the creator of imported events.
     * @param filePath Source file path of the ICS file to import.
     * @return true if at least one event was imported successfully.
     */
    bool importEventsFromIcs(int calendarId, int userId, const QString &filePath);
};

#endif
