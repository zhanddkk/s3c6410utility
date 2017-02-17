#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QScrollBar>
#include <QBrush>
#include <QColor>
#include <QFontMetrics>
#include <dbt.h>

static const GUID GUID_DEVINTERFACE_USB_DEVICE =
    {0xA5DCBF10, 0x6530, 0x11D2, {0x90, 0x1F, 0x00, 0xC0, 0x4F, 0xB9, 0x51, 0xED}};

void MainWindow::register_device_notify()
{
    HDEVNOTIFY hDevNotify;
    Q_UNUSED(hDevNotify);
    DEV_BROADCAST_DEVICEINTERFACE NotificationFilter;
    ZeroMemory( &NotificationFilter, sizeof(NotificationFilter) );
    NotificationFilter.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
    NotificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
    NotificationFilter.dbcc_classguid = GUID_DEVINTERFACE_USB_DEVICE;
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

    disk_manager.p_log = &log;
    disk_manager.search();
    register_device_notify();
}

MainWindow::~MainWindow()
{
    delete ui;
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
    static int times = 0;
    log.comment_printf("--------Times = %d--------\n", times++);
    log.stdout_printf("This is standard output message!\n");
    log.stderr_printf("This is error output message!\n");
    log.warning_printf("This is warning output message!\n");
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

            if (disk.interface_type == "USB" &&
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
        log.stderr_printf("Can\'t find any valid disk in this system.");
    }
}

bool MainWindow::nativeEvent(const QByteArray &eventType,
                             void *message,
                             long *result)
{
    // Q_UNUSED(eventType);
    MSG *p_msg = reinterpret_cast<MSG *>(message);
    if (p_msg->message == WM_DEVICECHANGE)
    {
        /*
        HWND hwnd;
        UINT message;
        WPARAM wParam;
        LPARAM lParam;
        DWORD time;
        POINT pt;
        */
        PDEV_BROADCAST_HDR lpdb = (PDEV_BROADCAST_HDR)p_msg->lParam;

        log.stdout_printf("%s: wParam: %08lX lParam: %08lX result: %ld",
                          QString(eventType).toStdString().c_str(),
                          p_msg->wParam,
                          p_msg->lParam,
                          result);
        switch (p_msg->wParam)\
        {
        case DBT_DEVICEARRIVAL:
            // log.stdout_printf("DBT_DEVICEARRIVAL\n");
            // break;
        case DBT_DEVICEREMOVECOMPLETE:
            // log.stdout_printf("DBT_DEVICEREMOVECOMPLETE\n");
            if (lpdb->dbch_devicetype == DBT_DEVTYP_VOLUME)
            {
                PDEV_BROADCAST_VOLUME lpdbv = (PDEV_BROADCAST_VOLUME)lpdb;

                if (lpdbv->dbcv_flags == 0)
                {
                    //StringCchPrintf(szMsg, sizeof(szMsg)/sizeof(szMsg[0]),
                    //    TEXT("Drive %c: Media has arrived.\n"),
                    //    FirstDriveFromMask(lpdbv ->dbcv_unitmask));
                    //MessageBox( hwnd, szMsg, TEXT("WM_DEVICECHANGE"), MB_OK );
                    log.stdout_printf("USB");
                }
            }
            break;
        default:
            // log.stdout_printf("\n");
            break;
        }
        log.stdout_printf("\n");

        /*
        if(wParam == DBT_DEVICEARRIVAL) //设备激活
        {
            PDEV_BROADCAST_HDR lpdb = (PDEV_BROADCAST_HDR)lParam;

            PDEV_BROADCAST_VOLUME lpdbv = (PDEV_BROADCAST_VOLUME)lpdb;

            char szMsg[80];
            sprintf (szMsg, "Drive %c: 被装载/n",
                FirstDriveFromMask(lpdbv ->dbcv_unitmask));

            MessageBox(hWnd, szMsg, "WM_DEVICECHANGE", MB_OK);

        }
        else if(wParam == DBT_DEVICEREMOVECOMPLETE)
        {
            PDEV_BROADCAST_HDR lpdb = (PDEV_BROADCAST_HDR)lParam;

            PDEV_BROADCAST_VOLUME lpdbv = (PDEV_BROADCAST_VOLUME)lpdb;

            char szMsg[80];
            sprintf (szMsg, "Drive %c: 被卸载/n",
                FirstDriveFromMask(lpdbv ->dbcv_unitmask));

            MessageBox(hWnd, szMsg, "WM_DEVICECHANGE", MB_OK);
        }
        */
    }
    else
    {
        // log.stdout_printf("\n");
    }
    return false;
}
