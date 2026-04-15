#ifndef CREATEEVENTDIALOG_H
#define CREATEEVENTDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QTextEdit>
#include <QDateTimeEdit>
#include <QPushButton>
#include <QLabel>

class CreateEventDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CreateEventDialog(QWidget *parent = nullptr);

    QString getTitle() const;
    QString getDescription() const;
    QString getLocation() const;
    QDateTime getStartDateTime() const;
    QDateTime getEndDateTime() const;

private slots:
    void onCreateClicked();

private:
    QLineEdit *titleEdit;
    QTextEdit *descriptionEdit;
    QLineEdit *locationEdit;
    QDateTimeEdit *startDateTimeEdit;
    QDateTimeEdit *endDateTimeEdit;
    QPushButton *createButton;
    QPushButton *cancelButton;
    QLabel *errorLabel;

    void setupUi();
};

#endif