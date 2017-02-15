#include "qsimplethread.h"

QSimpleThread::QSimpleThread(QObject *parent, void *target, void *args) : QThread(parent)
{
    set_parameters(target, args);
}

void QSimpleThread::set_parameters(void *target, void *args)
{
    pf_target = target;
    p_args = args;
}

void QSimpleThread::run()
{
    if (pf_target)
    {
        if (p_args)
        {
            ((args_target_type)pf_target)(p_args);
        }
        else
        {
            ((no_args_target_type)pf_target)();
        }
    }
}
