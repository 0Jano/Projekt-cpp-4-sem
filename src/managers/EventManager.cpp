#include "managers/EventManager.h"
#include "managers/DatabaseManager.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDateTime>
#include <QDebug>

bool EventManager::createEvent(const Event &event)
{
    if (event.getCalendarId() <= 0)
    {
        qWarning() << "Cannot create event without a valid calendar id.";
        return false;
    }

    if (event.getStartDateTime() >= event.getEndDateTime())
    {
        qWarning() << "Cannot create event with invalid date range.";
        return false;
    }

    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    QSqlQuery query(db);

    query.prepare("INSERT INTO events "
                  "(calendar_id, title, description, location, start_datetime, end_datetime, created_by) "
                  "VALUES (:calendar_id, :title, :description, :location, :start_datetime, :end_datetime, :created_by)");

    query.bindValue(":calendar_id", event.getCalendarId());
    query.bindValue(":title", event.getTitle());
    query.bindValue(":description", event.getDescription());
    query.bindValue(":location", event.getLocation());
    query.bindValue(":start_datetime", event.getStartDateTime().toString(Qt::ISODate));
    query.bindValue(":end_datetime", event.getEndDateTime().toString(Qt::ISODate));
    query.bindValue(":created_by", event.getCreatedBy());

    if (!query.exec())
    {
        qWarning() << "Failed to create event:" << query.lastError().text();
        return false;
    }

    return true;
}

bool EventManager::updateEvent(const Event &event)
{
    if (event.getStartDateTime() >= event.getEndDateTime())
    {
        qWarning() << "Cannot update event with invalid date range.";
        return false;
    }

    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    QSqlQuery query(db);

    query.prepare("UPDATE events SET "
                  "title = :title, "
                  "description = :description, "
                  "location = :location, "
                  "start_datetime = :start_datetime, "
                  "end_datetime = :end_datetime "
                  "WHERE id = :id");

    query.bindValue(":title", event.getTitle());
    query.bindValue(":description", event.getDescription());
    query.bindValue(":location", event.getLocation());
    query.bindValue(":start_datetime", event.getStartDateTime().toString(Qt::ISODate));
    query.bindValue(":end_datetime", event.getEndDateTime().toString(Qt::ISODate));
    query.bindValue(":id", event.getId());

    if (!query.exec())
    {
        qWarning() << "Failed to update event:" << query.lastError().text();
        return false;
    }

    return true;
}

bool EventManager::deleteEvent(int eventId)
{
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    QSqlQuery query(db);

    query.prepare("DELETE FROM events WHERE id = :id");
    query.bindValue(":id", eventId);

    return query.exec();
}

bool EventManager::hasConflict(int calendarId, const QDateTime &start, const QDateTime &end, int ignoredEventId)
{
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    QSqlQuery query(db);

    QString sql = "SELECT 1 FROM events "
                  "WHERE calendar_id = :calendar_id "
                  "AND start_datetime < :new_end "
                  "AND end_datetime > :new_start";

    if (ignoredEventId != -1)
    {
        sql += " AND id != :ignored_event_id";
    }

    sql += " LIMIT 1";

    query.prepare(sql);
    query.bindValue(":calendar_id", calendarId);
    query.bindValue(":new_start", start.toString(Qt::ISODate));
    query.bindValue(":new_end", end.toString(Qt::ISODate));

    if (ignoredEventId != -1)
        query.bindValue(":ignored_event_id", ignoredEventId);

    if (!query.exec())
    {
        qWarning() << "Failed to check event conflict:" << query.lastError().text();
        return false;
    }

    return query.next();
}

std::vector<Event> EventManager::getEventsForCalendar(int calendarId) const
{
    std::vector<Event> result;

    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    QSqlQuery query(db);

    query.prepare("SELECT id, calendar_id, title, description, location, start_datetime, end_datetime, created_by "
                  "FROM events WHERE calendar_id = :calendar_id");
    query.bindValue(":calendar_id", calendarId);

    if (!query.exec())
    {
        return result;
    }

    while (query.next())
    {
        result.push_back(Event(
            query.value(0).toInt(),
            query.value(1).toInt(),
            query.value(2).toString(),
            query.value(3).toString(),
            query.value(4).toString(),
            QDateTime::fromString(query.value(5).toString(), Qt::ISODate),
            QDateTime::fromString(query.value(6).toString(), Qt::ISODate),
            query.value(7).toInt()
        ));
    }

    return result;
}
