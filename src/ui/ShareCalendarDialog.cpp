#include "ui/ShareCalendarDialog.h"
#include "managers/AuthManager.h"
#include "managers/InvitationManager.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>

ShareCalendarDialog::ShareCalendarDialog(int calendarId, int currentUserId, QWidget *parent)
    : QDialog(parent), calendarId(calendarId), currentUserId(currentUserId)
{
    setupUi();
    setWindowTitle("Share Calendar");
    setModal(true);
}

void ShareCalendarDialog::setupUi()
{
    identifierEdit = new QLineEdit(this);
    identifierEdit->setPlaceholderText("username or email");

    roleCombo = new QComboBox(this);
    roleCombo->addItem("viewer");
    roleCombo->addItem("editor");

    errorLabel = new QLabel(this);
    errorLabel->setStyleSheet("color: red;");
    errorLabel->setVisible(false);

    shareButton = new QPushButton("Share", this);
    cancelButton = new QPushButton("Cancel", this);

    QFormLayout *formLayout = new QFormLayout();
    formLayout->addRow("User:", identifierEdit);
    formLayout->addRow("Role:", roleCombo);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(cancelButton);
    buttonLayout->addWidget(shareButton);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(formLayout);
    mainLayout->addWidget(errorLabel);
    mainLayout->addLayout(buttonLayout);

    connect(shareButton, &QPushButton::clicked, this, &ShareCalendarDialog::onShareClicked);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

void ShareCalendarDialog::onShareClicked()
{
    const QString identifier = identifierEdit->text().trimmed();
    if (identifier.isEmpty())
    {
        errorLabel->setText("Please enter a username or email.");
        errorLabel->setVisible(true);
        return;
    }

    AuthManager authManager;
    const int targetUserId = authManager.getUserId(identifier);
    if (targetUserId == -1)
    {
        errorLabel->setText("User not found.");
        errorLabel->setVisible(true);
        return;
    }

    if (targetUserId == currentUserId)
    {
        errorLabel->setText("You cannot invite yourself.");
        errorLabel->setVisible(true);
        return;
    }

    InvitationManager invitationManager;
    if (!invitationManager.createInvitation(calendarId, currentUserId, targetUserId, roleCombo->currentText()))
    {
        errorLabel->setText("Could not create invitation. User may already be a member or already invited.");
        errorLabel->setVisible(true);
        return;
    }

    accept();
}
