#ifndef CALENDARLISTWIDGET_H
#define CALENDARLISTWIDGET_H

#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QWidget>
#include <vector>

#include "models/Calendar.h"

class QEvent;

class CalendarListWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CalendarListWidget(QWidget *parent = nullptr);

    void setUserId(int userId);
    void refresh();

signals:
    void calendarSelected(int calendarId);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    void onAddClicked();

private:
    int userId;
    int selectedCalendarId;
    int favoriteCalendarId;
    QVBoxLayout *myCalendarsLayout;
    QVBoxLayout *sharedCalendarsLayout;
    QPushButton *addButton;
    QLabel *myCalendarsHeader;
    QLabel *sharedCalendarsHeader;

    std::vector<Calendar> calendars;

    void setupUi();
    void clearLayout(QVBoxLayout *layout);
    QWidget* createCalendarCard(const Calendar &calendar);
    void selectCalendar(int calendarId, bool emitSelection);
    void selectInitialCalendar();
};

#endif
