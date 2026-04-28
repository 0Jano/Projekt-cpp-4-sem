#include "managers/InvitationManager.h"
#include "managers/DatabaseManager.h"

#include <QDateTime>
#include <QSqlQuery>
#include <QVariant>

bool InvitationManager::createInvitation(int calendarId, int inviterId, int inviteeId, const QString &role)
{
    if (calendarId == -1 || inviterId == -1 || inviteeId == -1)
        return false;

    QSqlDatabase db = DatabaseManager::instance().getDatabase();

    QSqlQuery userQuery(db);
    userQuery.prepare("SELECT id FROM users WHERE id = :user_id");
    userQuery.bindValue(":user_id", inviteeId);
    if (!userQuery.exec() || !userQuery.next())
        return false;

    QSqlQuery memberQuery(db);
    memberQuery.prepare(
        "SELECT 1 FROM calendar_members "
        "WHERE calendar_id = :calendar_id AND user_id = :user_id"
    );
    memberQuery.bindValue(":calendar_id", calendarId);
    memberQuery.bindValue(":user_id", inviteeId);
    if (!memberQuery.exec() || memberQuery.next())
        return false;

    QSqlQuery pendingQuery(db);
    pendingQuery.prepare(
        "SELECT 1 FROM calendar_invitations "
        "WHERE calendar_id = :calendar_id AND invitee_id = :invitee_id AND status = 'pending'"
    );
    pendingQuery.bindValue(":calendar_id", calendarId);
    pendingQuery.bindValue(":invitee_id", inviteeId);
    if (!pendingQuery.exec() || pendingQuery.next())
        return false;

    QSqlQuery insertQuery(db);
    insertQuery.prepare(
        "INSERT INTO calendar_invitations "
        "(calendar_id, inviter_id, invitee_id, role, status, created_at) "
        "VALUES (:calendar_id, :inviter_id, :invitee_id, :role, :status, :created_at)"
    );
    insertQuery.bindValue(":calendar_id", calendarId);
    insertQuery.bindValue(":inviter_id", inviterId);
    insertQuery.bindValue(":invitee_id", inviteeId);
    insertQuery.bindValue(":role", role);
    insertQuery.bindValue(":status", "pending");
    insertQuery.bindValue(":created_at", QDateTime::currentDateTime().toString(Qt::ISODate));

    return insertQuery.exec();
}

std::vector<CalendarInvitation> InvitationManager::getPendingInvitationsForUser(int userId) const
{
    std::vector<CalendarInvitation> result;

    if (userId == -1)
        return result;

    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    QSqlQuery query(db);

    query.prepare(
        "SELECT i.id, i.calendar_id, c.name, i.inviter_id, u.username, "
        "i.invitee_id, i.role, i.status, i.created_at "
        "FROM calendar_invitations i "
        "JOIN calendars c ON i.calendar_id = c.id "
        "JOIN users u ON i.inviter_id = u.id "
        "WHERE i.invitee_id = :user_id AND i.status = 'pending'"
    );
    query.bindValue(":user_id", userId);

    if (!query.exec())
        return result;

    while (query.next())
    {
        result.push_back(CalendarInvitation(
            query.value(0).toInt(),
            query.value(1).toInt(),
            query.value(2).toString(),
            query.value(3).toInt(),
            query.value(4).toString(),
            query.value(5).toInt(),
            query.value(6).toString(),
            query.value(7).toString(),
            query.value(8).toString()
        ));
    }

    return result;
}

bool InvitationManager::acceptInvitation(int invitationId)
{
    QSqlDatabase db = DatabaseManager::instance().getDatabase();

    if (!db.transaction())
        return false;

    QSqlQuery selectQuery(db);
    selectQuery.prepare(
        "SELECT calendar_id, invitee_id, role "
        "FROM calendar_invitations "
        "WHERE id = :id AND status = 'pending'"
    );
    selectQuery.bindValue(":id", invitationId);

    if (!selectQuery.exec() || !selectQuery.next())
    {
        db.rollback();
        return false;
    }

    const int calendarId = selectQuery.value(0).toInt();
    const int inviteeId = selectQuery.value(1).toInt();
    const QString role = selectQuery.value(2).toString();

    QSqlQuery memberQuery(db);
    memberQuery.prepare(
        "INSERT OR IGNORE INTO calendar_members(calendar_id, user_id, role) "
        "VALUES(:calendar_id, :user_id, :role)"
    );
    memberQuery.bindValue(":calendar_id", calendarId);
    memberQuery.bindValue(":user_id", inviteeId);
    memberQuery.bindValue(":role", role);

    if (!memberQuery.exec())
    {
        db.rollback();
        return false;
    }

    QSqlQuery updateQuery(db);
    updateQuery.prepare(
        "UPDATE calendar_invitations "
        "SET status = 'accepted' "
        "WHERE id = :id AND status = 'pending'"
    );
    updateQuery.bindValue(":id", invitationId);

    if (!updateQuery.exec() || updateQuery.numRowsAffected() != 1)
    {
        db.rollback();
        return false;
    }

    return db.commit();
}

bool InvitationManager::declineInvitation(int invitationId)
{
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    QSqlQuery query(db);

    query.prepare(
        "UPDATE calendar_invitations "
        "SET status = 'declined' "
        "WHERE id = :id AND status = 'pending'"
    );
    query.bindValue(":id", invitationId);

    return query.exec() && query.numRowsAffected() == 1;
}

int InvitationManager::getPendingInvitationCountForUser(int userId) const
{
    if (userId == -1)
        return 0;

    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    QSqlQuery query(db);

    query.prepare(
        "SELECT COUNT(*) FROM calendar_invitations "
        "WHERE invitee_id = :user_id AND status = 'pending'"
    );
    query.bindValue(":user_id", userId);

    if (!query.exec() || !query.next())
        return 0;

    return query.value(0).toInt();
}
