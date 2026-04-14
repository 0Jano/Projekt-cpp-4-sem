#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);

    QString getIdentifier() const;
    QString getPassword() const;

private slots:
    void onLoginClicked();

private:
    QLineEdit *emailEdit;
    QLineEdit *passwordEdit;
    QPushButton *loginButton;
    QPushButton *cancelButton;
    QLabel *errorLabel;

    void setupUi();
};

#endif