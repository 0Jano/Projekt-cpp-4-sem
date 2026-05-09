#ifndef CREATECALENDARDIALOG_H
#define CREATECALENDARDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>

class CreateCalendarDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CreateCalendarDialog(QWidget *parent = nullptr);

    QString getName() const;

private slots:
    void onCreateClicked();

private:
    QLineEdit *nameEdit;
    QPushButton *createButton;
    QPushButton *cancelButton;
    QLabel *errorLabel;

    void setupUi();
};

#endif
