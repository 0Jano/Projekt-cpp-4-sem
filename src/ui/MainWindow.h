#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QLabel>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void onLoginClicked();
    void onRegisterClicked();
    void onLogoutClicked();

private:
    void setupUI();
    void updateStatus();

    QLabel *titleLabel;
    QLabel *statusLabel;
    QPushButton *loginButton;
    QPushButton *registerButton;
    QPushButton *logoutButton;

    int loggedInUserId;
};

#endif