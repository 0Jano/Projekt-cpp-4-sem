#include "ui/CalendarListWidget.h"
#include "ui/CreateCalendarDialog.h"
#include "ui/ShareCalendarDialog.h"
#include "managers/CalendarManager.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QAbstractButton>
#include <QIcon>

static void clearMessageBoxButtonIcons(QMessageBox *messageBox)
{
    for (QAbstractButton *button : messageBox->buttons())
        button->setIcon(QIcon());
}

CalendarListWidget::CalendarListWidget(QWidget *parent)
    : QWidget(parent), userId(-1)
{
    setupUi();
}

void CalendarListWidget::setupUi()
{
    titleLabel = new QLabel("Calendars", this);
    listWidget = new QListWidget(this);
    addButton = new QPushButton("+ New Calendar", this);
    shareButton = new QPushButton("Share", this);
    shareButton->setEnabled(false);
    deleteLeaveButton = new QPushButton("Delete / Leave", this);
    deleteLeaveButton->setEnabled(false);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(shareButton);
    buttonLayout->addWidget(deleteLeaveButton);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(listWidget);
    mainLayout->addLayout(buttonLayout);

    setFixedWidth(220);

    connect(addButton, &QPushButton::clicked, this, &CalendarListWidget::onAddClicked);
    connect(shareButton, &QPushButton::clicked, this, &CalendarListWidget::onShareClicked);
    connect(deleteLeaveButton, &QPushButton::clicked, this, &CalendarListWidget::onDeleteLeaveClicked);
    connect(listWidget, &QListWidget::itemClicked, this, &CalendarListWidget::onItemClicked);
    connect(listWidget, &QListWidget::itemSelectionChanged, this, &CalendarListWidget::onSelectionChanged);
}

void CalendarListWidget::setUserId(int id)
{
    userId = id;
    refresh();
}

void CalendarListWidget::refresh()
{
    listWidget->clear();
    calendars.clear();
    shareButton->setEnabled(false);
    deleteLeaveButton->setEnabled(false);

    if (userId == -1)
        return;

    CalendarManager calendarManager;
    calendars = calendarManager.getCalendarsForUser(userId);

    for (const Calendar &calendar : calendars)
    {
        QListWidgetItem *item = new QListWidgetItem(calendar.getName(), listWidget);
        item->setData(Qt::UserRole, calendar.getId());
    }
}

void CalendarListWidget::onAddClicked()
{
    if (userId == -1)
        return;

    CreateCalendarDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted)
    {
        CalendarManager calendarManager;
        calendarManager.createCalendar(dialog.getName(), userId, dialog.getType());
        refresh();
    }
}

void CalendarListWidget::onShareClicked()
{
    QListWidgetItem *item = listWidget->currentItem();
    if (!item)
        return;

    const int calendarId = item->data(Qt::UserRole).toInt();
    CalendarManager calendarManager;
    if (calendarManager.getUserRoleInCalendar(calendarId, userId) != "owner")
        return;

    ShareCalendarDialog dialog(calendarId, userId, this);
    dialog.exec();
}

void CalendarListWidget::onDeleteLeaveClicked()
{
    QListWidgetItem *item = listWidget->currentItem();
    if (!item)
        return;

    const int calendarId = item->data(Qt::UserRole).toInt();
    const QString calendarName = item->text();

    CalendarManager calendarManager;
    const QString role = calendarManager.getUserRoleInCalendar(calendarId, userId);
    if (role.isEmpty())
        return;

    bool success = false;
    if (role == "owner")
    {
        QMessageBox messageBox(
            QMessageBox::Question,
            "Delete calendar",
            QString("Delete calendar \"%1\" and all its events?").arg(calendarName),
            QMessageBox::Yes | QMessageBox::No,
            this
        );
        messageBox.setDefaultButton(QMessageBox::No);
        clearMessageBoxButtonIcons(&messageBox);

        const QMessageBox::StandardButton answer = static_cast<QMessageBox::StandardButton>(messageBox.exec());

        if (answer != QMessageBox::Yes)
            return;

        success = calendarManager.deleteCalendar(calendarId, userId);
    }
    else
    {
        QMessageBox messageBox(
            QMessageBox::Question,
            "Leave calendar",
            QString("Leave calendar \"%1\"?").arg(calendarName),
            QMessageBox::Yes | QMessageBox::No,
            this
        );
        messageBox.setDefaultButton(QMessageBox::No);
        clearMessageBoxButtonIcons(&messageBox);

        const QMessageBox::StandardButton answer = static_cast<QMessageBox::StandardButton>(messageBox.exec());

        if (answer != QMessageBox::Yes)
            return;

        success = calendarManager.leaveCalendar(calendarId, userId);
    }

    if (!success)
    {
        QMessageBox messageBox(
            QMessageBox::Warning,
            "Calendar",
            "The calendar operation failed.",
            QMessageBox::Ok,
            this
        );
        clearMessageBoxButtonIcons(&messageBox);
        messageBox.exec();
        return;
    }

    refresh();
    emit calendarSelected(-1);
}

void CalendarListWidget::onItemClicked(QListWidgetItem *item)
{
    int calendarId = item->data(Qt::UserRole).toInt();
    emit calendarSelected(calendarId);
}

void CalendarListWidget::onSelectionChanged()
{
    updateActionButtons();
}

void CalendarListWidget::updateActionButtons()
{
    QListWidgetItem *item = listWidget->currentItem();
    if (!item || userId == -1)
    {
        shareButton->setEnabled(false);
        deleteLeaveButton->setEnabled(false);
        return;
    }

    CalendarManager calendarManager;
    const QString role = calendarManager.getUserRoleInCalendar(item->data(Qt::UserRole).toInt(), userId);

    shareButton->setEnabled(role == "owner");
    deleteLeaveButton->setEnabled(!role.isEmpty());
    deleteLeaveButton->setText(role == "owner" ? "Delete" : "Leave");
}
