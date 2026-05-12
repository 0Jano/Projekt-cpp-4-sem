#ifndef CALENDARGRIDWIDGET_H
#define CALENDARGRIDWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>
#include <QDate>
#include <vector>
#include <map>
#include "models/Event.h"

class QListWidget;
class QFrame;

class CalendarGridWidget : public QWidget
{
    Q_OBJECT

public:
    enum ViewMode { Month, Week };
    explicit CalendarGridWidget(QWidget *parent = nullptr);

    void setCalendarId(int calendarId);
    void refresh();
    void setUserId(int userId);

private slots:
    void onPrevClicked();
    void onNextClicked();
    void onAddEventClicked();
    void onImportIcsClicked();
    void onExportIcsClicked();
    void onViewModeChanged(bool checked);

private:
    int calendarId;
    QDate currentStartDate;
    ViewMode currentMode;

    int userId;

    QLabel *dateRangeLabel;
    QPushButton *prevButton;
    QPushButton *nextButton;
    QPushButton *addEventButton;
    QPushButton *importIcsButton;
    QPushButton *exportIcsButton;
    QPushButton *viewModeToggle;
    
    QGridLayout *gridLayout;
    QWidget *gridWidget;

    std::map<QDate, std::vector<Event>> eventsByDay;

    void setupUi();
    void loadEvents();
    void rebuildGrid();
    void rebuildMonthGrid();
    QFrame *makeOverflowCell(const QDate &date);
    void rebuildWeekGrid();
    void showDayPopup(const QDate &date, const std::vector<Event> &events);
    void populateDayEventsList(QListWidget *listWidget, QLabel *countLabel, const std::vector<Event> &events);
    void editEvent(const Event &event);
    void createEventForDate(const QDate &date);
    bool canManageEvents() const;
    void showPermissionDenied();
    void updateEventControls();
    void updateDateLabel();
    bool eventFilter(QObject *obj, QEvent *event) override;
};

#endif
