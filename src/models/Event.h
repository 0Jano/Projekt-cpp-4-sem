#ifndef EVENT_H
#define EVENT_H

#include <QString>
#include <QDateTime>

class Event
{
public:
    Event();
    Event(int id,
          int calendarId,
          const QString &title,
          const QString &description,
          const QString &location,
          const QDateTime &startDateTime,
          const QDateTime &endDateTime,
          int createdBy);

    int getId() const;
    int getCalendarId() const;
    QString getTitle() const;
    QString getDescription() const;
    QString getLocation() const;
    QDateTime getStartDateTime() const;
    QDateTime getEndDateTime() const;
    int getCreatedBy() const;

private:
    int id;
    int calendarId;
    QString title;
    QString description;
    QString location;
    QDateTime startDateTime;
    QDateTime endDateTime;
    int createdBy;
};

#endif