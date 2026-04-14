#include "ui/CreateCalendarDialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>

CreateCalendarDialog::CreateCalendarDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUi();
    setWindowTitle("New Calendar");
    setModal(true);
}

void CreateCalendarDialog::setupUi()
{
    nameEdit = new QLineEdit(this);
    nameEdit->setPlaceholderText("calendar name");

    typeCombo = new QComboBox(this);
    typeCombo->addItem("personal");
    typeCombo->addItem("shared");

    errorLabel = new QLabel(this);
    errorLabel->setStyleSheet("color: red;");
    errorLabel->setVisible(false);

    createButton = new QPushButton("Create", this);
    cancelButton = new QPushButton("Cancel", this);

    QFormLayout *formLayout = new QFormLayout();
    formLayout->addRow("Name:", nameEdit);
    formLayout->addRow("Type:", typeCombo);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(cancelButton);
    buttonLayout->addWidget(createButton);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(formLayout);
    mainLayout->addWidget(errorLabel);
    mainLayout->addLayout(buttonLayout);

    connect(createButton, &QPushButton::clicked, this, &CreateCalendarDialog::onCreateClicked);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

void CreateCalendarDialog::onCreateClicked()
{
    if (nameEdit->text().trimmed().isEmpty())
    {
        errorLabel->setText("Please enter a calendar name.");
        errorLabel->setVisible(true);
        return;
    }

    accept();
}

QString CreateCalendarDialog::getName() const
{
    return nameEdit->text().trimmed();
}

QString CreateCalendarDialog::getType() const
{
    return typeCombo->currentText();
}
