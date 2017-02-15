#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QScrollBar>
#include <QBrush>
#include <QColor>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(&log, SIGNAL(send_log_msg(emLogType, QString)), this, SLOT(display_log(emLogType, QString)));
    connect(ui->program_push_button, SIGNAL(clicked(bool)), this, SLOT(slot_program_button_clicked(bool)));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::display_log(emLogType log_type, QString msg)
{
    QTextCursor tmp(ui->log_output_text_edit->document());
    QScrollBar *vbar = ui->log_output_text_edit->verticalScrollBar();

    const bool atBottom = ui->log_output_text_edit->isReadOnly() ? vbar->value() >= vbar->maximum() : ui->log_output_text_edit->textCursor().atEnd();

    tmp.beginEditBlock();
    tmp.movePosition(QTextCursor::End);

    QTextCharFormat old_char_format = ui->log_output_text_edit->currentCharFormat();
    QTextCharFormat new_char_format = old_char_format;


    switch (log_type)
    {
    case e_STDOUT:
        new_char_format.setForeground(QBrush(QColor(0, 0, 0)));
        tmp.setCharFormat(new_char_format);
        break;
    case e_STDERR:
        msg = "ERROR: " + msg;
        new_char_format.setForeground(QBrush(QColor(0xf0, 0, 0)));
        tmp.setCharFormat(new_char_format);
        break;
    case e_WARNING:
        msg = "WARNING: " + msg;
        new_char_format.setForeground(QBrush(QColor(0xe0, 0x80, 0)));
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
    log.stdout_printf("--------Times = %d--------\n", times++);
    log.stdout_printf("This is standard output message!\n");
    log.stderr_printf("This is error output message!\n");
    log.warning_printf("This is warning output message!\n");
}
