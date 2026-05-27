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
#include <QFrame>

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
    centralWidget->setObjectName("centralWidget");
    setCentralWidget(centralWidget);

    QLabel *logoIcon = new QLabel("□", this);
    logoIcon->setFixedSize(26, 26);
    logoIcon->setAlignment(Qt::AlignCenter);
    logoIcon->setStyleSheet("background: #E6F1FB; color: #185FA5; border-radius: 6px; font-weight: bold;");
    QLabel *logoText = new QLabel("SyncCal", this);
    logoText->setStyleSheet("font-size: 15px; font-weight: bold; color: #202124;");

    statusLabel = new QLabel(this);
    loginButton = new QPushButton("Log in", this);
    registerButton = new QPushButton("Register", this);
    logoutButton = new QPushButton("Log out", this);
    invitationsButton = new QPushButton("Invitations", this);
    invitationsButton->setVisible(false);
    invitationsButton->setEnabled(false);
    logoutButton->setEnabled(false);
    logoutButton->setFlat(true);
    logoutButton->setStyleSheet("QPushButton { border: none; background: transparent; padding: 5px 8px; } QPushButton:hover { background: #F1F3F4; }");

    userAvatarLabel = new QLabel("U", this);
    userAvatarLabel->setFixedSize(26, 26);
    userAvatarLabel->setAlignment(Qt::AlignCenter);
    userAvatarLabel->setStyleSheet("background: #185FA5; color: white; border-radius: 13px; font-weight: bold;");
    userNameLabel = new QLabel(this);

    userPill = new QWidget(this);
    userPill->setStyleSheet("QWidget { background: white; border: 1px solid #E0E0E0; border-radius: 16px; } QLabel { border: none; background: transparent; }");
    QHBoxLayout *userPillLayout = new QHBoxLayout(userPill);
    userPillLayout->setContentsMargins(4, 3, 10, 3);
    userPillLayout->setSpacing(6);
    userPillLayout->addWidget(userAvatarLabel);
    userPillLayout->addWidget(userNameLabel);
    userPill->setVisible(false);

    QHBoxLayout *topBar = new QHBoxLayout();
    topBar->setContentsMargins(16, 8, 16, 8);
    topBar->setSpacing(10);
    topBar->addWidget(logoIcon);
    topBar->addWidget(logoText);
    topBar->addStretch();
    topBar->addWidget(invitationsButton);
    topBar->addWidget(userPill);
    topBar->addWidget(logoutButton);

    loggedOutView = new QWidget(this);
    QVBoxLayout *loggedOutLayout = new QVBoxLayout(loggedOutView);
    loggedOutLayout->addStretch();
    QLabel *loggedOutTitle = new QLabel("SyncCal", loggedOutView);
    loggedOutTitle->setAlignment(Qt::AlignCenter);
    loggedOutTitle->setStyleSheet("font-size: 32px; font-weight: bold; color: #202124;");
    QLabel *loggedOutSubtitle = new QLabel("Manage your calendars", loggedOutView);
    loggedOutSubtitle->setAlignment(Qt::AlignCenter);
    loggedOutSubtitle->setStyleSheet("font-size: 15px; color: #5F6368;");
    QHBoxLayout *loggedOutButtons = new QHBoxLayout();
    loggedOutButtons->setSpacing(10);
    loggedOutButtons->addStretch();
    loginButton->setStyleSheet("QPushButton { background: #185FA5; color: white; border-color: #185FA5; padding: 8px 18px; font-weight: 600; } QPushButton:hover { background: #134D86; }");
    registerButton->setStyleSheet("QPushButton { background: white; color: #185FA5; border-color: #185FA5; padding: 8px 18px; font-weight: 600; }");
    loggedOutButtons->addWidget(loginButton);
    loggedOutButtons->addWidget(registerButton);
    loggedOutButtons->addStretch();
    loggedOutLayout->addWidget(loggedOutTitle);
    loggedOutLayout->addWidget(loggedOutSubtitle);
    loggedOutLayout->addSpacing(18);
    loggedOutLayout->addLayout(loggedOutButtons);
    loggedOutLayout->addStretch();

    loggedInView = new QWidget(this);
    calendarListWidget = new CalendarListWidget(loggedInView);
    calendarGridWidget = new CalendarGridWidget(loggedInView);

    sidebarWidget = new QWidget(loggedInView);
    sidebarWidget->setFixedWidth(210);
    sidebarWidget->setStyleSheet("QWidget { background: #FAFAFA; border-right: 1px solid #E0E0E0; }");
    newEventButton = new QPushButton("+ New event", sidebarWidget);
    newEventButton->setStyleSheet("QPushButton { background: #185FA5; color: white; border-color: #185FA5; font-weight: 600; padding: 8px 10px; } QPushButton:hover { background: #134D86; } QPushButton:disabled { background: #EBEBEB; color: #BDBDBD; border-color: #EBEBEB; }");
    QVBoxLayout *sidebarLayout = new QVBoxLayout(sidebarWidget);
    sidebarLayout->setContentsMargins(12, 14, 12, 14);
    sidebarLayout->setSpacing(10);
    sidebarLayout->addWidget(newEventButton);
    sidebarLayout->addWidget(calendarListWidget, 1);

    QHBoxLayout *loggedInLayout = new QHBoxLayout(loggedInView);
    loggedInLayout->setContentsMargins(0, 0, 0, 0);
    loggedInLayout->setSpacing(0);
    loggedInLayout->addWidget(sidebarWidget);
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
    connect(newEventButton, &QPushButton::clicked, calendarGridWidget, &CalendarGridWidget::onAddEventClicked);
    connect(calendarListWidget, &CalendarListWidget::calendarSelected, this, &MainWindow::onCalendarSelected);
}

void MainWindow::onLoginClicked()
{
    LoginDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted)
    {
        AuthManager authManager;
        loggedInUserId = authManager.getUserId(dialog.getIdentifier());
        statusLabel->setText(dialog.getIdentifier());
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
        const QString displayName = statusLabel->text().isEmpty()
            ? QString("ID %1").arg(loggedInUserId)
            : statusLabel->text();
        userNameLabel->setText(displayName);
        userAvatarLabel->setText(displayName.left(1).toUpper());
        loginButton->setEnabled(false);
        registerButton->setEnabled(false);
        logoutButton->setEnabled(true);
        invitationsButton->setVisible(true);
        invitationsButton->setEnabled(true);
        userPill->setVisible(true);
        logoutButton->setVisible(true);
    }
    else
    {
        statusLabel->clear();
        loginButton->setEnabled(true);
        registerButton->setEnabled(true);
        logoutButton->setEnabled(false);
        invitationsButton->setVisible(false);
        invitationsButton->setEnabled(false);
        userPill->setVisible(false);
        logoutButton->setVisible(false);
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
    calendarGridWidget->setUserId(loggedInUserId);
    calendarListWidget->setUserId(loggedInUserId);
    stackedWidget->setCurrentWidget(loggedInView);
}

void MainWindow::showLoggedOutView()
{
    calendarListWidget->setUserId(-1);
    calendarGridWidget->setUserId(-1);
    calendarGridWidget->setCalendarId(-1);
    stackedWidget->setCurrentWidget(loggedOutView);
}
