#ifndef EVENTMANAGER_H
#define EVENTMANAGER_H

#include <vector>
#include "models/Event.h"

class EventManager
{
public:
    bool createEvent(const Event &event);
    bool updateEvent(const Event &event);
    bool deleteEvent(int eventId);
    std::vector<Event> getEventsForCalendar(int calendarId) const;
};

#endif