#include "ui/MainWindow.h"
#include "ui/LoginDialog.h"
#include "ui/RegisterDialog.h"
#include "ui/InvitationsDialog.h"
#include "managers/AuthManager.h"
#include "managers/InvitationManager.h"

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

    statusLabel = new QLabel("Not logged in.", this);
    loginButton = new QPushButton("Log in", this);
    registerButton = new QPushButton("Register", this);
    logoutButton = new QPushButton("Log out", this);
    invitationsButton = new QPushButton("Invitations", this);
    invitationsButton->setVisible(false);
    invitationsButton->setEnabled(false);
    logoutButton->setEnabled(false);

    QHBoxLayout *topBar = new QHBoxLayout();
    topBar->addStretch();
    topBar->addWidget(statusLabel);
    topBar->addWidget(invitationsButton);
    topBar->addWidget(loginButton);
    topBar->addWidget(registerButton);
    topBar->addWidget(logoutButton);

    loggedOutView = new QWidget(this);
    QVBoxLayout *loggedOutLayout = new QVBoxLayout(loggedOutView);
    loggedOutLayout->addStretch();
    loggedOutLayout->addWidget(new QLabel("Log in to see your calendars.", loggedOutView));
    loggedOutLayout->addStretch();

    loggedInView = new QWidget(this);
    calendarListWidget = new CalendarListWidget(loggedInView);
    calendarGridWidget = new CalendarGridWidget(loggedInView);

    QHBoxLayout *loggedInLayout = new QHBoxLayout(loggedInView);
    loggedInLayout->addWidget(calendarListWidget);
    loggedInLayout->addWidget(calendarGridWidget, 1);

    stackedWidget = new QStackedWidget(this);
    stackedWidget->addWidget(loggedOutView);
    stackedWidget->addWidget(loggedInView);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->addLayout(topBar);
    mainLayout->addWidget(stackedWidget);

    connect(loginButton, &QPushButton::clicked, this, &MainWindow::onLoginClicked);
    connect(registerButton, &QPushButton::clicked, this, &MainWindow::onRegisterClicked);
    connect(logoutButton, &QPushButton::clicked, this, &MainWindow::onLogoutClicked);
    connect(invitationsButton, &QPushButton::clicked, this, &MainWindow::onInvitationsClicked);
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
        updateInvitationsButton();
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
    updateInvitationsButton();
}

void MainWindow::onInvitationsClicked()
{
    InvitationsDialog dialog(loggedInUserId, this);
    connect(&dialog, &InvitationsDialog::invitationsChanged, this, [this]() {
        calendarListWidget->refresh();
        updateInvitationsButton();
    });

    dialog.exec();
    calendarListWidget->refresh();
    updateInvitationsButton();
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
        invitationsButton->setVisible(true);
        invitationsButton->setEnabled(true);
    }
    else
    {
        statusLabel->setText("Not logged in.");
        loginButton->setEnabled(true);
        registerButton->setEnabled(true);
        logoutButton->setEnabled(false);
        invitationsButton->setVisible(false);
        invitationsButton->setEnabled(false);
    }
}

void MainWindow::updateInvitationsButton()
{
    if (loggedInUserId == -1)
    {
        invitationsButton->setText("Invitations");
        invitationsButton->setVisible(false);
        invitationsButton->setEnabled(false);
        return;
    }

    InvitationManager invitationManager;
    const int count = invitationManager.getPendingInvitationCountForUser(loggedInUserId);
    invitationsButton->setText(count > 0
        ? QString("Invitations (%1)").arg(count)
        : "Invitations");
    invitationsButton->setVisible(true);
    invitationsButton->setEnabled(true);
}

void MainWindow::showLoggedInView()
{
    calendarListWidget->setUserId(loggedInUserId);
    calendarGridWidget->setUserId(loggedInUserId);
    stackedWidget->setCurrentWidget(loggedInView);
}

void MainWindow::showLoggedOutView()
{
    calendarListWidget->setUserId(-1);
    calendarGridWidget->setUserId(-1);
    calendarGridWidget->setCalendarId(-1);
    stackedWidget->setCurrentWidget(loggedOutView);
}
