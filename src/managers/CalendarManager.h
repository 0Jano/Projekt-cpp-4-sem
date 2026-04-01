#ifndef CALENDARMANAGER_H
#define CALENDARMANAGER_H

#include <vector>
#include <QString>
#include "models/Calendar.h"

class CalendarManager
{
public:
    bool createCalendar(const QString &name, int ownerId, const QString &type);
    bool addUserToCalendar(int calendarId, int userId, const QString &role);
    std::vector<Calendar> getCalendarsForUser(int userId) const;
};

#endif