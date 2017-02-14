#ifndef LOGOUTPUT_H
#define LOGOUTPUT_H

#include <QObject>

typedef enum {
    e_STDOUT = 0,
    e_STDERR,
    e_WARNING,
}emLogType;

class LogOutput : public QObject
{
    Q_OBJECT
public:
    explicit LogOutput(QObject *parent = 0);

    int stdout_printf(const char *__format, ...);
    int stderr_printf(const char *__format, ...);
    int warning_printf(const char *__format, ...);

signals:
    void send_log_msg(emLogType, QString);

public slots:
};

#endif // LOGOUTPUT_H
