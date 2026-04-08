#include "ui/MainWindow.h"
#include "ui/LoginDialog.h"
#include "ui/RegisterDialog.h"
#include "managers/AuthManager.h"

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), loggedInUserId(-1)
{
    setupUI();
    setWindowTitle("SyncCal");
    resize(900, 600);
}

void MainWindow::setupUI()
{
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    titleLabel = new QLabel("Welcome to SyncCal", this);
    statusLabel = new QLabel("Nie zalogowano.", this);
    loginButton = new QPushButton("Log in", this);
    registerButton = new QPushButton("Register", this);
    logoutButton = new QPushButton("Log out", this);
    logoutButton->setEnabled(false);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(statusLabel);
    buttonLayout->addWidget(loginButton);
    buttonLayout->addWidget(registerButton);
    buttonLayout->addWidget(logoutButton);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->addWidget(titleLabel);
    mainLayout->addLayout(buttonLayout);
    mainLayout->addStretch();

    connect(loginButton, &QPushButton::clicked, this, &MainWindow::onLoginClicked);
    connect(registerButton, &QPushButton::clicked, this, &MainWindow::onRegisterClicked);
    connect(logoutButton, &QPushButton::clicked, this, &MainWindow::onLogoutClicked);
}

void MainWindow::onLoginClicked()
{
    LoginDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted)
    {
        AuthManager authManager;
        loggedInUserId = authManager.getUserIdByEmail(dialog.getEmail());
        updateStatus();
    }
}

void MainWindow::onRegisterClicked()
{
    RegisterDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted)
    {
        statusLabel->setText("Konto utworzone. Możesz się teraz zalogować.");
    }
}

void MainWindow::onLogoutClicked()
{
    loggedInUserId = -1;
    statusLabel->setText("Nie zalogowano.");
    loginButton->setEnabled(true);
    registerButton->setEnabled(true);
    logoutButton->setEnabled(false);
}

void MainWindow::updateStatus()
{
    if (loggedInUserId != -1)
    {
        statusLabel->setText(QString("Zalogowano (ID: %1)").arg(loggedInUserId));
        loginButton->setEnabled(false);
        registerButton->setEnabled(false);
        logoutButton->setEnabled(true);
    }
}