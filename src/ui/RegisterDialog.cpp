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
    setWindowTitle("Rejestracja");
    setModal(true);
}

void RegisterDialog::setupUi()
{
    usernameEdit = new QLineEdit(this);
    usernameEdit->setPlaceholderText("nazwa użytkownika");

    emailEdit = new QLineEdit(this);
    emailEdit->setPlaceholderText("adres e-mail");

    passwordEdit = new QLineEdit(this);
    passwordEdit->setPlaceholderText("hasło");
    passwordEdit->setEchoMode(QLineEdit::Password);

    passwordConfirmEdit = new QLineEdit(this);
    passwordConfirmEdit->setPlaceholderText("powtórz hasło");
    passwordConfirmEdit->setEchoMode(QLineEdit::Password);

    errorLabel = new QLabel(this);
    errorLabel->setStyleSheet("color: red;");
    errorLabel->setVisible(false);

    registerButton = new QPushButton("Zarejestruj", this);
    cancelButton = new QPushButton("Anuluj", this);

    QFormLayout *formLayout = new QFormLayout();
    formLayout->addRow("Nazwa:", usernameEdit);
    formLayout->addRow("E-mail:", emailEdit);
    formLayout->addRow("Hasło:", passwordEdit);
    formLayout->addRow("Powtórz hasło:", passwordConfirmEdit);

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
        errorLabel->setText("Wypełnij wszystkie pola.");
        errorLabel->setVisible(true);
        return;
    }

    if (password != passwordConfirm)
    {
        errorLabel->setText("Hasła nie są zgodne.");
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
        errorLabel->setText("Rejestracja nie powiodła się. Sprawdź, czy e-mail nie jest zajęty.");
        errorLabel->setVisible(true);
    }
}