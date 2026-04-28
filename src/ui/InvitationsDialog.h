#ifndef INVITATIONSDIALOG_H
#define INVITATIONSDIALOG_H

#include <QDialog>
#include <QListWidget>
#include <QPushButton>
#include <QLabel>
#include <vector>
#include "models/CalendarInvitation.h"

class InvitationsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit InvitationsDialog(int userId, QWidget *parent = nullptr);

signals:
    void invitationsChanged();

private slots:
    void onAcceptClicked();
    void onDeclineClicked();
    void onSelectionChanged();

private:
    int userId;
    QListWidget *listWidget;
    QPushButton *acceptButton;
    QPushButton *declineButton;
    QPushButton *closeButton;
    QLabel *emptyLabel;
    std::vector<CalendarInvitation> invitations;

    void setupUi();
    void loadInvitations();
};

#endif
