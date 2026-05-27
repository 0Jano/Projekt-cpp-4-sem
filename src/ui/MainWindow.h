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
    void onInvitationsClicked();
    void onCalendarSelected(int calendarId);

private:
    void setupUi();
    void updateStatus();
    void updateInvitationsButton();
    void showLoggedInView();
    void showLoggedOutView();

    QLabel *statusLabel;
    QPushButton *loginButton;
    QPushButton *registerButton;
    QPushButton *logoutButton;
    QPushButton *invitationsButton;
    QPushButton *newEventButton;
    QLabel *userAvatarLabel;
    QLabel *userNameLabel;
    QWidget *userPill;
    QWidget *sidebarWidget;

    QWidget *loggedOutView;
    QWidget *loggedInView;
    QStackedWidget *stackedWidget;
    CalendarListWidget *calendarListWidget;
    CalendarGridWidget *calendarGridWidget;

    int loggedInUserId;
};

#endif
