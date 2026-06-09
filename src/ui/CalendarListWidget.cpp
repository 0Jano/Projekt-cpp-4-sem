#include "ui/CalendarListWidget.h"
#include "ui/CreateCalendarDialog.h"
#include "ui/ShareCalendarDialog.h"
#include "managers/CalendarManager.h"

#include <QAbstractButton>
#include <QEvent>
#include <QFrame>
#include <QHBoxLayout>
#include <QIcon>
#include <QMessageBox>
#include <QSizePolicy>
#include <QStringList>

static QString calendarColor(int calendarId)
{
    const QStringList colors = {"#185FA5", "#1D9E75", "#D85A30", "#7F77DD"};
    return colors[calendarId % colors.size()];
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
    QWidget *scrollContent = new QWidget(this);
    scrollContent->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    QVBoxLayout *scrollLayout = new QVBoxLayout(scrollContent);
    scrollLayout->setContentsMargins(8, 8, 8, 8);
    scrollLayout->setSpacing(0);

    myCalendarsHeader = new QLabel("MY CALENDARS", scrollContent);
    myCalendarsHeader->setStyleSheet(
        "font-size: 11px; font-weight: 600; color: #888888; padding: 4px 4px 6px 4px;"
    );
    scrollLayout->addWidget(myCalendarsHeader);

    myCalendarsLayout = new QVBoxLayout();
    myCalendarsLayout->setSpacing(4);
    myCalendarsLayout->setContentsMargins(0, 0, 0, 0);
    scrollLayout->addLayout(myCalendarsLayout);

    sharedCalendarsHeader = new QLabel("SHARED", scrollContent);
    sharedCalendarsHeader->setStyleSheet(
        "font-size: 11px; font-weight: 600; color: #888888; padding: 12px 4px 6px 4px;"
    );
    scrollLayout->addWidget(sharedCalendarsHeader);

    sharedCalendarsLayout = new QVBoxLayout();
    sharedCalendarsLayout->setSpacing(4);
    sharedCalendarsLayout->setContentsMargins(0, 0, 0, 0);
    scrollLayout->addLayout(sharedCalendarsLayout);
    scrollLayout->addStretch();

    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidget(scrollContent);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setStyleSheet("QScrollArea { background: transparent; }");

    addButton = new QPushButton("+ New Calendar", this);
    addButton->setStyleSheet(
        "QPushButton {"
        "  font-size: 12px; color: #185FA5; background: transparent;"
        "  border: 1px solid #185FA5; padding: 6px 4px; text-align: left;"
        "}"
        "QPushButton:hover { color: #0C447C; border-color: #0C447C; }"
    );

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 8);
    mainLayout->setSpacing(0);
    mainLayout->addWidget(scrollArea, 1);
    mainLayout->addWidget(addButton);

    connect(addButton, &QPushButton::clicked, this, &CalendarListWidget::onAddClicked);
}

void CalendarListWidget::clearLayout(QVBoxLayout *layout)
{
    while (QLayoutItem *item = layout->takeAt(0))
    {
        if (QWidget *widget = item->widget())
            delete widget;

        delete item;
    }
}

QWidget* CalendarListWidget::createCalendarCard(const Calendar &calendar)
{
    CalendarManager calendarManager;
    const int calId = calendar.getId();
    const QString role = calendarManager.getUserRoleInCalendar(calId, userId);
    const bool isOwner = role == "owner";

    QFrame *card = new QFrame(this);
    card->setFrameShape(QFrame::NoFrame);
    card->setProperty("calendarId", calId);
    card->setCursor(Qt::PointingHandCursor);
    card->installEventFilter(this);

    QLabel *dot = new QLabel(card);
    dot->setFixedSize(10, 10);
    dot->setAttribute(Qt::WA_TransparentForMouseEvents);
    dot->setStyleSheet(QString("background: %1; border-radius: 5px; border: none;").arg(calendarColor(calId)));

    QLabel *nameLabel = new QLabel(calendar.getName(), card);
    nameLabel->setWordWrap(true);
    nameLabel->setAttribute(Qt::WA_TransparentForMouseEvents);
    nameLabel->setStyleSheet("font-size: 13px; color: #202124; border: none; background: transparent;");
    nameLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    QHBoxLayout *nameRow = new QHBoxLayout();
    nameRow->setSpacing(8);
    nameRow->setContentsMargins(0, 0, 0, 0);
    nameRow->addWidget(dot);
    nameRow->addWidget(nameLabel, 1);

    auto makeIconButton = [](const QString &icon,
                             const QString &tooltip,
                             const QString &textColor,
                             const QString &hoverBg,
                             QWidget *parent) {
        QPushButton *button = new QPushButton(icon, parent);
        button->setFixedSize(28, 28);
        button->setToolTip(tooltip);
        button->setStyleSheet(QString(
            "QPushButton {"
            "  font-size: 15px; padding: 0;"
            "  border: 1px solid #DADADA; border-radius: 5px;"
            "  background: white; color: %1;"
            "}"
            "QPushButton:hover { background: %2; border-color: %1; }"
            "QPushButton:disabled { color: #B0B0B0; background: #F7F7F7; }"
        ).arg(textColor, hoverBg));
        return button;
    };

    QPushButton *shareButton = makeIconButton("↗", "Udostępnij", "#185FA5", "#E6F1FB", card);
    QPushButton *deleteLeaveButton = makeIconButton("✕", isOwner ? "Usuń" : "Opuść", "#993C1D", "#FAECE7", card);
    shareButton->setEnabled(isOwner);
    deleteLeaveButton->setEnabled(!role.isEmpty());

    QPushButton *favoriteButton = new QPushButton("★", card);
    favoriteButton->setCheckable(true);
    favoriteButton->setChecked(calId == favoriteCalendarId);
    favoriteButton->setFixedSize(28, 28);
    favoriteButton->setToolTip("Dodaj do ulubionych");
    favoriteButton->setStyleSheet(
        "QPushButton {"
        "  font-size: 14px; padding: 0;"
        "  border: 1px solid #DADADA; border-radius: 5px;"
        "  background: white; color: #BDBDBD;"
        "}"
        "QPushButton:hover { color: #F0A500; border-color: #F0A500; }"
        "QPushButton:checked { color: #F0A500; background: #FFF8E7; border-color: #F0A500; }"
    );

    QHBoxLayout *buttonRow = new QHBoxLayout();
    buttonRow->setSpacing(6);
    buttonRow->setContentsMargins(0, 0, 0, 0);
    buttonRow->addWidget(shareButton);
    buttonRow->addWidget(deleteLeaveButton);
    buttonRow->addWidget(favoriteButton);
    buttonRow->addStretch();

    QVBoxLayout *cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(10, 8, 10, 8);
    cardLayout->setSpacing(6);
    cardLayout->addLayout(nameRow);
    cardLayout->addLayout(buttonRow);

    const bool selected = calId == selectedCalendarId;
    card->setStyleSheet(selected
        ? "QFrame { background: #E6F1FB; border-radius: 8px; border: 1px solid #DDE3F0; }"
          "QFrame:hover { background: #E6F1FB; }"
        : "QFrame { background: #F0F4FF; border-radius: 8px; border: 1px solid #DDE3F0; }"
          "QFrame:hover { background: #E6EEFA; }");

    connect(shareButton, &QPushButton::clicked, this, [this, calId]() {
        CalendarManager calendarManager;
        if (calendarManager.getUserRoleInCalendar(calId, userId) != "owner")
            return;

        ShareCalendarDialog dialog(calId, userId, this);
        dialog.exec();
    });

    connect(deleteLeaveButton, &QPushButton::clicked, this, [this, calId, calendarName = calendar.getName()]() {
        CalendarManager calendarManager;
        const QString role = calendarManager.getUserRoleInCalendar(calId, userId);
        if (role.isEmpty())
            return;

        const bool isOwner = role == "owner";
        QMessageBox messageBox(
            QMessageBox::Question,
            isOwner ? "Delete calendar" : "Leave calendar",
            isOwner
                ? QString("Delete calendar \"%1\" and all its events?").arg(calendarName)
                : QString("Leave calendar \"%1\"?").arg(calendarName),
            QMessageBox::Yes | QMessageBox::No,
            this
        );
        messageBox.setDefaultButton(QMessageBox::No);
        clearMessageBoxButtonIcons(&messageBox);

        const QMessageBox::StandardButton answer = static_cast<QMessageBox::StandardButton>(messageBox.exec());
        if (answer != QMessageBox::Yes)
            return;

        const bool success = isOwner
            ? calendarManager.deleteCalendar(calId, userId)
            : calendarManager.leaveCalendar(calId, userId);

        if (!success)
        {
            QMessageBox warningBox(
                QMessageBox::Warning,
                "Calendar",
                "The calendar operation failed.",
                QMessageBox::Ok,
                this
            );
            clearMessageBoxButtonIcons(&warningBox);
            warningBox.exec();
            return;
        }

        const bool removedSelectedCalendar = selectedCalendarId == calId;
        if (removedSelectedCalendar)
            selectedCalendarId = -1;

        refresh();
        if (removedSelectedCalendar)
            emit calendarSelected(-1);
    });

    connect(favoriteButton, &QPushButton::clicked, this, [this, calId]() {
        CalendarManager calendarManager;

        if (calId == calendarManager.getFavoriteCalendarId(userId))
        {
            if (!calendarManager.clearFavoriteCalendar(userId))
                return;

            favoriteCalendarId = -1;
            refresh();
            selectCalendar(calId, true);
            return;
        }

        if (!calendarManager.setFavoriteCalendar(userId, calId))
            return;

        favoriteCalendarId = calId;
        selectedCalendarId = calId;
        refresh();
        selectCalendar(calId, true);
    });

    return card;
}

void CalendarListWidget::refresh()
{
    clearLayout(myCalendarsLayout);
    clearLayout(sharedCalendarsLayout);
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

    for (const Calendar &calendar : calendars)
    {
        QWidget *card = createCalendarCard(calendar);
        if (calendar.getOwnerId() == userId)
            myCalendarsLayout->addWidget(card);
        else
            sharedCalendarsLayout->addWidget(card);
    }

    selectInitialCalendar();
}

void CalendarListWidget::setUserId(int id)
{
    userId = id;
    selectedCalendarId = -1;
    favoriteCalendarId = -1;
    refresh();
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

bool CalendarListWidget::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonRelease)
    {
        QWidget *widget = qobject_cast<QWidget *>(obj);
        if (widget)
        {
            const QVariant calendarId = widget->property("calendarId");
            if (calendarId.isValid())
            {
                selectCalendar(calendarId.toInt(), true);
                return true;
            }
        }
    }

    return QWidget::eventFilter(obj, event);
}

void CalendarListWidget::selectCalendar(int calendarId, bool emitSelection)
{
    selectedCalendarId = calendarId;
    refresh();

    if (emitSelection)
        emit calendarSelected(calendarId);
}

void CalendarListWidget::selectInitialCalendar()
{
    for (const Calendar &calendar : calendars)
    {
        if (calendar.getId() == selectedCalendarId)
            return;
    }

    selectedCalendarId = -1;

    for (const Calendar &calendar : calendars)
    {
        if (calendar.getId() == favoriteCalendarId)
        {
            selectedCalendarId = favoriteCalendarId;
            emit calendarSelected(selectedCalendarId);
            return;
        }
    }

    if (!calendars.empty())
    {
        selectedCalendarId = calendars.front().getId();
        emit calendarSelected(selectedCalendarId);
    }
}
