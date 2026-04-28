#ifndef SHARECALENDARDIALOG_H
#define SHARECALENDARDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>

class ShareCalendarDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ShareCalendarDialog(int calendarId, int currentUserId, QWidget *parent = nullptr);

private slots:
    void onShareClicked();

private:
    int calendarId;
    int currentUserId;
    QLineEdit *identifierEdit;
    QComboBox *roleCombo;
    QPushButton *shareButton;
    QPushButton *cancelButton;
    QLabel *errorLabel;

    void setupUi();
};

#endif
