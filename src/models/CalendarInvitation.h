#ifndef CALENDARINVITATION_H
#define CALENDARINVITATION_H

#include <QString>

class CalendarInvitation
{
public:
    CalendarInvitation();
    CalendarInvitation(int id,
                       int calendarId,
                       const QString &calendarName,
                       int inviterId,
                       const QString &inviterName,
                       int inviteeId,
                       const QString &role,
                       const QString &status,
                       const QString &createdAt);

    int getId() const;
    int getCalendarId() const;
    QString getCalendarName() const;
    int getInviterId() const;
    QString getInviterName() const;
    int getInviteeId() const;
    QString getRole() const;
    QString getStatus() const;
    QString getCreatedAt() const;

private:
    int id;
    int calendarId;
    QString calendarName;
    int inviterId;
    QString inviterName;
    int inviteeId;
    QString role;
    QString status;
    QString createdAt;
};

#endif
