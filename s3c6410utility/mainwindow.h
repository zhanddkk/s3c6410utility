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
    std::list<struct PhysicalDisk *> usb_disks;
    void register_device_notify();

public slots:
    void display_log(emLogType log_type, QString msg);
    void slot_program_button_clicked(bool);
    void process_search_result(bool result);
    void slot_log_output_context_menu(const QPoint &pos);
    void clear_log(bool);

protected:
    bool nativeEvent(const QByteArray &eventType, void *message, long *result);
};

#endif // MAINWINDOW_H
