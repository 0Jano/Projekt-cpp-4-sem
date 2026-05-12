#ifndef ICSMANAGER_H
#define ICSMANAGER_H

#include <QString>

class IcsManager
{
public:
    bool exportCalendarToIcs(int calendarId, const QString &filePath) const;
    bool importEventsFromIcs(int calendarId, int userId, const QString &filePath);
};

#endif
