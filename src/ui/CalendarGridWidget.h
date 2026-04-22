#ifndef CALENDARGRIDWIDGET_H
#define CALENDARGRIDWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>
#include <QDate>
#include <QComboBox>
#include <vector>
#include <map>
#include "models/Event.h"

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
    void onViewModeChanged(int index);

private:
    int calendarId;
    QDate currentStartDate; // Points to the first day of the month or the first day of the week
    ViewMode currentMode;

    int userId;

    QLabel *dateRangeLabel;
    QPushButton *prevButton;
    QPushButton *nextButton;
    QPushButton *addEventButton;
    QComboBox *viewModeCombo;
    
    QGridLayout *gridLayout;
    QWidget *gridWidget;

    std::map<QDate, std::vector<Event>> eventsByDay; // date -> events

    void setupUi();
    void loadEvents();
    void rebuildGrid();
    void rebuildMonthGrid();
    void rebuildWeekGrid();
    void showDayPopup(const QDate &date, const std::vector<Event> &events);
    void updateDateLabel();
    bool eventFilter(QObject *obj, QEvent *event) override;
};

#endif
