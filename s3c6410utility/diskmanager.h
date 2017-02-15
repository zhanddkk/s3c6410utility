#ifndef DISKMANAGER_H
#define DISKMANAGER_H

#include <QObject>
#define _WIN32_DCOM
#include <comdef.h>
#include <wbemidl.h>

#include "qsimplethread.h"

class DiskManager : public QObject
{
    Q_OBJECT
public:
    explicit DiskManager(QObject *parent = 0);
    void search();

signals:
    void search_finished(BOOL result);

public slots:

private:
    static void do_search(void *obj);
    QSimpleThread rearch_thread;
};

#endif // DISKMANAGER_H
