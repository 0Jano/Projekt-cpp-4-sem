#include "ui/LoginDialog.h"
#include "managers/AuthManager.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QMessageBox>

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUi();
    setWindowTitle("Logowanie");
    setModal(true);
}

void LoginDialog::setupUi()
{
    emailEdit = new QLineEdit(this);
    emailEdit->setPlaceholderText("adres e-mail");

    passwordEdit = new QLineEdit(this);
    passwordEdit->setPlaceholderText("hasło");
    passwordEdit->setEchoMode(QLineEdit::Password);

    errorLabel = new QLabel(this);
    errorLabel->setStyleSheet("color: red;");
    errorLabel->setVisible(false);

    loginButton = new QPushButton("Zaloguj", this);
    cancelButton = new QPushButton("Anuluj", this);

    QFormLayout *formLayout = new QFormLayout();
    formLayout->addRow("E-mail:", emailEdit);
    formLayout->addRow("Hasło:", passwordEdit);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(cancelButton);
    buttonLayout->addWidget(loginButton);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(formLayout);
    mainLayout->addWidget(errorLabel);
    mainLayout->addLayout(buttonLayout);

    connect(loginButton, &QPushButton::clicked, this, &LoginDialog::onLoginClicked);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

void LoginDialog::onLoginClicked()
{
    const QString email = emailEdit->text().trimmed();
    const QString password = passwordEdit->text();

    if (email.isEmpty() || password.isEmpty())
    {
        errorLabel->setText("Wypełnij wszystkie pola.");
        errorLabel->setVisible(true);
        return;
    }

    AuthManager authManager;
    if (authManager.loginUser(email, password))
    {
        accept();
    }
    else
    {
        errorLabel->setText("Nieprawidłowy e-mail lub hasło.");
        errorLabel->setVisible(true);
        passwordEdit->clear();
    }
}

QString LoginDialog::getEmail() const
{
    return emailEdit->text().trimmed();
}

QString LoginDialog::getPassword() const
{
    return passwordEdit->text();
}