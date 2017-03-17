#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QScrollBar>
#include <QBrush>
#include <QColor>
#include <QFontMetrics>
#include <dbt.h>
#include <QMenu>
#include <QFile>

static const GUID GUID_DEVINTERFACE_DISK_DEVICE = {
    0x53f56307,0xb6bf,0x11d0,
    {
        0x94, 0xf2, 0x00, 0xa0, 0xc9, 0x1e, 0xfb, 0x8b
    }
};

void MainWindow::register_device_notify()
{
    HDEVNOTIFY hDevNotify;
    Q_UNUSED(hDevNotify);
    DEV_BROADCAST_DEVICEINTERFACE NotificationFilter;
    ZeroMemory( &NotificationFilter, sizeof(NotificationFilter) );
    NotificationFilter.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
    NotificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
    NotificationFilter.dbcc_classguid = GUID_DEVINTERFACE_DISK_DEVICE;
    hDevNotify = RegisterDeviceNotification((HWND)winId(), &NotificationFilter,
                                            DEVICE_NOTIFY_WINDOW_HANDLE);
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // User ui default setting

    /* Set log output format */
    QFontMetrics _font_me(ui->log_output_text_edit->font());
    ui->log_output_text_edit->setTabStopWidth(_font_me.width("    "));

    QMenu *p_menu = ui->log_output_text_edit->createStandardContextMenu();
    p_menu->addAction("Test");


    connect(&log,
            SIGNAL(send_log_msg(emLogType, QString)),
            this,
            SLOT(display_log(emLogType, QString)));
    connect(ui->program_push_button,
            SIGNAL(clicked(bool)),
            this,
            SLOT(slot_program_button_clicked(bool)));
    connect(&disk_manager,
            SIGNAL(search_finished(bool)),
            this,
            SLOT(process_search_result(bool)));

    connect(ui->log_output_text_edit,
            SIGNAL(customContextMenuRequested(QPoint)),
            this,
            SLOT(slot_log_output_context_menu(QPoint)));

    disk_manager.p_log = &log;
    disk_manager.search();
    register_device_notify();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::slot_log_output_context_menu(const QPoint &pos)
{
    Q_UNUSED(pos)
    QMenu *menu = ui->log_output_text_edit->createStandardContextMenu();
    menu->addAction(tr("Clear"),
                    this,
                    SLOT(clear_log(bool)));
    menu->exec(QCursor::pos());
    delete menu;
}

void MainWindow::clear_log(bool)
{
    ui->log_output_text_edit->clear();
}

void MainWindow::display_log(emLogType log_type, QString msg)
{
    QTextCursor tmp(ui->log_output_text_edit->document());
    QScrollBar *vbar = ui->log_output_text_edit->verticalScrollBar();

    const bool atBottom =
        ui->log_output_text_edit->isReadOnly() ?
            vbar->value() >= vbar->maximum() :
            ui->log_output_text_edit->textCursor().atEnd();

    tmp.beginEditBlock();
    tmp.movePosition(QTextCursor::End);

    QTextCharFormat old_char_format =
        ui->log_output_text_edit->currentCharFormat();
    QTextCharFormat new_char_format = old_char_format;


    switch (log_type)
    {
    case e_STDOUT:
        new_char_format.setForeground(QBrush(QColor(0, 0, 0)));
        tmp.setCharFormat(new_char_format);
        break;
    case e_STDERR:
        msg = "ERROR: " + msg;
        new_char_format.setForeground(QBrush(QColor(0xff, 0, 0)));
        tmp.setCharFormat(new_char_format);
        break;
    case e_WARNING:
        msg = "WARNING: " + msg;
        new_char_format.setForeground(QBrush(QColor(0xdf, 0xd0, 0)));
        tmp.setCharFormat(new_char_format);
        break;
    case e_COMMENT:
        new_char_format.setForeground(QBrush(QColor(0, 0xe0, 0)));
        tmp.setCharFormat(new_char_format);
        break;
    default:
        break;
    }
    tmp.insertText(msg);

    if (!ui->log_output_text_edit->textCursor().hasSelection())
        ui->log_output_text_edit->setCurrentCharFormat(old_char_format);

    tmp.endEditBlock();

    if (atBottom)
        vbar->setValue(vbar->maximum());
}

void MainWindow::slot_program_button_clicked(bool)
{
//    static int times = 0;
//    log.comment_printf("--------Times = %d--------\n", times++);
//    log.stdout_printf("This is standard output message!\n");
//    log.stderr_printf("This is error output message!\n");
//    log.warning_printf("This is warning output message!\n");
    QString disk_id = ui->sd_card_physical_path_combo_box->currentText();
    if (disk_id.isEmpty())
        return;

    QFile hdd(disk_id);
    if (!hdd.open(QIODevice::ReadOnly))
        return;

    QByteArray d = hdd.read(512);
        // Linux allows non-512 mutiplies

    hdd.close();

    for (auto &value: d)
    {
        log.stdout_printf("%02X ", (uint8_t)value);
    }


//    QFile file("out.bin");

//    if (!file.open(QIODevice::WriteOnly))
//        return 255;

//    file.write(d);
//    file.close();
}

void MainWindow::process_search_result(bool result)
{
    if (result)
    {
        QString current_text =
            ui->sd_card_physical_path_combo_box->currentText();
        usb_disks.clear();
        ui->sd_card_physical_path_combo_box->clear();
        log.comment_printf("--------Disk Info--------\n");
        for (struct PhysicalDisk &disk : disk_manager.phy_disks)
        {
            log.stdout_printf("%s\n |->", disk.device_id.toStdString().c_str());

            log.comment_printf("Interface Type : ");
            log.stdout_printf("%s\n |->",
                              disk.interface_type.toStdString().c_str());

            log.comment_printf("Media Type     : ");
            log.stdout_printf("%s\n |->",
                              disk.media_type.toStdString().c_str());

            log.comment_printf("Logic Disks    : ");
            int i = 0;
            for (QString &logic_disk : disk.logical_disks)
            {
                log.stdout_printf(i > 0 ? ", [%s]" : "[%s]",
                                  logic_disk.toStdString().c_str());
                i++;
            }
            log.stdout_printf("\n |->");

            log.comment_printf("Byte Per Sector: ");
            log.stdout_printf("%lu\n └->", disk.bytes_per_sector);

            log.comment_printf("Total sectors  : ");
            log.stdout_printf("%llu\n", disk.total_sectors);

            if ((disk.interface_type == "USB" ||
                 disk.interface_type == "SCSI") &&
                disk.media_type == "Removable Media")
            {
                usb_disks.push_back(&disk);
            }
        }

        for (struct PhysicalDisk *&sd_card : usb_disks)
        {
            ui->sd_card_physical_path_combo_box->addItem(sd_card->device_id);
        }

        int current_index = ui->sd_card_physical_path_combo_box->findText(
            current_text,
            Qt::MatchExactly |
            Qt::MatchCaseSensitive);
        if (current_index != -1)
            ui->sd_card_physical_path_combo_box->setCurrentIndex(current_index);
    }
    else
    {
        log.stderr_printf("Can\'t find any valid disk in this system.\n");
    }
}

bool MainWindow::nativeEvent(const QByteArray &eventType,
                             void *message,
                             long *result)
{
    Q_UNUSED(eventType);
    Q_UNUSED(result);
    MSG *p_msg = reinterpret_cast<MSG *>(message);
    if (p_msg->message == WM_DEVICECHANGE)
    {
        PDEV_BROADCAST_HDR lpdb = (PDEV_BROADCAST_HDR)p_msg->lParam;
        switch (p_msg->wParam)
        {
        case DBT_DEVICEARRIVAL:
        case DBT_DEVICEREMOVECOMPLETE:
            if (lpdb->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE)
            {
                disk_manager.search();
            }
            break;
        default:
            break;
        }
    }
    return false;
}
