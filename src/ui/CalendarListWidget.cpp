#include "ui/CalendarListWidget.h"
#include "ui/CreateCalendarDialog.h"
#include "ui/ShareCalendarDialog.h"
#include "managers/CalendarManager.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidgetItem>

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

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(shareButton);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(listWidget);
    mainLayout->addLayout(buttonLayout);

    setFixedWidth(220);

    connect(addButton, &QPushButton::clicked, this, &CalendarListWidget::onAddClicked);
    connect(shareButton, &QPushButton::clicked, this, &CalendarListWidget::onShareClicked);
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
    ShareCalendarDialog dialog(calendarId, userId, this);
    dialog.exec();
}

void CalendarListWidget::onItemClicked(QListWidgetItem *item)
{
    int calendarId = item->data(Qt::UserRole).toInt();
    emit calendarSelected(calendarId);
}

void CalendarListWidget::onSelectionChanged()
{
    shareButton->setEnabled(listWidget->currentItem() != nullptr);
}
