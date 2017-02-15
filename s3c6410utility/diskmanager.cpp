#include "diskmanager.h"
#include "qsimplethread.h"


DiskManager::DiskManager(QObject *parent) : QObject(parent)
{
    rearch_thread.set_parameters((void *)do_search, (void *)this);
}

void DiskManager::search()
{
    rearch_thread.start();
}


void DiskManager::do_search(void *obj)
{
    int i = 0;
    while(1)
    {
        printf("%d ", i);
        i++;
        if (i > 100)
            break;
    }
}

