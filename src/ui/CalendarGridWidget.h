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

class CalendarGridWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CalendarGridWidget(QWidget *parent = nullptr);

    void setCalendarId(int calendarId);
    void refresh();

private slots:
    void onPrevMonthClicked();
    void onNextMonthClicked();

private:
    int calendarId;
    QDate currentMonth;

    QLabel *monthLabel;
    QPushButton *prevButton;
    QPushButton *nextButton;
    QGridLayout *gridLayout;
    QWidget *gridWidget;

    std::map<int, std::vector<Event>> eventsByDay;

    void setupUi();
    void loadEvents();
    void rebuildGrid();
    void showDayPopup(const QDate &date, const std::vector<Event> &events);
    void updateMonthLabel();
    bool eventFilter(QObject *obj, QEvent *event) override;
};

#endif
