#include "managers/CalendarManager.h"
#include "managers/DatabaseManager.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>

bool CalendarManager::createCalendar(const QString &name, int ownerId, const QString &type)
{
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    QSqlQuery query(db);

    query.prepare("INSERT INTO calendars (name, owner_id, type) "
                  "VALUES (:name, :owner_id, :type)");
    query.bindValue(":name", name);
    query.bindValue(":owner_id", ownerId);
    query.bindValue(":type", type);

    if (!query.exec())
    {
        return false;
    }

    int calendarId = query.lastInsertId().toInt();

    QSqlQuery memberQuery(db);
    memberQuery.prepare("INSERT INTO calendar_members (calendar_id, user_id, role) "
                        "VALUES (:calendar_id, :user_id, :role)");
    memberQuery.bindValue(":calendar_id", calendarId);
    memberQuery.bindValue(":user_id", ownerId);
    memberQuery.bindValue(":role", "owner");

    return memberQuery.exec();
}

bool CalendarManager::addUserToCalendar(int calendarId, int userId, const QString &role)
{
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    QSqlQuery query(db);

    query.prepare("INSERT OR IGNORE INTO calendar_members (calendar_id, user_id, role) "
                  "VALUES (:calendar_id, :user_id, :role)");
    query.bindValue(":calendar_id", calendarId);
    query.bindValue(":user_id", userId);
    query.bindValue(":role", role);

    return query.exec();
}

QString CalendarManager::getUserRoleInCalendar(int calendarId, int userId)
{
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    QSqlQuery query(db);

    query.prepare("SELECT role FROM calendar_members "
                  "WHERE calendar_id = :calendar_id AND user_id = :user_id");
    query.bindValue(":calendar_id", calendarId);
    query.bindValue(":user_id", userId);

    if (!query.exec())
    {
        qWarning() << "Failed to get user role in calendar:" << query.lastError().text();
        return QString();
    }

    if (!query.next())
        return QString();

    return query.value(0).toString();
}

bool CalendarManager::deleteCalendar(int calendarId, int userId)
{
    if (getUserRoleInCalendar(calendarId, userId) != "owner")
        return false;

    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    if (!db.transaction())
    {
        qWarning() << "Failed to start delete calendar transaction:" << db.lastError().text();
        return false;
    }

    auto rollback = [&db]() {
        if (!db.rollback())
            qWarning() << "Failed to rollback delete calendar transaction:" << db.lastError().text();
    };

    QSqlQuery query(db);

    query.prepare("DELETE FROM calendar_invitations WHERE calendar_id = :calendar_id");
    query.bindValue(":calendar_id", calendarId);
    if (!query.exec())
    {
        qWarning() << "Failed to delete calendar invitations:" << query.lastError().text();
        rollback();
        return false;
    }

    query.prepare("DELETE FROM events WHERE calendar_id = :calendar_id");
    query.bindValue(":calendar_id", calendarId);
    if (!query.exec())
    {
        qWarning() << "Failed to delete calendar events:" << query.lastError().text();
        rollback();
        return false;
    }

    query.prepare("DELETE FROM calendar_members WHERE calendar_id = :calendar_id");
    query.bindValue(":calendar_id", calendarId);
    if (!query.exec())
    {
        qWarning() << "Failed to delete calendar members:" << query.lastError().text();
        rollback();
        return false;
    }

    query.prepare("DELETE FROM calendars WHERE id = :calendar_id");
    query.bindValue(":calendar_id", calendarId);
    if (!query.exec())
    {
        qWarning() << "Failed to delete calendar:" << query.lastError().text();
        rollback();
        return false;
    }

    if (!db.commit())
    {
        qWarning() << "Failed to commit delete calendar transaction:" << db.lastError().text();
        rollback();
        return false;
    }

    return true;
}

bool CalendarManager::leaveCalendar(int calendarId, int userId)
{
    const QString role = getUserRoleInCalendar(calendarId, userId);
    if (role.isEmpty() || role == "owner")
        return false;

    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    QSqlQuery query(db);

    query.prepare("DELETE FROM calendar_members "
                  "WHERE calendar_id = :calendar_id AND user_id = :user_id");
    query.bindValue(":calendar_id", calendarId);
    query.bindValue(":user_id", userId);

    if (!query.exec())
    {
        qWarning() << "Failed to leave calendar:" << query.lastError().text();
        return false;
    }

    return query.numRowsAffected() > 0;
}

std::vector<Calendar> CalendarManager::getCalendarsForUser(int userId) const
{
    std::vector<Calendar> result;

    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    QSqlQuery query(db);

    query.prepare(
        "SELECT c.id, c.name, c.owner_id, c.type "
        "FROM calendars c "
        "JOIN calendar_members cm ON c.id = cm.calendar_id "
        "WHERE cm.user_id = :user_id"
    );
    query.bindValue(":user_id", userId);

    if (!query.exec())
    {
        return result;
    }

    while (query.next())
    {
        result.push_back(Calendar(
            query.value(0).toInt(),
            query.value(1).toString(),
            query.value(2).toInt(),
            query.value(3).toString()
        ));
    }

    return result;
}
