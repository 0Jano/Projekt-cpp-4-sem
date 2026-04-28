#include "ui/InvitationsDialog.h"
#include "managers/InvitationManager.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidgetItem>

InvitationsDialog::InvitationsDialog(int userId, QWidget *parent)
    : QDialog(parent), userId(userId)
{
    setupUi();
    setWindowTitle("Invitations");
    setModal(true);
    setMinimumWidth(360);
    loadInvitations();
}

void InvitationsDialog::setupUi()
{
    emptyLabel = new QLabel("No pending invitations.", this);
    listWidget = new QListWidget(this);
    acceptButton = new QPushButton("Accept", this);
    declineButton = new QPushButton("Decline", this);
    closeButton = new QPushButton("Close", this);

    acceptButton->setEnabled(false);
    declineButton->setEnabled(false);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(acceptButton);
    buttonLayout->addWidget(declineButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(closeButton);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(emptyLabel);
    mainLayout->addWidget(listWidget);
    mainLayout->addLayout(buttonLayout);

    connect(acceptButton, &QPushButton::clicked, this, &InvitationsDialog::onAcceptClicked);
    connect(declineButton, &QPushButton::clicked, this, &InvitationsDialog::onDeclineClicked);
    connect(closeButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(listWidget, &QListWidget::itemSelectionChanged, this, &InvitationsDialog::onSelectionChanged);
}

void InvitationsDialog::loadInvitations()
{
    listWidget->clear();
    acceptButton->setEnabled(false);
    declineButton->setEnabled(false);

    InvitationManager invitationManager;
    invitations = invitationManager.getPendingInvitationsForUser(userId);

    emptyLabel->setVisible(invitations.empty());
    listWidget->setVisible(!invitations.empty());

    for (const CalendarInvitation &invitation : invitations)
    {
        const QString text = QString("Calendar: %1\nInvited by: %2\nRole: %3")
            .arg(invitation.getCalendarName(), invitation.getInviterName(), invitation.getRole());
        QListWidgetItem *item = new QListWidgetItem(text, listWidget);
        item->setData(Qt::UserRole, invitation.getId());
    }
}

void InvitationsDialog::onAcceptClicked()
{
    QListWidgetItem *item = listWidget->currentItem();
    if (!item)
        return;

    InvitationManager invitationManager;
    if (invitationManager.acceptInvitation(item->data(Qt::UserRole).toInt()))
    {
        emit invitationsChanged();
        loadInvitations();
    }
}

void InvitationsDialog::onDeclineClicked()
{
    QListWidgetItem *item = listWidget->currentItem();
    if (!item)
        return;

    InvitationManager invitationManager;
    if (invitationManager.declineInvitation(item->data(Qt::UserRole).toInt()))
    {
        emit invitationsChanged();
        loadInvitations();
    }
}

void InvitationsDialog::onSelectionChanged()
{
    const bool hasSelection = listWidget->currentItem() != nullptr;
    acceptButton->setEnabled(hasSelection);
    declineButton->setEnabled(hasSelection);
}
