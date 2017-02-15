#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "diskmanager.h"
#include "logoutput.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    LogOutput log;
    DiskManager disk_manager;

public slots:
    void display_log(emLogType log_type, QString msg);
    void slot_program_button_clicked(bool);
};

#endif // MAINWINDOW_H
