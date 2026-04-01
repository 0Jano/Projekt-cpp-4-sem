#include "ui/MainWindow.h"

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

MainWindow::MainWindow()
{
    setupUI();
    setWindowTitle("SyncCal");
    resize(900, 600);
}

void MainWindow::setupUI()
{
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout *layout = new QVBoxLayout(centralWidget);

    titleLabel = new QLabel("Welcome to SyncCal", this);
    loginButton = new QPushButton("Log in", this);
    registerButton = new QPushButton("Register", this);

    layout->addWidget(titleLabel);
    layout->addWidget(loginButton);
    layout->addWidget(registerButton);
}