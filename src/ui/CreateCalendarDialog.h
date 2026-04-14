#ifndef CREATECALENDARDIALOG_H
#define CREATECALENDARDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>

class CreateCalendarDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CreateCalendarDialog(QWidget *parent = nullptr);

    QString getName() const;
    QString getType() const;

private slots:
    void onCreateClicked();

private:
    QLineEdit *nameEdit;
    QComboBox *typeCombo;
    QPushButton *createButton;
    QPushButton *cancelButton;
    QLabel *errorLabel;

    void setupUi();
};

#endif
