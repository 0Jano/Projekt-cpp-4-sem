#include "managers/CalendarManager.h"
#include "managers/DatabaseManager.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>

bool CalendarManager::createCalendar(const QString &name, int ownerId)
{
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    QSqlQuery query(db);

    query.prepare("INSERT INTO calendars (name, owner_id) "
                  "VALUES (:name, :owner_id)");
    query.bindValue(":name", name);
    query.bindValue(":owner_id", ownerId);

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

    query.prepare("UPDATE users SET favorite_calendar_id = NULL "
                  "WHERE favorite_calendar_id = :calendar_id");
    query.bindValue(":calendar_id", calendarId);
    if (!query.exec())
    {
        qWarning() << "Failed to clear favorite calendar:" << query.lastError().text();
        rollback();
        return false;
    }

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
    if (!db.transaction())
    {
        qWarning() << "Failed to start leave calendar transaction:" << db.lastError().text();
        return false;
    }

    auto rollback = [&db]() {
        if (!db.rollback())
            qWarning() << "Failed to rollback leave calendar transaction:" << db.lastError().text();
    };

    QSqlQuery query(db);

    query.prepare("UPDATE users SET favorite_calendar_id = NULL "
                  "WHERE id = :user_id AND favorite_calendar_id = :calendar_id");
    query.bindValue(":user_id", userId);
    query.bindValue(":calendar_id", calendarId);
    if (!query.exec())
    {
        qWarning() << "Failed to clear favorite calendar:" << query.lastError().text();
        rollback();
        return false;
    }

    query.prepare("DELETE FROM calendar_members "
                  "WHERE calendar_id = :calendar_id AND user_id = :user_id");
    query.bindValue(":calendar_id", calendarId);
    query.bindValue(":user_id", userId);

    if (!query.exec())
    {
        qWarning() << "Failed to leave calendar:" << query.lastError().text();
        rollback();
        return false;
    }

    if (query.numRowsAffected() <= 0)
    {
        rollback();
        return false;
    }

    if (!db.commit())
    {
        qWarning() << "Failed to commit leave calendar transaction:" << db.lastError().text();
        rollback();
        return false;
    }

    return true;
}

bool CalendarManager::setFavoriteCalendar(int userId, int calendarId)
{
    if (getUserRoleInCalendar(calendarId, userId).isEmpty())
        return false;

    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    QSqlQuery query(db);

    query.prepare("UPDATE users SET favorite_calendar_id = :calendar_id "
                  "WHERE id = :user_id");
    query.bindValue(":calendar_id", calendarId);
    query.bindValue(":user_id", userId);

    if (!query.exec())
    {
        qWarning() << "Failed to set favorite calendar:" << query.lastError().text();
        return false;
    }

    return query.numRowsAffected() > 0;
}

bool CalendarManager::clearFavoriteCalendar(int userId)
{
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    QSqlQuery query(db);

    query.prepare("UPDATE users SET favorite_calendar_id = NULL "
                  "WHERE id = :user_id");
    query.bindValue(":user_id", userId);

    if (!query.exec())
    {
        qWarning() << "Failed to clear favorite calendar:" << query.lastError().text();
        return false;
    }

    return query.numRowsAffected() > 0;
}

int CalendarManager::getFavoriteCalendarId(int userId)
{
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    QSqlQuery query(db);

    query.prepare("SELECT favorite_calendar_id FROM users WHERE id = :user_id");
    query.bindValue(":user_id", userId);

    if (!query.exec())
    {
        qWarning() << "Failed to get favorite calendar:" << query.lastError().text();
        return -1;
    }

    if (!query.next() || query.value(0).isNull())
        return -1;

    const int calendarId = query.value(0).toInt();
    if (!getUserRoleInCalendar(calendarId, userId).isEmpty())
        return calendarId;

    QSqlQuery clearQuery(db);
    clearQuery.prepare("UPDATE users SET favorite_calendar_id = NULL WHERE id = :user_id");
    clearQuery.bindValue(":user_id", userId);
    if (!clearQuery.exec())
        qWarning() << "Failed to clear inaccessible favorite calendar:" << clearQuery.lastError().text();

    return -1;
}

std::vector<Calendar> CalendarManager::getCalendarsForUser(int userId) const
{
    std::vector<Calendar> result;

    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    QSqlQuery query(db);

    query.prepare(
        "SELECT c.id, c.name, c.owner_id "
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
            query.value(2).toInt()
        ));
    }

    return result;
}
