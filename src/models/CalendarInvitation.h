#ifndef CALENDARINVITATION_H
#define CALENDARINVITATION_H

#include <QString>

/**
 * @brief Represents an invitation sent to a user to join a shared calendar.
 *
 * @details Stores the invitation identity, related calendar and user IDs,
 * requested role, status, and creation timestamp. Invitation state changes are
 * handled by InvitationManager.
 */
class CalendarInvitation
{
public:
    /**
     * @brief Constructs an empty calendar invitation model.
     */
    CalendarInvitation();

    /**
     * @brief Constructs a calendar invitation model with all persisted fields.
     */
    CalendarInvitation(int id,
                       int calendarId,
                       const QString &calendarName,
                       int inviterId,
                       const QString &inviterName,
                       int inviteeId,
                       const QString &role,
                       const QString &status,
                       const QString &createdAt);

    /**
     * @brief Returns the invitation ID.
     * @return Database identifier of the invitation.
     */
    int getId() const;

    /**
     * @brief Returns the shared calendar ID.
     * @return ID of the calendar being shared.
     */
    int getCalendarId() const;

    /**
     * @brief Returns the shared calendar name.
     * @return Display name of the calendar being shared.
     */
    QString getCalendarName() const;

    /**
     * @brief Returns the inviter user ID.
     * @return User ID of the sender.
     */
    int getInviterId() const;

    /**
     * @brief Returns the inviter name.
     * @return Display name of the sender.
     */
    QString getInviterName() const;

    /**
     * @brief Returns the invitee user ID.
     * @return User ID of the recipient.
     */
    int getInviteeId() const;

    /**
     * @brief Returns the offered calendar role.
     * @return Role offered to the invitee.
     */
    QString getRole() const;

    /**
     * @brief Returns the invitation status.
     * @return Current invitation status.
     */
    QString getStatus() const;

    /**
     * @brief Returns the creation timestamp.
     * @return Creation timestamp stored for the invitation.
     */
    QString getCreatedAt() const;

private:
    int id; ///< Database identifier of the invitation.
    int calendarId; ///< ID of the calendar being shared.
    QString calendarName; ///< Display name of the calendar being shared.
    int inviterId; ///< User ID of the sender.
    QString inviterName; ///< Display name of the sender.
    int inviteeId; ///< User ID of the recipient.
    QString role; ///< Role offered to the invitee.
    QString status; ///< Current invitation status.
    QString createdAt; ///< Creation timestamp stored for the invitation.
};

#endif
