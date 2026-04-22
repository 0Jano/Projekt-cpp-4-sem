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
#include <QComboBox>

static const QStringList DAY_NAMES = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};

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

    viewModeCombo = new QComboBox(this);
    viewModeCombo->addItem("Month", Month);
    viewModeCombo->addItem("Week", Week);

    addEventButton = new QPushButton("+ New Event", this);
    addEventButton->setEnabled(false);

    prevButton->setFixedWidth(32);
    nextButton->setFixedWidth(32);

    QHBoxLayout *navLayout = new QHBoxLayout();
    navLayout->addWidget(prevButton);
    navLayout->addWidget(dateRangeLabel, 1);
    navLayout->addWidget(nextButton);
    navLayout->addWidget(viewModeCombo);

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

    connect(prevButton, &QPushButton::clicked, this, &CalendarGridWidget::onPrevClicked);
    connect(nextButton, &QPushButton::clicked, this, &CalendarGridWidget::onNextClicked);
    connect(addEventButton, &QPushButton::clicked, this, &CalendarGridWidget::onAddEventClicked);
    connect(viewModeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &CalendarGridWidget::onViewModeChanged);

    updateDateLabel();
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
        gridLayout->addWidget(header, 0, col);
    }

    const int daysInMonth = currentStartDate.daysInMonth();
    const int firstDayOfWeek = currentStartDate.dayOfWeek() - 1;

    int col = firstDayOfWeek;
    int row = 1;

    for (int day = 1; day <= daysInMonth; ++day)
    {
        const QDate date(currentStartDate.year(), currentStartDate.month(), day);
        QFrame *cell = new QFrame(gridWidget);
        cell->setFrameShape(QFrame::StyledPanel);
        cell->setMinimumSize(80, 70);

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
            cellLayout->addWidget(countLabel);

            const std::vector<Event> &dayEvents = it->second;

            cell->setCursor(Qt::PointingHandCursor);
            cell->setProperty("clickable", true);
            
            QPushButton *clickOverlay = new QPushButton(cell);
            clickOverlay->setFlat(true);
            clickOverlay->setStyleSheet("QPushButton { background: transparent; border: none; }");
            clickOverlay->setObjectName("overlay");
            clickOverlay->setGeometry(0, 0, cell->width(), cell->height());
            
            connect(clickOverlay, &QPushButton::clicked, this, [this, date, dayEvents]() {
                showDayPopup(date, dayEvents);
            });
            cell->installEventFilter(this);
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

void CalendarGridWidget::rebuildWeekGrid()
{
    for (int col = 0; col < 7; ++col)
    {
        const QDate date = currentStartDate.addDays(col);
        const bool isToday = (QDate::currentDate() == date);

        QVBoxLayout *colLayout = new QVBoxLayout();
        
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
        
        gridLayout->addWidget(headerFrame, 0, col);

        QFrame *cell = new QFrame(gridWidget);
        cell->setFrameShape(QFrame::StyledPanel);
        cell->setMinimumSize(100, 300);
        cell->setStyleSheet("QFrame { border: 1px solid #e0e0e0; border-radius: 4px; background-color: white; }");

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
    gridLayout->setRowStretch(1, 1);
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

void CalendarGridWidget::onViewModeChanged(int index)
{
    currentMode = static_cast<ViewMode>(viewModeCombo->itemData(index).toInt());
    
    // Adjust currentStartDate to a sensible start for the mode
    if (currentMode == Month) {
        currentStartDate = QDate(currentStartDate.year(), currentStartDate.month(), 1);
    } else {
        // Find Monday of the current week
        int daysToMonday = currentStartDate.dayOfWeek() - 1;
        currentStartDate = currentStartDate.addDays(-daysToMonday);
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
