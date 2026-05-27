#include <QApplication>
#include <QDebug>

#include "ui/MainWindow.h"
#include "managers/DatabaseManager.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    qApp->setStyleSheet(R"(
      QMainWindow, QWidget#centralWidget {
        background: #F5F5F5;
      }
      QPushButton {
        border: 1px solid #DADADA;
        border-radius: 6px;
        padding: 5px 12px;
        font-size: 13px;
        background: white;
        color: #202124;
      }
      QPushButton:hover {
        background: #F1F3F4;
      }
      QPushButton:disabled {
        color: #BDBDBD;
        border-color: #EBEBEB;
      }
      QListWidget {
        border: 1px solid #E0E0E0;
        border-radius: 6px;
        background: white;
      }
      QListWidget::item {
        padding: 6px 8px;
        border-radius: 4px;
      }
      QListWidget::item:selected {
        background: #E6F1FB;
        color: #185FA5;
      }
      QLineEdit, QTextEdit, QDateTimeEdit {
        border: 1px solid #DADADA;
        border-radius: 6px;
        padding: 6px 10px;
        font-size: 13px;
        background: white;
      }
      QLineEdit:focus, QTextEdit:focus, QDateTimeEdit:focus {
        border-color: #185FA5;
      }
      QDialog {
        background: white;
      }
      QLabel#sectionHeader {
        font-size: 11px;
        color: #888888;
        font-weight: bold;
      }
    )");

    if (!DatabaseManager::instance().openDatabase())
    {
        qDebug() << "Failed to open database.";
        return -1;
    }

    MainWindow window;
    window.show();

    return app.exec();
}
