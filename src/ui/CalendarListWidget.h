#ifndef CALENDARLISTWIDGET_H
#define CALENDARLISTWIDGET_H

#include <QWidget>
#include <QListWidget>
#include <QPushButton>
#include <QLabel>
#include <vector>
#include "models/Calendar.h"

class CalendarListWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CalendarListWidget(QWidget *parent = nullptr);

    void setUserId(int userId);
    void refresh();

signals:
    void calendarSelected(int calendarId);

private slots:
    void onAddClicked();
    void onItemClicked(QListWidgetItem *item);

private:
    int userId;
    QListWidget *listWidget;
    QPushButton *addButton;
    QLabel *titleLabel;

    std::vector<Calendar> calendars;

    void setupUi();
};

#endif
