#include "ui/MainWindow.h"
#include "ui/LoginDialog.h"
#include "ui/RegisterDialog.h"
#include "managers/AuthManager.h"

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStackedWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), loggedInUserId(-1)
{
    setupUi();
    setWindowTitle("SyncCal");
    resize(900, 600);
}

void MainWindow::setupUi()
{
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    // top bar
    titleLabel = new QLabel("Welcome to SyncCal", this);
    statusLabel = new QLabel("Not logged in.", this);
    loginButton = new QPushButton("Log in", this);
    registerButton = new QPushButton("Register", this);
    logoutButton = new QPushButton("Log out", this);
    logoutButton->setEnabled(false);

    QHBoxLayout *topBar = new QHBoxLayout();
    topBar->addWidget(titleLabel);
    topBar->addStretch();
    topBar->addWidget(statusLabel);
    topBar->addWidget(loginButton);
    topBar->addWidget(registerButton);
    topBar->addWidget(logoutButton);

    // logged out view
    loggedOutView = new QWidget(this);
    QVBoxLayout *loggedOutLayout = new QVBoxLayout(loggedOutView);
    loggedOutLayout->addStretch();
    loggedOutLayout->addWidget(new QLabel("Log in to see your calendars.", loggedOutView));
    loggedOutLayout->addStretch();

    // logged in view
    loggedInView = new QWidget(this);
    calendarListWidget = new CalendarListWidget(loggedInView);
    calendarGridWidget = new CalendarGridWidget(loggedInView);

    QHBoxLayout *loggedInLayout = new QHBoxLayout(loggedInView);
    loggedInLayout->addWidget(calendarListWidget);
    loggedInLayout->addWidget(calendarGridWidget, 1);

    // stacked widget
    stackedWidget = new QStackedWidget(this);
    stackedWidget->addWidget(loggedOutView);
    stackedWidget->addWidget(loggedInView);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->addLayout(topBar);
    mainLayout->addWidget(stackedWidget);

    connect(loginButton, &QPushButton::clicked, this, &MainWindow::onLoginClicked);
    connect(registerButton, &QPushButton::clicked, this, &MainWindow::onRegisterClicked);
    connect(logoutButton, &QPushButton::clicked, this, &MainWindow::onLogoutClicked);
    connect(calendarListWidget, &CalendarListWidget::calendarSelected, this, &MainWindow::onCalendarSelected);
}

void MainWindow::onLoginClicked()
{
    LoginDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted)
    {
        AuthManager authManager;
        loggedInUserId = authManager.getUserId(dialog.getIdentifier());
        updateStatus();
        showLoggedInView();
    }
}

void MainWindow::onRegisterClicked()
{
    RegisterDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted)
    {
        statusLabel->setText("Account created. You can now log in.");
    }
}

void MainWindow::onLogoutClicked()
{
    loggedInUserId = -1;
    updateStatus();
    showLoggedOutView();
}

void MainWindow::onCalendarSelected(int calendarId)
{    
    calendarGridWidget->setCalendarId(calendarId);
}

void MainWindow::updateStatus()
{
    if (loggedInUserId != -1)
    {
        statusLabel->setText(QString("Logged in (ID: %1)").arg(loggedInUserId));
        loginButton->setEnabled(false);
        registerButton->setEnabled(false);
        logoutButton->setEnabled(true);
    }
    else
    {
        statusLabel->setText("Not logged in.");
        loginButton->setEnabled(true);
        registerButton->setEnabled(true);
        logoutButton->setEnabled(false);
    }
}

void MainWindow::showLoggedInView()
{
    calendarListWidget->setUserId(loggedInUserId);
    stackedWidget->setCurrentWidget(loggedInView);
}

void MainWindow::showLoggedOutView()
{
    calendarListWidget->setUserId(-1);
    stackedWidget->setCurrentWidget(loggedOutView);
}
