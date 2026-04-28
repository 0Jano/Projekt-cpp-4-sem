#ifndef INVITATIONMANAGER_H
#define INVITATIONMANAGER_H

#include <vector>
#include <QString>
#include "models/CalendarInvitation.h"

class InvitationManager
{
public:
    bool createInvitation(int calendarId, int inviterId, int inviteeId, const QString &role);
    std::vector<CalendarInvitation> getPendingInvitationsForUser(int userId) const;
    bool acceptInvitation(int invitationId);
    bool declineInvitation(int invitationId);
    int getPendingInvitationCountForUser(int userId) const;
};

#endif
