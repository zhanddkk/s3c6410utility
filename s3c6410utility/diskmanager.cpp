#include "diskmanager.h"
#include "qsimplethread.h"

using namespace std;

DiskManager::DiskManager(QObject *parent) : QObject(parent)
{
    rearch_thread.set_parameters((void *)&DiskManager::do_search, (void *)this);
}

void DiskManager::search()
{
    rearch_thread.start();
}


void DiskManager::do_search(void *obj)
{

    BOOL ret = FALSE;
    DiskManager *p_this = (DiskManager *)obj;
    p_this->p_log->stdout_printf("Search Disk...\n");
    ret = p_this->wmi_run();
    if (ret)
    {
        ret = p_this->wmi_getDriveLetters();
        if (ret)
        {
            p_this->wmi_close();
        }
    }
    emit p_this->search_finished((bool)ret);
}

BOOL DiskManager::wmi_run()
{
    HRESULT hres;

    // Step 1: --------------------------------------------------
    // Initialize COM. ------------------------------------------

#if 1
    hres = CoInitializeEx(0, COINIT_MULTITHREADED);
#else
    hres = CoInitializeEx(0, COINIT_APARTMENTTHREADED);
#endif

    if (FAILED(hres))
    {
        // cout << "Failed to initialize COM library. Error code = 0x"
        //     << hex << hres << endl;
        p_log->stderr_printf(
            "Failed to initialize COM library. Error code = 0x%08lX\n",
            hres);
        return FALSE;                  // Program has failed.
    }

    // Step 2: --------------------------------------------------
    // Set general COM security levels --------------------------
    // Note: If you are using Windows 2000, you need to specify -
    // the default authentication credentials for a user by using
    // a SOLE_AUTHENTICATION_LIST structure in the pAuthList ----
    // parameter of CoInitializeSecurity ------------------------

    hres = CoInitializeSecurity(
        NULL,
        -1,                          // COM authentication
        NULL,                        // Authentication services
        NULL,                        // Reserved
        RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication
        RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation
        NULL,                        // Authentication info
        EOAC_NONE,                   // Additional capabilities
        NULL                         // Reserved
    );



    if (FAILED(hres))
    {
        // cout << "Failed to initialize security. Error code = 0x"
        //     << hex << hres << endl;
        p_log->stderr_printf(
            "Failed to initialize security. Error code = 0x%08lX\n",
            hres);
        CoUninitialize();
        return FALSE;                    // Program has failed.
    }

    // Step 3: ---------------------------------------------------
    // Obtain the initial locator to WMI -------------------------

    //IWbemLocator *pLoc = NULL;

    hres = CoCreateInstance(
        CLSID_WbemLocator,
        0,
        CLSCTX_INPROC_SERVER,
        IID_IWbemLocator, (LPVOID *)&p_loc);

    if (FAILED(hres))
    {
        // cout << "Failed to create IWbemLocator object."
        //     << " Err code = 0x"
        //     << hex << hres << endl;
        p_log->stderr_printf(
            "Failed to create IWbemLocator object. Error code = 0x%08lX\n",
            hres);
        CoUninitialize();
        return FALSE;                 // Program has failed.
    }

    // Step 4: -----------------------------------------------------
    // Connect to WMI through the IWbemLocator::ConnectServer method

    //IWbemServices *pSvc = NULL;

    // Connect to the root\cimv2 namespace with
    // the current user and obtain pointer pSvc
    // to make IWbemServices calls.
    hres = p_loc->ConnectServer(
        _bstr_t(L"ROOT\\CIMV2"), // Object path of WMI namespace
        NULL,                    // User name. NULL = current user
        NULL,                    // User password. NULL = current
        0,                       // Locale. NULL indicates current
        0,                       // Security flags.
        0,                       // Authority (e.g. Kerberos)
        NULL,                    // Context object
        &p_svc                   // pointer to IWbemServices proxy
    );

    if (FAILED(hres))
    {
        // cout << "Could not connect. Error code = 0x"
        //     << hex << hres << endl;
        p_log->stderr_printf("Could not connect. Error code = 0x%08lX\n", hres);
        p_loc->Release();
        CoUninitialize();
        return FALSE;                // Program has failed.
    }

    p_log->stdout_printf("Connected to ROOT\\CIMV2 WMI namespace\n");

    // cout << "Connected to ROOT\\CIMV2 WMI namespace" << endl;

    // Step 5: --------------------------------------------------
    // Set security levels on the proxy -------------------------

    hres = CoSetProxyBlanket(
        p_svc,                       // Indicates the proxy to set
        RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx
        RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx
        NULL,                        // Server principal name
        RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx
        RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
        NULL,                        // client identity
        EOAC_NONE                    // proxy capabilities
    );

    if (FAILED(hres))
    {
        // cout << "Could not set proxy blanket. Error code = 0x"
        //     << hex << hres << endl;
        p_log->stderr_printf(
            "Could not set proxy blanket. Error code = 0x%08lX\n",
            hres);
        p_svc->Release();
        p_loc->Release();
        CoUninitialize();
        return FALSE;               // Program has failed.
    }
    return TRUE;
}

//
// get Drives, logical Drives and Driveletters
BOOL DiskManager::wmi_getDriveLetters()
{

    // Use the IWbemServices pointer to make requests of WMI.
    // Make requests here:
#define QYERY_ERROR_MSG "Query for processes failed. Error code = 0x%08lX\n"

    HRESULT hres;
    IEnumWbemClassObject* p_enumerator = NULL;

    // get localdrives
    hres = p_svc->ExecQuery(
        _bstr_t(L"WQL"),
        _bstr_t(L"SELECT * FROM Win32_DiskDrive"),
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
        NULL,
        &p_enumerator);

    if (FAILED(hres)) {
        // cout << "Query for processes failed. "
        //     << "Error code = 0x"
        //     << hex << hres << endl;
        p_log->stderr_printf(
            QYERY_ERROR_MSG,
            hres);
        p_svc->Release();
        p_loc->Release();
        CoUninitialize();
        return FALSE;               // Program has failed.
    }
    else {

        IWbemClassObject *p_cls_obj;
        ULONG u_return = 0;
        while (p_enumerator) {
            hres = p_enumerator->Next(WBEM_INFINITE, 1,
                &p_cls_obj, &u_return);
            if (0 == u_return) break;

            VARIANT vt_prop;
            struct PhysicalDisk _new_phy_disk;

            hres = p_cls_obj->Get(_bstr_t(L"InterfaceType"), 0, &vt_prop, 0, 0);
            _new_phy_disk.interface_type =
                    QString::fromStdWString(vt_prop.bstrVal);
            VariantClear(&vt_prop);

            hres = p_cls_obj->Get(_bstr_t(L"MediaType"), 0, &vt_prop, 0, 0);
            _new_phy_disk.media_type =
                    QString::fromStdWString(vt_prop.bstrVal);
            VariantClear(&vt_prop);

            hres = p_cls_obj->Get(_bstr_t(L"BytesPerSector"),
                                  0,
                                  &vt_prop,
                                  0,
                                  0);
            _new_phy_disk.bytes_per_sector = vt_prop.intVal;
            VariantClear(&vt_prop);

            hres = p_cls_obj->Get(_bstr_t(L"TotalSectors"), 0, &vt_prop, 0, 0);
            _new_phy_disk.total_sectors =
                QString::fromStdWString(vt_prop.bstrVal).toLongLong();
            VariantClear(&vt_prop);

            hres = p_cls_obj->Get(_bstr_t(L"DeviceID"), 0, &vt_prop, 0, 0);
            _new_phy_disk.device_id = QString::fromStdWString(vt_prop.bstrVal);

            // adjust string
            wstring tmp = vt_prop.bstrVal;
            tmp = tmp.substr(4);

            wstring wstr_query =
                L"Associators of {Win32_DiskDrive.DeviceID='\\\\.\\";
            wstr_query += tmp;
            wstr_query += L"'} where AssocClass=Win32_DiskDriveToDiskPartition";

            // reference drive to partition
            IEnumWbemClassObject* p_enumerator1 = NULL;
            hres = p_svc->ExecQuery(
                _bstr_t(L"WQL"),
                _bstr_t(wstr_query.c_str()),
                WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
                NULL,
                &p_enumerator1);

            if (FAILED(hres)) {
                // cout << "Query for processes failed. "
                //     << "Error code = 0x"
                //     << hex << hres << endl;
                p_log->stderr_printf(
                    QYERY_ERROR_MSG,
                    hres);
                p_svc->Release();
                p_loc->Release();
                CoUninitialize();
                return FALSE;               // Program has failed.
            }
            else {

                IWbemClassObject *p_cls_obj1;
                ULONG u_return1 = 0;
                while (p_enumerator1) {
                    hres = p_enumerator1->Next(WBEM_INFINITE, 1,
                        &p_cls_obj1, &u_return1);
                    if (0 == u_return1) break;

                    // reference logical drive to partition
                    VARIANT vt_prop1;
                    hres = p_cls_obj1->Get(_bstr_t(L"DeviceID"),
                                           0,
                                           &vt_prop1,
                                           0,
                                           0);
                    wstring wstr_query =
                        L"Associators of {Win32_DiskPartition.DeviceID='";
                    wstr_query += vt_prop1.bstrVal;
                    wstr_query +=
                        L"'} where AssocClass=Win32_LogicalDiskToPartition";



                    IEnumWbemClassObject* p_enumerator2 = NULL;
                    hres = p_svc->ExecQuery(
                        _bstr_t(L"WQL"),
                        _bstr_t(wstr_query.c_str()),
                        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
                        NULL,
                        &p_enumerator2);





                    if (FAILED(hres)) {
                        // cout << "Query for processes failed. "
                        //     << "Error code = 0x"
                        //     << hex << hres << endl;
                        p_log->stderr_printf(
                            QYERY_ERROR_MSG,
                            hres);
                        p_svc->Release();
                        p_loc->Release();
                        CoUninitialize();
                        return FALSE;               // Program has failed.
                    }
                    else {

                        // get driveletter
                        IWbemClassObject *p_cls_obj2;
                        ULONG u_return2 = 0;
                        while (p_enumerator2) {
                            hres = p_enumerator2->Next(WBEM_INFINITE, 1,
                                &p_cls_obj2, &u_return2);
                            if (0 == u_return2) break;

                            VARIANT vt_prop2;
                            hres = p_cls_obj2->Get(_bstr_t(L"DeviceID"),
                                                   0,
                                                   &vt_prop2,
                                                   0,
                                                   0);



                            // print result
                            _new_phy_disk.logical_disks. push_back(
                                QString::fromStdWString(
                                    vt_prop2.bstrVal));
                            p_log->stdout_printf("%ls : %ls\n",
                                                 vt_prop.bstrVal,
                                                 vt_prop2.bstrVal);

                            VariantClear(&vt_prop2);
                        }
                        p_cls_obj1->Release();
                    }
                    VariantClear(&vt_prop1);
                    p_enumerator2->Release();
                }
                p_cls_obj->Release();
            }
            VariantClear(&vt_prop);
            p_enumerator1->Release();

            phy_disks.push_back(_new_phy_disk);
        }
    }
    p_enumerator->Release();
#undef QYERY_ERROR_MSG
    return TRUE;
}

BOOL DiskManager::wmi_close()
{
    // Cleanup
    // ========
    // Win32_DiskDrive a;
    p_svc->Release();
    p_loc->Release();
    CoUninitialize();

    return TRUE;   // Program successfully completed.
}
