#include "ui/CalendarGridWidget.h"
#include "managers/EventManager.h"
#include "managers/CalendarManager.h"
#include "ui/CreateEventDialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QDialog>
#include <QListWidget>
#include <QScrollArea>
#include <QMouseEvent>
#include <QTime>
#include <QMessageBox>
#include <QAbstractButton>
#include <QIcon>

static const QStringList DAY_NAMES = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};

static void showMessageBoxWithoutButtonIcons(QWidget *parent, QMessageBox::Icon icon, const QString &title, const QString &text)
{
    QMessageBox messageBox(icon, title, text, QMessageBox::Ok, parent);
    for (QAbstractButton *button : messageBox.buttons())
        button->setIcon(QIcon());
    messageBox.exec();
}

static bool confirmSaveConflictingEvent(QWidget *parent)
{
    QMessageBox messageBox(
        QMessageBox::Question,
        "Event conflict",
        "This event overlaps with another event in this calendar. Save anyway?",
        QMessageBox::Yes | QMessageBox::No,
        parent);
    messageBox.setDefaultButton(QMessageBox::No);
    for (QAbstractButton *button : messageBox.buttons())
        button->setIcon(QIcon());
    return messageBox.exec() == QMessageBox::Yes;
}

CalendarGridWidget::CalendarGridWidget(QWidget *parent)
    : QWidget(parent), calendarId(-1), userId(-1), 
      currentStartDate(QDate::currentDate().year(), QDate::currentDate().month(), 1),
      currentMode(Month)
{
    setupUi();
}

void CalendarGridWidget::setupUi()
{
    prevButton = new QPushButton("<", this);
    nextButton = new QPushButton(">", this);
    dateRangeLabel = new QLabel(this);
    dateRangeLabel->setAlignment(Qt::AlignCenter);
    dateRangeLabel->setStyleSheet("font-size: 16px; font-weight: bold;");

    viewModeToggle = new QPushButton("Month", this);
    viewModeToggle->setCheckable(true);
    viewModeToggle->setChecked(true);
    viewModeToggle->setMaximumWidth(100);

    addEventButton = new QPushButton("+ New Event", this);
    addEventButton->setEnabled(false);

    prevButton->setFixedWidth(32);
    nextButton->setFixedWidth(32);

    QHBoxLayout *navLayout = new QHBoxLayout();
    navLayout->addWidget(prevButton);
    navLayout->addWidget(dateRangeLabel, 1);
    navLayout->addWidget(nextButton);
    navLayout->addWidget(viewModeToggle);

    gridWidget = new QWidget(this);
    gridLayout = new QGridLayout(gridWidget);
    gridLayout->setSpacing(4);

    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidget(gridWidget);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(navLayout);
    mainLayout->addWidget(addEventButton);
    mainLayout->addWidget(scrollArea, 1);

    connect(prevButton, &QPushButton::clicked, this, &CalendarGridWidget::onPrevClicked);
    connect(nextButton, &QPushButton::clicked, this, &CalendarGridWidget::onNextClicked);
    connect(addEventButton, &QPushButton::clicked, this, &CalendarGridWidget::onAddEventClicked);
    connect(viewModeToggle, &QPushButton::toggled, this, &CalendarGridWidget::onViewModeChanged);

    updateDateLabel();
    rebuildGrid();
}

void CalendarGridWidget::setCalendarId(int id)
{
    calendarId = id;
    updateEventControls();
    loadEvents();
    rebuildGrid();
}

void CalendarGridWidget::setUserId(int id)
{
    userId = id;
    updateEventControls();
}

void CalendarGridWidget::refresh()
{
    loadEvents();
    rebuildGrid();
}

void CalendarGridWidget::loadEvents()
{
    eventsByDay.clear();

    if (calendarId == -1)
        return;

    EventManager eventManager;
    const std::vector<Event> events = eventManager.getEventsForCalendar(calendarId);

    QDate endDate;
    if (currentMode == Month) {
        endDate = currentStartDate.addMonths(1).addDays(-1);
    } else {
        endDate = currentStartDate.addDays(6);
    }

    for (const Event &event : events)
    {
        const QDate date = event.getStartDateTime().date();
        if (date >= currentStartDate && date <= endDate)
        {
            eventsByDay[date].push_back(event);
        }
    }
}

void CalendarGridWidget::rebuildGrid()
{
    QLayoutItem *item;
    while ((item = gridLayout->takeAt(0)) != nullptr)
    {
        if (item->widget()) {
            item->widget()->setParent(nullptr);
            delete item->widget();
        }
        delete item;
    }

    gridWidget->setMinimumHeight(120);
    gridWidget->updateGeometry();

    if (currentMode == Month) {
        rebuildMonthGrid();
    } else {
        rebuildWeekGrid();
    }
}

void CalendarGridWidget::rebuildMonthGrid()
{
    for (int col = 0; col < 7; ++col)
    {
        QLabel *header = new QLabel(DAY_NAMES[col], gridWidget);
        header->setAlignment(Qt::AlignCenter);
        header->setStyleSheet("font-weight: bold; color: gray;");
        header->setFixedHeight(28);
        gridLayout->addWidget(header, 0, col);
    }
    gridLayout->setRowStretch(0, 0);

    const int daysInMonth = currentStartDate.daysInMonth();
    const int firstDayOfWeek = currentStartDate.dayOfWeek() - 1;

    int col = firstDayOfWeek;
    int row = 1;

    for (int day = 1; day <= daysInMonth; ++day)
    {
        const QDate date(currentStartDate.year(), currentStartDate.month(), day);
        QFrame *cell = new QFrame(gridWidget);
        cell->setFrameShape(QFrame::StyledPanel);
        cell->setMinimumWidth(80);
        cell->setMinimumHeight(120);

        const bool isToday = (QDate::currentDate() == date);
        if (isToday)
            cell->setStyleSheet("QFrame { background-color: #c2e7ff; border: 2px solid #1a73e8; border-radius: 4px; }");
        else
            cell->setStyleSheet("QFrame { border: 1px solid #e0e0e0; border-radius: 4px; }");

        QVBoxLayout *cellLayout = new QVBoxLayout(cell);
        cellLayout->setContentsMargins(4, 4, 4, 4);
        cellLayout->setSpacing(2);

        QLabel *dayLabel = new QLabel(QString::number(day), cell);
        dayLabel->setAlignment(Qt::AlignRight);
        if (isToday)
            dayLabel->setStyleSheet("font-weight: bold;");
        dayLabel->setCursor(Qt::PointingHandCursor);
        dayLabel->setProperty("date", date);
        dayLabel->installEventFilter(this);
        cellLayout->addWidget(dayLabel);
        cellLayout->addStretch();

        const auto it = eventsByDay.find(date);
        if (it != eventsByDay.end())
        {
            const int count = static_cast<int>(it->second.size());
            const QString countText = QString("%1 event%2").arg(count).arg(count > 1 ? "s" : "");
            QLabel *countLabel = new QLabel(countText, cell);
            countLabel->setStyleSheet("color: #1a73e8; font-size: 11px;");
            countLabel->setAlignment(Qt::AlignCenter);
            countLabel->setCursor(Qt::PointingHandCursor);
            countLabel->setProperty("date", date);
            countLabel->installEventFilter(this);
            cellLayout->addWidget(countLabel);
        }

        cell->setCursor(Qt::PointingHandCursor);
        cell->setProperty("date", date);
        cell->installEventFilter(this);

        gridLayout->addWidget(cell, row, col);

        col++;
        if (col == 7)
        {
            col = 0;
            row++;
        }
    }

    for (int c = 0; c < 7; ++c)
        gridLayout->setColumnStretch(c, 1);

    for (int r = 1; r <= 6; ++r)
        gridLayout->setRowStretch(r, 1);
}

void CalendarGridWidget::rebuildWeekGrid()
{
    for (int col = 0; col < 7; ++col)
    {
        const QDate date = currentStartDate.addDays(col);
        const bool isToday = (QDate::currentDate() == date);

        QLabel *header = new QLabel(DAY_NAMES[col], gridWidget);
        header->setAlignment(Qt::AlignCenter);
        header->setStyleSheet(isToday ? "font-weight: bold; color: #1a73e8;" : "font-weight: bold; color: gray;");

        QLabel *dayNum = new QLabel(QString::number(date.day()), gridWidget);
        dayNum->setAlignment(Qt::AlignCenter);
        if (isToday)
            dayNum->setStyleSheet("font-weight: bold; background-color: #1a73e8; color: white; border-radius: 12px; min-width: 24px; min-height: 24px;");
        else
            dayNum->setStyleSheet("font-size: 14px;");

        QFrame *headerFrame = new QFrame(gridWidget);
        QVBoxLayout *hLayout = new QVBoxLayout(headerFrame);
        hLayout->addWidget(header);
        hLayout->addWidget(dayNum);
        hLayout->setContentsMargins(0, 5, 0, 5);
        hLayout->setSpacing(0);
        headerFrame->setMaximumHeight(50);
        
        gridLayout->addWidget(headerFrame, 0, col);

        QFrame *cell = new QFrame(gridWidget);
        cell->setFrameShape(QFrame::StyledPanel);
        cell->setMinimumSize(100, 300);
        cell->setStyleSheet("QFrame { border: 1px solid #e0e0e0; border-radius: 4px; background-color: white; }");
        cell->setCursor(Qt::PointingHandCursor);
        cell->setProperty("date", date);
        cell->installEventFilter(this);

        QVBoxLayout *cellLayout = new QVBoxLayout(cell);
        cellLayout->setContentsMargins(2, 2, 2, 2);
        cellLayout->setSpacing(4);

        const auto it = eventsByDay.find(date);
        if (it != eventsByDay.end())
        {
            for (const Event &ev : it->second)
            {
                QPushButton *evBtn = new QPushButton(ev.getStartDateTime().toString("HH:mm") + " " + ev.getTitle(), cell);
                evBtn->setStyleSheet("QPushButton { background-color: #e8f0fe; border: 1px solid #c2e7ff; text-align: left; padding: 2px; font-size: 11px; border-radius: 2px; } "
                                     "QPushButton:hover { background-color: #d2e3fc; }");
                
                connect(evBtn, &QPushButton::clicked, this, [this, date, ev]() {
                    showDayPopup(date, {ev});
                });
                cellLayout->addWidget(evBtn);
            }
        }
        
        cellLayout->addStretch();
        
        gridLayout->addWidget(cell, 1, col);
    }

    for (int c = 0; c < 7; ++c)
        gridLayout->setColumnStretch(c, 1);
    gridLayout->setRowStretch(0, 0);
    gridLayout->setRowStretch(1, 1);
}

bool CalendarGridWidget::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonRelease)
    {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        if (mouseEvent->button() != Qt::LeftButton)
            return QWidget::eventFilter(obj, event);

        QWidget *widget = qobject_cast<QWidget *>(obj);
        if (!widget)
            return QWidget::eventFilter(obj, event);

        const QDate date = widget->property("date").toDate();
        if (!date.isValid())
            return QWidget::eventFilter(obj, event);

        const auto it = eventsByDay.find(date);
        if (it == eventsByDay.end()) {
            createEventForDate(date);
        } else {
            showDayPopup(date, it->second);
        }
        return true;
    }
    return QWidget::eventFilter(obj, event);
}

void CalendarGridWidget::showDayPopup(const QDate &date, const std::vector<Event> &events)
{
    QDialog popup(this);
    popup.setWindowTitle(date.toString("dddd, MMMM d, yyyy"));
    popup.setMinimumWidth(380);

    QLabel *countLabel = new QLabel(&popup);
    QListWidget *listWidget = new QListWidget(&popup);
    populateDayEventsList(listWidget, countLabel, events);

    QPushButton *addButton = new QPushButton("+ Add", &popup);
    addButton->setEnabled(canManageEvents());
    QPushButton *editButton = new QPushButton("Edit", &popup);
    editButton->setEnabled(false);
    QPushButton *deleteButton = new QPushButton("Delete", &popup);
    deleteButton->setEnabled(false);
    QPushButton *closeButton = new QPushButton("Close", &popup);

    connect(listWidget, &QListWidget::itemSelectionChanged, this, [this, editButton, deleteButton, listWidget]() {
        const bool hasSelection = !listWidget->selectedItems().isEmpty();
        const bool canManage = canManageEvents();
        editButton->setEnabled(hasSelection && canManage);
        deleteButton->setEnabled(hasSelection && canManage);
    });

    connect(addButton, &QPushButton::clicked, this, [this, &popup, date]() {
        if (!canManageEvents()) {
            showPermissionDenied();
            return;
        }

        createEventForDate(date);
        popup.accept();
    });

    connect(editButton, &QPushButton::clicked, this, [this, &popup, listWidget, date, events]() {
        QListWidgetItem *selectedItem = listWidget->currentItem();
        if (!selectedItem) return;

        int eventId = selectedItem->data(Qt::UserRole).toInt();
        const Event *selectedEvent = nullptr;
        for (const Event &e : events) {
            if (e.getId() == eventId) {
                selectedEvent = &e;
                break;
            }
        }
        if (!selectedEvent) return;

        editEvent(*selectedEvent);
        popup.accept();
    });

    connect(deleteButton, &QPushButton::clicked, this, [this, listWidget, countLabel, date]() {
        if (!canManageEvents()) {
            showPermissionDenied();
            return;
        }

        QListWidgetItem *selectedItem = listWidget->currentItem();
        if (!selectedItem) return;

        int eventId = selectedItem->data(Qt::UserRole).toInt();
        
        EventManager eventManager;
        if (eventManager.deleteEvent(eventId)) {
            refresh();
            const auto it = eventsByDay.find(date);
            populateDayEventsList(listWidget,
                                  countLabel,
                                  it == eventsByDay.end() ? std::vector<Event>() : it->second);
        }
    });

    connect(closeButton, &QPushButton::clicked, &popup, &QDialog::accept);

    QVBoxLayout *layout = new QVBoxLayout(&popup);
    layout->addWidget(countLabel);
    layout->addWidget(listWidget);

    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->addWidget(addButton);
    btnLayout->addWidget(editButton);
    btnLayout->addWidget(deleteButton);
    btnLayout->addStretch();
    btnLayout->addWidget(closeButton);
    layout->addLayout(btnLayout);

    popup.exec();
}

void CalendarGridWidget::populateDayEventsList(QListWidget *listWidget, QLabel *countLabel, const std::vector<Event> &events)
{
    listWidget->clear();
    countLabel->setText(QString("%1 event(s)").arg(events.size()));

    if (events.empty())
    {
        QListWidgetItem *item = new QListWidgetItem("No events", listWidget);
        item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
        return;
    }

    for (const Event &event : events)
    {
        const QString time = event.getStartDateTime().toString("HH:mm")
                           + " – "
                           + event.getEndDateTime().toString("HH:mm");

        const QString text = QString("%1\n%2").arg(event.getTitle(), time);
        QListWidgetItem *item = new QListWidgetItem(text, listWidget);
        item->setData(Qt::UserRole, event.getId());
        item->setToolTip(event.getDescription().isEmpty()
                         ? event.getTitle()
                         : event.getDescription());
    }
}

void CalendarGridWidget::editEvent(const Event &event)
{
    if (!canManageEvents())
    {
        showPermissionDenied();
        return;
    }

    CreateEventDialog dialog(this);
    dialog.setWindowTitle("Edit Event");
    dialog.setTitle(event.getTitle());
    dialog.setDescription(event.getDescription());
    dialog.setLocation(event.getLocation());
    dialog.setStartDateTime(event.getStartDateTime());
    dialog.setEndDateTime(event.getEndDateTime());

    if (dialog.exec() == QDialog::Accepted) {
        EventManager eventManager;
        Event updatedEvent(event.getId(), event.getCalendarId(), dialog.getTitle(),
                          dialog.getDescription(), dialog.getLocation(),
                          dialog.getStartDateTime(), dialog.getEndDateTime(),
                          event.getCreatedBy());

        if (eventManager.hasConflict(updatedEvent.getCalendarId(),
                                     updatedEvent.getStartDateTime(),
                                     updatedEvent.getEndDateTime(),
                                     updatedEvent.getId())
            && !confirmSaveConflictingEvent(this))
            return;

        if (eventManager.updateEvent(updatedEvent)) {
            refresh();
        }
        else
        {
            showMessageBoxWithoutButtonIcons(this, QMessageBox::Warning, "Event", "The event could not be saved.");
        }
    }
}

void CalendarGridWidget::createEventForDate(const QDate &date)
{
    if (calendarId <= 0)
    {
        showMessageBoxWithoutButtonIcons(this, QMessageBox::Warning, "Event", "Select a calendar before creating an event.");
        return;
    }

    if (!canManageEvents())
    {
        showPermissionDenied();
        return;
    }

    CreateEventDialog dialog(this);
    const QDateTime start(date, QTime(QTime::currentTime().hour(), 0));
    dialog.setStartDateTime(start);
    dialog.setEndDateTime(start.addSecs(3600));

    if (dialog.exec() == QDialog::Accepted)
    {
        EventManager eventManager;
        Event newEvent(
            -1,
            calendarId,
            dialog.getTitle(),
            dialog.getDescription(),
            dialog.getLocation(),
            dialog.getStartDateTime(),
            dialog.getEndDateTime(),
            userId
        );

        if (eventManager.hasConflict(newEvent.getCalendarId(),
                                     newEvent.getStartDateTime(),
                                     newEvent.getEndDateTime())
            && !confirmSaveConflictingEvent(this))
            return;

        if (eventManager.createEvent(newEvent))
        {
            refresh();
        }
        else
        {
            showMessageBoxWithoutButtonIcons(this, QMessageBox::Warning, "Event", "The event could not be saved.");
        }
    }
}

bool CalendarGridWidget::canManageEvents() const
{
    if (calendarId <= 0 || userId <= 0)
        return false;

    CalendarManager calendarManager;
    const QString role = calendarManager.getUserRoleInCalendar(calendarId, userId);
    return role == "owner" || role == "editor";
}

void CalendarGridWidget::showPermissionDenied()
{
    showMessageBoxWithoutButtonIcons(this,
                                     QMessageBox::Information,
                                     "Permission denied",
                                     "You do not have permission to manage events in this calendar.");
}

void CalendarGridWidget::updateEventControls()
{
    addEventButton->setEnabled(canManageEvents());
}

void CalendarGridWidget::onPrevClicked()
{
    if (currentMode == Month)
        currentStartDate = currentStartDate.addMonths(-1);
    else
        currentStartDate = currentStartDate.addDays(-7);

    updateDateLabel();
    loadEvents();
    rebuildGrid();
}

void CalendarGridWidget::onNextClicked()
{
    if (currentMode == Month)
        currentStartDate = currentStartDate.addMonths(1);
    else
        currentStartDate = currentStartDate.addDays(7);

    updateDateLabel();
    loadEvents();
    rebuildGrid();
}

void CalendarGridWidget::onViewModeChanged(bool checked)
{
    currentMode = checked ? Month : Week;
    viewModeToggle->setText(currentMode == Month ? "Month" : "Week");
    
    if (currentMode == Month) {
        QDate focusDate = currentStartDate.addDays(3);
        currentStartDate = QDate(focusDate.year(), focusDate.month(), 1);
    } else {
        QDate today = QDate::currentDate();
        int daysToMonday = today.dayOfWeek() - 1;
        currentStartDate = today.addDays(-daysToMonday);
    }

    updateDateLabel();
    loadEvents();
    rebuildGrid();
}

void CalendarGridWidget::updateDateLabel()
{
    if (currentMode == Month) {
        dateRangeLabel->setText(currentStartDate.toString("MMMM yyyy"));
    } else {
        QDate endDate = currentStartDate.addDays(6);
        if (currentStartDate.month() == endDate.month()) {
            dateRangeLabel->setText(QString("%1 – %2 %3")
                .arg(currentStartDate.day())
                .arg(endDate.day())
                .arg(currentStartDate.toString("MMMM yyyy")));
        } else {
            dateRangeLabel->setText(QString("%1 – %2")
                .arg(currentStartDate.toString("MMM d"))
                .arg(endDate.toString("MMM d, yyyy")));
        }
    }
}

void CalendarGridWidget::onAddEventClicked()
{
    const QDate today = QDate::currentDate();
    QDate date = currentStartDate;

    if (currentMode == Month) {
        if (today.year() == currentStartDate.year() && today.month() == currentStartDate.month())
            date = today;
    } else if (today >= currentStartDate && today <= currentStartDate.addDays(6)) {
        date = today;
    }

    createEventForDate(date);
}
