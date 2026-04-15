#include "ui/CalendarGridWidget.h"
#include "managers/EventManager.h"
#include "ui/CreateEventDialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QDialog>
#include <QListWidget>
#include <QScrollArea>
#include <QMouseEvent>
#include <QApplication>

static const QStringList DAY_NAMES = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};

CalendarGridWidget::CalendarGridWidget(QWidget *parent)
    : QWidget(parent), calendarId(-1), userId(-1), currentMonth(QDate::currentDate().year(), QDate::currentDate().month(), 1)
{
    setupUi();
}

void CalendarGridWidget::setupUi()
{
    prevButton = new QPushButton("<", this);
    nextButton = new QPushButton(">", this);
    monthLabel = new QLabel(this);
    monthLabel->setAlignment(Qt::AlignCenter);

    addEventButton = new QPushButton("+ New Event", this);
    addEventButton->setEnabled(false);

    prevButton->setFixedWidth(32);
    nextButton->setFixedWidth(32);

    QHBoxLayout *navLayout = new QHBoxLayout();
    navLayout->addWidget(prevButton);
    navLayout->addWidget(monthLabel, 1);
    navLayout->addWidget(nextButton);

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
    mainLayout->addWidget(scrollArea);

    connect(prevButton, &QPushButton::clicked, this, &CalendarGridWidget::onPrevMonthClicked);
    connect(nextButton, &QPushButton::clicked, this, &CalendarGridWidget::onNextMonthClicked);
    connect(addEventButton, &QPushButton::clicked, this, &CalendarGridWidget::onAddEventClicked);

    updateMonthLabel();
    rebuildGrid();
}

void CalendarGridWidget::setCalendarId(int id)
{
    calendarId = id;
    addEventButton->setEnabled(calendarId != -1);
    loadEvents();
    rebuildGrid();
}

void CalendarGridWidget::setUserId(int id)
{
    userId = id;
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

    for (const Event &event : events)
    {
        const QDate date = event.getStartDateTime().date();
        if (date.year() == currentMonth.year() && date.month() == currentMonth.month())
        {
            eventsByDay[date.day()].push_back(event);
        }
    }
}

void CalendarGridWidget::rebuildGrid()
{
    QLayoutItem *item;
    while ((item = gridLayout->takeAt(0)) != nullptr)
    {
        delete item->widget();
        delete item;
    }

    for (int col = 0; col < 7; ++col)
    {
        QLabel *header = new QLabel(DAY_NAMES[col], gridWidget);
        header->setAlignment(Qt::AlignCenter);
        header->setStyleSheet("font-weight: bold; color: gray;");
        gridLayout->addWidget(header, 0, col);
    }

    const int daysInMonth = currentMonth.daysInMonth();
    const int firstDayOfWeek = currentMonth.dayOfWeek() - 1;

    int col = firstDayOfWeek;
    int row = 1;

    for (int day = 1; day <= daysInMonth; ++day)
    {
        QFrame *cell = new QFrame(gridWidget);
        cell->setFrameShape(QFrame::StyledPanel);
        cell->setMinimumSize(80, 70);

        const bool isToday = (QDate::currentDate() == QDate(currentMonth.year(), currentMonth.month(), day));
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
        cellLayout->addWidget(dayLabel);
        cellLayout->addStretch();

        const auto it = eventsByDay.find(day);
        if (it != eventsByDay.end())
        {
            const int count = static_cast<int>(it->second.size());
            const QString countText = QString("%1 event%2").arg(count).arg(count > 1 ? "s" : "");
            QLabel *countLabel = new QLabel(countText, cell);
            countLabel->setStyleSheet("color: #1a73e8; font-size: 11px;");
            countLabel->setAlignment(Qt::AlignCenter);
            cellLayout->addWidget(countLabel);

            const std::vector<Event> &dayEvents = it->second;
            const QDate date(currentMonth.year(), currentMonth.month(), day);

            cell->setCursor(Qt::PointingHandCursor);
            cell->installEventFilter(this);
            cell->setProperty("clickable", true);
            cell->setProperty("date", date);
            cell->setProperty("eventCount", count);

            connect(countLabel, &QLabel::linkActivated, this, [=]() {
                showDayPopup(date, dayEvents);
            });

            cell->setProperty("hasEvents", true);
            QPushButton *clickOverlay = new QPushButton(cell);
            clickOverlay->setFlat(true);
            clickOverlay->setStyleSheet("QPushButton { background: transparent; border: none; }");
            clickOverlay->setGeometry(0, 0, cell->width(), cell->height());
            clickOverlay->raise();

            connect(clickOverlay, &QPushButton::clicked, this, [this, date, dayEvents]() {
                showDayPopup(date, dayEvents);
            });

            cell->installEventFilter(this);
            clickOverlay->setObjectName("overlay");
            cell->setProperty("overlaySet", true);
        }

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
}

bool CalendarGridWidget::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::Resize)
    {
        QWidget *cell = qobject_cast<QWidget *>(obj);
        if (cell)
        {
            QPushButton *overlay = cell->findChild<QPushButton *>("overlay");
            if (overlay)
                overlay->setGeometry(0, 0, cell->width(), cell->height());
        }
    }
    return QWidget::eventFilter(obj, event);
}

void CalendarGridWidget::showDayPopup(const QDate &date, const std::vector<Event> &events)
{
    QDialog popup(this);
    popup.setWindowTitle(date.toString("dddd, MMMM d, yyyy"));
    popup.setMinimumWidth(320);

    QListWidget *listWidget = new QListWidget(&popup);

    for (const Event &event : events)
    {
        const QString time = event.getStartDateTime().toString("HH:mm")
                           + " – "
                           + event.getEndDateTime().toString("HH:mm");

        const QString text = QString("%1\n%2").arg(event.getTitle(), time);
        QListWidgetItem *item = new QListWidgetItem(text, listWidget);
        item->setToolTip(event.getDescription().isEmpty()
                         ? event.getTitle()
                         : event.getDescription());
    }

    QVBoxLayout *layout = new QVBoxLayout(&popup);
    layout->addWidget(new QLabel(QString("%1 event(s)").arg(events.size()), &popup));
    layout->addWidget(listWidget);

    QPushButton *closeButton = new QPushButton("Close", &popup);
    connect(closeButton, &QPushButton::clicked, &popup, &QDialog::accept);

    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->addStretch();
    btnLayout->addWidget(closeButton);
    layout->addLayout(btnLayout);

    popup.exec();
}

void CalendarGridWidget::onPrevMonthClicked()
{
    currentMonth = currentMonth.addMonths(-1);
    updateMonthLabel();
    loadEvents();
    rebuildGrid();
}

void CalendarGridWidget::onNextMonthClicked()
{
    currentMonth = currentMonth.addMonths(1);
    updateMonthLabel();
    loadEvents();
    rebuildGrid();
}

void CalendarGridWidget::updateMonthLabel()
{
    monthLabel->setText(currentMonth.toString("MMMM yyyy"));
}

void CalendarGridWidget::onAddEventClicked()
{
    CreateEventDialog dialog(this);
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

        if (eventManager.createEvent(newEvent))
        {
            loadEvents();
            rebuildGrid();
        }
    }
}
