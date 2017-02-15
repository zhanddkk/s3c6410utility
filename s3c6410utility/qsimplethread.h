#ifndef QSIMPLETHREAD_H
#define QSIMPLETHREAD_H

#include <QObject>
#include <QThread>

typedef void (*args_target_type)(void *args);
typedef void (*no_args_target_type)(void);

class QSimpleThread : public QThread
{
    Q_OBJECT
public:
    explicit QSimpleThread(QObject *parent = 0,
                           void *target = NULL,
                           void *args = NULL);

    void set_parameters(void *target, void *args);

protected:
    void run();

signals:

public slots:

private:
    void *pf_target;
    void *p_args;
};

#endif // QSIMPLETHREAD_H
