#include "models/Event.h"

Event::Event()
    : id(-1), calendarId(-1), createdBy(-1) {}

Event::Event(int id,
             int calendarId,
             const QString &title,
             const QString &description,
             const QString &location,
             const QDateTime &startDateTime,
             const QDateTime &endDateTime,
             int createdBy)
    : id(id),
      calendarId(calendarId),
      title(title),
      description(description),
      location(location),
      startDateTime(startDateTime),
      endDateTime(endDateTime),
      createdBy(createdBy) {}

int Event::getId() const
{
    return id;
}

int Event::getCalendarId() const
{
    return calendarId;
}

QString Event::getTitle() const
{
    return title;
}

QString Event::getDescription() const
{
    return description;
}

QString Event::getLocation() const
{
    return location;
}

QDateTime Event::getStartDateTime() const
{
    return startDateTime;
}

QDateTime Event::getEndDateTime() const
{
    return endDateTime;
}

int Event::getCreatedBy() const
{
    return createdBy;
}