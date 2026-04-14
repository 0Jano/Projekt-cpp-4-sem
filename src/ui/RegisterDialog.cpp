#include "ui/RegisterDialog.h"
#include "managers/AuthManager.h"
#include "models/User.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>

RegisterDialog::RegisterDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUi();
    setWindowTitle("Registration");
    setModal(true);
}

void RegisterDialog::setupUi()
{
    usernameEdit = new QLineEdit(this);
    usernameEdit->setPlaceholderText("username");

    emailEdit = new QLineEdit(this);
    emailEdit->setPlaceholderText("email address");

    passwordEdit = new QLineEdit(this);
    passwordEdit->setPlaceholderText("password");
    passwordEdit->setEchoMode(QLineEdit::Password);

    passwordConfirmEdit = new QLineEdit(this);
    passwordConfirmEdit->setPlaceholderText("confirm password");
    passwordConfirmEdit->setEchoMode(QLineEdit::Password);

    errorLabel = new QLabel(this);
    errorLabel->setStyleSheet("color: red;");
    errorLabel->setVisible(false);

    registerButton = new QPushButton("Register", this);
    cancelButton = new QPushButton("Cancel", this);

    QFormLayout *formLayout = new QFormLayout();
    formLayout->addRow("Username:", usernameEdit);
    formLayout->addRow("Email:", emailEdit);
    formLayout->addRow("Password:", passwordEdit);
    formLayout->addRow("Confirm password:", passwordConfirmEdit);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(cancelButton);
    buttonLayout->addWidget(registerButton);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(formLayout);
    mainLayout->addWidget(errorLabel);
    mainLayout->addLayout(buttonLayout);

    connect(registerButton, &QPushButton::clicked, this, &RegisterDialog::onRegisterClicked);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

void RegisterDialog::onRegisterClicked()
{
    const QString username = usernameEdit->text().trimmed();
    const QString email = emailEdit->text().trimmed();
    const QString password = passwordEdit->text();
    const QString passwordConfirm = passwordConfirmEdit->text();

    if (username.isEmpty() || email.isEmpty() || password.isEmpty())
    {
        errorLabel->setText("Please fill all fields.");
        errorLabel->setVisible(true);
        return;
    }

    if (password != passwordConfirm)
    {
        errorLabel->setText("Passwords do not match.");
        errorLabel->setVisible(true);
        passwordConfirmEdit->clear();
        return;
    }

    AuthManager authManager;
    User newUser(username, email, password);

    if (authManager.registerUser(newUser))
    {
        accept();
    }
    else
    {
        errorLabel->setText("Registration failed. Check if email is already taken.");
        errorLabel->setVisible(true);
    }
}