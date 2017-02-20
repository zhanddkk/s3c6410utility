#include "logoutput.h"

LogOutput::LogOutput(QObject *parent) : QObject(parent)
{
    qRegisterMetaType<emLogType>("emLogType");
}

int LogOutput::stdout_printf(const char *__format, ...)
{
    stdout_mutex.lock();
    register int __retval;
    __builtin_va_list __local_argv;
    __builtin_va_start( __local_argv, __format );
    QString msg = QString::vasprintf(__format, __local_argv);
    __retval = msg.length();
    __builtin_va_end( __local_argv );
    emit send_log_msg(e_STDOUT, msg);
    stdout_mutex.unlock();
    return __retval;
}

int LogOutput::stderr_printf(const char *__format, ...)
{
    stderr_mutex.lock();
    register int __retval;
    __builtin_va_list __local_argv;
    __builtin_va_start( __local_argv, __format );
    QString msg = QString::vasprintf(__format, __local_argv);
    __retval = msg.length();
    __builtin_va_end( __local_argv );
    emit send_log_msg(e_STDERR, msg);
    stderr_mutex.unlock();
    return __retval;
}

int LogOutput::warning_printf(const char *__format, ...)
{
    warning_mutex.lock();
    register int __retval;
    __builtin_va_list __local_argv;
    __builtin_va_start( __local_argv, __format );
    QString msg = QString::vasprintf(__format, __local_argv);
    __retval = msg.length();
    __builtin_va_end( __local_argv );
    emit send_log_msg(e_WARNING, msg);
    warning_mutex.unlock();
    return __retval;
}

int LogOutput::comment_printf(const char *__format, ...)
{
    comment_mutex.lock();
    register int __retval;
    __builtin_va_list __local_argv;
    __builtin_va_start( __local_argv, __format );
    QString msg = QString::vasprintf(__format, __local_argv);
    __retval = msg.length();
    __builtin_va_end( __local_argv );
    emit send_log_msg(e_COMMENT, msg);
    comment_mutex.unlock();
    return __retval;
}
