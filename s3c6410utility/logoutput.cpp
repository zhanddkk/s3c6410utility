#include "logoutput.h"

LogOutput::LogOutput(QObject *parent) : QObject(parent)
{

}

int LogOutput::stdout_printf(const char *__format, ...)
{
    register int __retval;
    __builtin_va_list __local_argv; __builtin_va_start( __local_argv, __format );
    QString msg = QString::vasprintf(__format, __local_argv);
    __retval = msg.length();
    __builtin_va_end( __local_argv );
    emit send_log_msg(e_STDOUT, msg);
    return __retval;
}

int LogOutput::stderr_printf(const char *__format, ...)
{
    register int __retval;
    __builtin_va_list __local_argv; __builtin_va_start( __local_argv, __format );
    QString msg = QString::vasprintf(__format, __local_argv);
    __retval = msg.length();
    __builtin_va_end( __local_argv );
    emit send_log_msg(e_STDERR, msg);
    return __retval;
}

int LogOutput::warning_printf(const char *__format, ...)
{
    register int __retval;
    __builtin_va_list __local_argv; __builtin_va_start( __local_argv, __format );
    QString msg = QString::vasprintf(__format, __local_argv);
    __retval = msg.length();
    __builtin_va_end( __local_argv );
    emit send_log_msg(e_WARNING, msg);
    return __retval;
}
