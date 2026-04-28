#include "models/CalendarInvitation.h"

CalendarInvitation::CalendarInvitation()
    : id(-1), calendarId(-1), inviterId(-1), inviteeId(-1) {}

CalendarInvitation::CalendarInvitation(int id,
                                       int calendarId,
                                       const QString &calendarName,
                                       int inviterId,
                                       const QString &inviterName,
                                       int inviteeId,
                                       const QString &role,
                                       const QString &status,
                                       const QString &createdAt)
    : id(id), calendarId(calendarId), calendarName(calendarName),
      inviterId(inviterId), inviterName(inviterName), inviteeId(inviteeId),
      role(role), status(status), createdAt(createdAt) {}

int CalendarInvitation::getId() const
{
    return id;
}

int CalendarInvitation::getCalendarId() const
{
    return calendarId;
}

QString CalendarInvitation::getCalendarName() const
{
    return calendarName;
}

int CalendarInvitation::getInviterId() const
{
    return inviterId;
}

QString CalendarInvitation::getInviterName() const
{
    return inviterName;
}

int CalendarInvitation::getInviteeId() const
{
    return inviteeId;
}

QString CalendarInvitation::getRole() const
{
    return role;
}

QString CalendarInvitation::getStatus() const
{
    return status;
}

QString CalendarInvitation::getCreatedAt() const
{
    return createdAt;
}
