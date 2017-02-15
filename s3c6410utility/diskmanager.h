#ifndef DISKMANAGER_H
#define DISKMANAGER_H

#include <QObject>
#define _WIN32_DCOM
#include <comdef.h>
#include <wbemidl.h>

#include "qsimplethread.h"
#include "logoutput.h"

struct PhysicalDisk{
    QString device_id;
    QString interface_type;
    std::vector<QString> logical_disks;
    UINT32 bytes_per_sector;
    UINT64 total_sectors;
};

class DiskManager : public QObject
{
    Q_OBJECT
public:
    explicit DiskManager(QObject *parent = 0);
    void search();

    std::vector<struct PhysicalDisk> phy_disks;
    LogOutput *p_log = NULL;

signals:
    void search_finished(BOOL result);

public slots:

private:
    static void do_search(void *obj);

    IWbemLocator *p_loc = NULL;
    IWbemServices *p_svc = NULL;

    BOOL wmi_run();
    BOOL wmi_getDriveLetters();
    BOOL wmi_close();

    QSimpleThread rearch_thread;
};

#endif // DISKMANAGER_H
