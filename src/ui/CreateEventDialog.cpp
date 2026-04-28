#include "ui/CreateEventDialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>

CreateEventDialog::CreateEventDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUi();
    setWindowTitle("New Event");
    setModal(true);
    setMinimumWidth(360);
}

void CreateEventDialog::setupUi()
{
    titleEdit = new QLineEdit(this);
    titleEdit->setPlaceholderText("event title");

    descriptionEdit = new QTextEdit(this);
    descriptionEdit->setPlaceholderText("description (optional)");
    descriptionEdit->setFixedHeight(72);

    locationEdit = new QLineEdit(this);
    locationEdit->setPlaceholderText("location (optional)");

    const QDateTime now = QDateTime::currentDateTime();

    startDateTimeEdit = new QDateTimeEdit(now, this);
    startDateTimeEdit->setDisplayFormat("yyyy-MM-dd HH:mm");
    startDateTimeEdit->setCalendarPopup(true);

    endDateTimeEdit = new QDateTimeEdit(now.addSecs(3600), this);
    endDateTimeEdit->setDisplayFormat("yyyy-MM-dd HH:mm");
    endDateTimeEdit->setCalendarPopup(true);

    errorLabel = new QLabel(this);
    errorLabel->setStyleSheet("color: red;");
    errorLabel->setVisible(false);

    createButton = new QPushButton("Create", this);
    cancelButton = new QPushButton("Cancel", this);

    QFormLayout *formLayout = new QFormLayout();
    formLayout->addRow("Title:", titleEdit);
    formLayout->addRow("Description:", descriptionEdit);
    formLayout->addRow("Location:", locationEdit);
    formLayout->addRow("Start:", startDateTimeEdit);
    formLayout->addRow("End:", endDateTimeEdit);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(cancelButton);
    buttonLayout->addWidget(createButton);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(formLayout);
    mainLayout->addWidget(errorLabel);
    mainLayout->addLayout(buttonLayout);

    connect(createButton, &QPushButton::clicked, this, &CreateEventDialog::onCreateClicked);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

void CreateEventDialog::onCreateClicked()
{
    if (titleEdit->text().trimmed().isEmpty())
    {
        errorLabel->setText("Please enter an event title.");
        errorLabel->setVisible(true);
        return;
    }

    if (endDateTimeEdit->dateTime() <= startDateTimeEdit->dateTime())
    {
        errorLabel->setText("End time must be after start time.");
        errorLabel->setVisible(true);
        return;
    }

    accept();
}

QString CreateEventDialog::getTitle() const
{
    return titleEdit->text().trimmed();
}

QString CreateEventDialog::getDescription() const
{
    return descriptionEdit->toPlainText().trimmed();
}

QString CreateEventDialog::getLocation() const
{
    return locationEdit->text().trimmed();
}

QDateTime CreateEventDialog::getStartDateTime() const
{
    return startDateTimeEdit->dateTime();
}

QDateTime CreateEventDialog::getEndDateTime() const
{
    return endDateTimeEdit->dateTime();
}

void CreateEventDialog::setTitle(const QString &title)
{
    titleEdit->setText(title);
}

void CreateEventDialog::setDescription(const QString &description)
{
    descriptionEdit->setPlainText(description);
}

void CreateEventDialog::setLocation(const QString &location)
{
    locationEdit->setText(location);
}

void CreateEventDialog::setStartDateTime(const QDateTime &dateTime)
{
    startDateTimeEdit->setDateTime(dateTime);
}

void CreateEventDialog::setEndDateTime(const QDateTime &dateTime)
{
    endDateTimeEdit->setDateTime(dateTime);
}