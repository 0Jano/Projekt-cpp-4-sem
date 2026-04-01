#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QLabel;
class QPushButton;

class MainWindow : public QMainWindow
{
public:
    MainWindow();

private:
    void setupUI();

    QLabel *titleLabel;
    QPushButton *loginButton;
    QPushButton *registerButton;
};

#endif