#ifndef EVENTMANAGER_H
#define EVENTMANAGER_H

#include <vector>
#include <QDateTime>
#include "models/Event.h"

class EventManager
{
public:
    bool createEvent(const Event &event);
    bool updateEvent(const Event &event);
    bool deleteEvent(int eventId);
    std::vector<Event> getEventsForCalendar(int calendarId) const;
    bool hasConflict(int calendarId, const QDateTime &start, const QDateTime &end, int ignoredEventId = -1);
};

#endif
