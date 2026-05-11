#ifndef CALENDARLISTWIDGET_H
#define CALENDARLISTWIDGET_H

#include <QWidget>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPushButton>
#include <QLabel>
#include <vector>
#include "models/Calendar.h"

class QMouseEvent;

class CalendarCardWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CalendarCardWidget(int calendarId,
                                const QString &name,
                                const QString &role,
                                bool selected,
                                bool favorite,
                                QWidget *parent = nullptr);

    int getCalendarId() const;
    void setSelected(bool selected);
    void setFavorite(bool favorite);

signals:
    void selected(int calendarId);
    void shareRequested(int calendarId);
    void deleteLeaveRequested(int calendarId);
    void favoriteRequested(int calendarId);

protected:
    void mousePressEvent(QMouseEvent *event) override;

private:
    int calendarId;
    QLabel *nameLabel;
    QPushButton *shareButton;
    QPushButton *deleteLeaveButton;
    QPushButton *starButton;

    void updateStyle(bool selected);
};

class CalendarListWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CalendarListWidget(QWidget *parent = nullptr);

    void setUserId(int userId);
    void refresh(bool selectFavorite = true);

signals:
    void calendarSelected(int calendarId);

private slots:
    void onAddClicked();
    void onShareClicked(int calendarId);
    void onDeleteLeaveClicked(int calendarId);
    void onStarClicked(int calendarId);
    void onItemClicked(QListWidgetItem *item);

private:
    int userId;
    int selectedCalendarId;
    int favoriteCalendarId;
    QListWidget *listWidget;
    QPushButton *addButton;
    QLabel *titleLabel;

    std::vector<Calendar> calendars;

    void setupUi();
    void selectCalendar(int calendarId, bool emitSelection);
    void selectInitialCalendar(bool selectFavorite);
    bool containsCalendar(int calendarId) const;
    Calendar findCalendar(int calendarId) const;
};

#endif
