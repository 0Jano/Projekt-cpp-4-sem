#include "ui/CalendarListWidget.h"
#include "ui/CreateCalendarDialog.h"
#include "ui/ShareCalendarDialog.h"
#include "managers/CalendarManager.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QAbstractButton>
#include <QAbstractItemView>
#include <QIcon>
#include <QMouseEvent>
#include <QSizePolicy>
#include <QFrame>
#include <QColor>
#include <QFont>

static void setCalendarCardActionButtonSize(QPushButton *button)
{
    button->setFixedSize(24, 24);
    button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

static QString calendarColor(int calendarId)
{
    switch (calendarId % 4)
    {
    case 1:
        return "#0F6E56";
    case 2:
        return "#993C1D";
    case 3:
        return "#3C3489";
    default:
        return "#185FA5";
    }
}

CalendarCardWidget::CalendarCardWidget(int calendarId,
                                       const QString &name,
                                       const QString &role,
                                       bool selected,
                                       bool favorite,
                                       QWidget *parent)
    : QWidget(parent), calendarId(calendarId)
{
    colorDot = new QLabel(this);
    nameLabel = new QLabel(name, this);
    shareButton = new QPushButton("S", this);
    deleteLeaveButton = new QPushButton(role == "owner" ? "D" : "L", this);
    starButton = new QPushButton(this);

    colorDot->setFixedSize(9, 9);
    colorDot->setStyleSheet(QString("background: %1; border-radius: 4px;").arg(calendarColor(calendarId)));
    nameLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    nameLabel->setStyleSheet("font-size: 13px; color: #202124;");
    nameLabel->setAttribute(Qt::WA_TransparentForMouseEvents);
    setCalendarCardActionButtonSize(shareButton);
    setCalendarCardActionButtonSize(deleteLeaveButton);
    setCalendarCardActionButtonSize(starButton);
    shareButton->setToolTip("Share");
    deleteLeaveButton->setToolTip(role == "owner" ? "Delete" : "Leave");
    starButton->setToolTip("Favorite");
    shareButton->setFlat(true);
    deleteLeaveButton->setFlat(true);
    starButton->setFlat(true);
    shareButton->setEnabled(role == "owner");
    deleteLeaveButton->setEnabled(!role.isEmpty());

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(8, 5, 6, 5);
    layout->setSpacing(6);
    layout->addWidget(colorDot);
    layout->addWidget(nameLabel, 1);
    layout->addWidget(shareButton);
    layout->addWidget(deleteLeaveButton);
    layout->addWidget(starButton);

    setCursor(Qt::PointingHandCursor);
    setFavorite(favorite);
    updateStyle(selected);

    connect(shareButton, &QPushButton::clicked, this, [this]() {
        emit shareRequested(this->calendarId);
    });
    connect(deleteLeaveButton, &QPushButton::clicked, this, [this]() {
        emit deleteLeaveRequested(this->calendarId);
    });
    connect(starButton, &QPushButton::clicked, this, [this]() {
        emit favoriteRequested(this->calendarId);
    });
}

int CalendarCardWidget::getCalendarId() const
{
    return calendarId;
}

void CalendarCardWidget::setSelected(bool selected)
{
    updateStyle(selected);
}

void CalendarCardWidget::setFavorite(bool favorite)
{
    starButton->setText(favorite ? "*" : "☆");
    starButton->setStyleSheet(favorite
        ? "QPushButton { border: none; background: transparent; color: #185FA5; font-weight: bold; padding: 0; }"
        : "QPushButton { border: none; background: transparent; color: #888888; padding: 0; }");
}

void CalendarCardWidget::mousePressEvent(QMouseEvent *event)
{
    emit selected(calendarId);
    event->accept();
}

void CalendarCardWidget::updateStyle(bool selected)
{
    setStyleSheet(selected
        ? "CalendarCardWidget { background: #E6F1FB; border: none; border-radius: 6px; }"
        : "CalendarCardWidget { background: transparent; border: none; border-radius: 6px; }");
}

static void clearMessageBoxButtonIcons(QMessageBox *messageBox)
{
    for (QAbstractButton *button : messageBox->buttons())
        button->setIcon(QIcon());
}

CalendarListWidget::CalendarListWidget(QWidget *parent)
    : QWidget(parent), userId(-1), selectedCalendarId(-1), favoriteCalendarId(-1)
{
    setupUi();
}

void CalendarListWidget::setupUi()
{
    titleLabel = new QLabel("Calendars", this);
    listWidget = new QListWidget(this);
    addButton = new QPushButton("+ New calendar", this);
    listWidget->setSpacing(2);
    listWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    listWidget->setFrameShape(QFrame::NoFrame);
    listWidget->setStyleSheet("QListWidget { border: none; background: transparent; } QListWidget::item { padding: 0; }");
    titleLabel->setVisible(false);
    addButton->setFlat(true);
    addButton->setStyleSheet("QPushButton { border: none; background: transparent; color: #888888; text-align: left; padding: 5px 4px; } QPushButton:hover { background: #F1F3F4; }");

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(addButton);
    buttonLayout->addStretch();

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(listWidget);
    mainLayout->addLayout(buttonLayout);

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    connect(addButton, &QPushButton::clicked, this, &CalendarListWidget::onAddClicked);
    connect(listWidget, &QListWidget::itemClicked, this, &CalendarListWidget::onItemClicked);
}

void CalendarListWidget::setUserId(int id)
{
    userId = id;
    selectedCalendarId = -1;
    favoriteCalendarId = -1;
    refresh();
}

void CalendarListWidget::refresh(bool selectFavorite)
{
    listWidget->clear();
    calendars.clear();
    favoriteCalendarId = -1;

    if (userId == -1)
    {
        selectedCalendarId = -1;
        return;
    }

    CalendarManager calendarManager;
    calendars = calendarManager.getCalendarsForUser(userId);
    favoriteCalendarId = calendarManager.getFavoriteCalendarId(userId);

    auto addSectionHeader = [this](const QString &title) {
        QListWidgetItem *headerItem = new QListWidgetItem(title.toUpper(), listWidget);
        headerItem->setFlags(Qt::ItemIsEnabled);
        headerItem->setSizeHint(QSize(0, 28));
        headerItem->setForeground(QColor("#888888"));
        QFont font = headerItem->font();
        font.setPointSize(8);
        font.setBold(true);
        headerItem->setFont(font);
    };

    auto addCalendarItem = [this, &calendarManager](const Calendar &calendar) {
        const QString role = calendarManager.getUserRoleInCalendar(calendar.getId(), userId);
        QListWidgetItem *item = new QListWidgetItem(listWidget);
        item->setData(Qt::UserRole, calendar.getId());
        item->setSizeHint(QSize(0, 36));

        CalendarCardWidget *card = new CalendarCardWidget(
            calendar.getId(),
            calendar.getName(),
            role,
            calendar.getId() == selectedCalendarId,
            calendar.getId() == favoriteCalendarId,
            listWidget
        );

        connect(card, &CalendarCardWidget::selected, this, [this](int calendarId) {
            selectCalendar(calendarId, true);
        });
        connect(card, &CalendarCardWidget::shareRequested, this, &CalendarListWidget::onShareClicked);
        connect(card, &CalendarCardWidget::deleteLeaveRequested, this, &CalendarListWidget::onDeleteLeaveClicked);
        connect(card, &CalendarCardWidget::favoriteRequested, this, &CalendarListWidget::onStarClicked);

        listWidget->setItemWidget(item, card);
    };

    addSectionHeader("My calendars");
    for (const Calendar &calendar : calendars)
    {
        if (calendar.getOwnerId() == userId)
            addCalendarItem(calendar);
    }

    addSectionHeader("Shared");
    for (const Calendar &calendar : calendars)
    {
        if (calendar.getOwnerId() != userId)
            addCalendarItem(calendar);
    }

    selectInitialCalendar(selectFavorite);
}

void CalendarListWidget::onAddClicked()
{
    if (userId == -1)
        return;

    CreateCalendarDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted)
    {
        CalendarManager calendarManager;
        calendarManager.createCalendar(dialog.getName(), userId);
        refresh();
    }
}

void CalendarListWidget::onShareClicked(int calendarId)
{
    if (userId == -1)
        return;

    CalendarManager calendarManager;
    if (calendarManager.getUserRoleInCalendar(calendarId, userId) != "owner")
        return;

    ShareCalendarDialog dialog(calendarId, userId, this);
    dialog.exec();
}

void CalendarListWidget::onDeleteLeaveClicked(int calendarId)
{
    if (userId == -1)
        return;

    const Calendar calendar = findCalendar(calendarId);
    if (calendar.getId() == -1)
        return;

    const QString calendarName = calendar.getName();

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

    const bool removedSelectedCalendar = selectedCalendarId == calendarId;
    if (removedSelectedCalendar)
        selectedCalendarId = -1;

    refresh(!removedSelectedCalendar);
    if (removedSelectedCalendar)
        emit calendarSelected(-1);
}

void CalendarListWidget::onStarClicked(int calendarId)
{
    if (userId == -1)
        return;

    CalendarManager calendarManager;

    if (calendarId == calendarManager.getFavoriteCalendarId(userId))
    {
        if (!calendarManager.clearFavoriteCalendar(userId))
            return;

        favoriteCalendarId = -1;
        refresh(false);
        selectCalendar(calendarId, true);
        return;
    }

    if (!calendarManager.setFavoriteCalendar(userId, calendarId))
        return;

    favoriteCalendarId = calendarId;
    selectedCalendarId = calendarId;
    refresh();
    selectCalendar(calendarId, true);
}

void CalendarListWidget::onItemClicked(QListWidgetItem *item)
{
    if (!item)
        return;
    if (!listWidget->itemWidget(item))
        return;

    const int calendarId = item->data(Qt::UserRole).toInt();
    selectCalendar(calendarId, true);
}

void CalendarListWidget::selectCalendar(int calendarId, bool emitSelection)
{
    selectedCalendarId = calendarId;

    for (int i = 0; i < listWidget->count(); ++i)
    {
        QListWidgetItem *item = listWidget->item(i);
        CalendarCardWidget *card = qobject_cast<CalendarCardWidget *>(listWidget->itemWidget(item));
        if (!card)
            continue;

        const bool selected = card->getCalendarId() == calendarId;
        card->setSelected(selected);
        if (selected && listWidget->currentItem() != item)
            listWidget->setCurrentItem(item);
    }

    if (emitSelection)
        emit calendarSelected(calendarId);
}

void CalendarListWidget::selectInitialCalendar(bool selectFavorite)
{
    if (containsCalendar(selectedCalendarId))
    {
        selectCalendar(selectedCalendarId, false);
        return;
    }

    selectedCalendarId = -1;
    if (selectFavorite && containsCalendar(favoriteCalendarId))
    {
        selectCalendar(favoriteCalendarId, true);
        return;
    }

    if (selectFavorite && !calendars.empty())
        selectCalendar(calendars.front().getId(), true);
}

bool CalendarListWidget::containsCalendar(int calendarId) const
{
    for (const Calendar &calendar : calendars)
        if (calendar.getId() == calendarId)
            return true;

    return false;
}

Calendar CalendarListWidget::findCalendar(int calendarId) const
{
    for (const Calendar &calendar : calendars)
        if (calendar.getId() == calendarId)
            return calendar;

    return Calendar();
}
