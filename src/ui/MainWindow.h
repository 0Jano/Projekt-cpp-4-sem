#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QStackedWidget>
#include "ui/CalendarListWidget.h"
#include "ui/CalendarGridWidget.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void onLoginClicked();
    void onRegisterClicked();
    void onLogoutClicked();
    void onCalendarSelected(int calendarId);

private:
    void setupUi();
    void updateStatus();
    void showLoggedInView();
    void showLoggedOutView();

    QLabel *titleLabel;
    QLabel *statusLabel;
    QPushButton *loginButton;
    QPushButton *registerButton;
    QPushButton *logoutButton;

    QWidget *loggedOutView;
    QWidget *loggedInView;
    QStackedWidget *stackedWidget;
    CalendarListWidget *calendarListWidget;
    CalendarGridWidget *calendarGridWidget;

    int loggedInUserId;
};

#endif
