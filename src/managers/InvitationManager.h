#ifndef INVITATIONMANAGER_H
#define INVITATIONMANAGER_H

#include <vector>
#include <QString>
#include "models/CalendarInvitation.h"

/**
 * @brief Manages calendar invitations for shared calendars.
 *
 * @details Creates pending invitations, queries invite lists and badge counts,
 * and records accept or decline decisions. Accepting an invitation also adds
 * the invitee to calendar membership.
 */
class InvitationManager
{
public:
    /**
     * @brief Creates a pending calendar invitation.
     * @param calendarId ID of the calendar being shared.
     * @param inviterId User ID of the sender.
     * @param inviteeId User ID of the recipient.
     * @param role Role offered to the invitee.
     * @return true if the invitation was created; false if membership or a pending invite already exists.
     */
    bool createInvitation(int calendarId, int inviterId, int inviteeId, const QString &role);

    /**
     * @brief Returns pending invitations for a user.
     * @param userId ID of the user whose pending invitations should be loaded.
     * @return List of pending invitations for the user.
     */
    std::vector<CalendarInvitation> getPendingInvitationsForUser(int userId) const;

    /**
     * @brief Accepts a pending invitation.
     * @param invitationId ID of the invitation to accept.
     * @return true if the invitee was added to the calendar and the invitation was marked accepted.
     */
    bool acceptInvitation(int invitationId);

    /**
     * @brief Declines a pending invitation.
     * @param invitationId ID of the invitation to decline.
     * @return true if the invitation was marked declined.
     */
    bool declineInvitation(int invitationId);

    /**
     * @brief Counts pending invitations for badge display.
     * @param userId ID of the user whose pending invitations should be counted.
     * @return Number of pending invitations for the user.
     */
    int getPendingInvitationCountForUser(int userId) const;
};

#endif
