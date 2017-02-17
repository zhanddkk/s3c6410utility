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
    QString media_type;
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

    bool read_sector(uint64_t sector);

signals:
    void search_finished(bool result);

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

/*
class Win32_DiskDrive : CIM_DiskDrive
{
  uint16   Availability;
  uint32   BytesPerSector;
  uint16   Capabilities[];
  string   CapabilityDescriptions[];
  string   Caption;
  string   CompressionMethod;
  uint32   ConfigManagerErrorCode;
  boolean  ConfigManagerUserConfig;
  string   CreationClassName;
  uint64   DefaultBlockSize;
  string   Description;
  string   DeviceID;
  boolean  ErrorCleared;
  string   ErrorDescription;
  string   ErrorMethodology;
  string   FirmwareRevision;
  uint32   Index;
  datetime InstallDate;
  string   InterfaceType;
  uint32   LastErrorCode;
  string   Manufacturer;
  uint64   MaxBlockSize;
  uint64   MaxMediaSize;
  boolean  MediaLoaded;
  string   MediaType;
  uint64   MinBlockSize;
  string   Model;
  string   Name;
  boolean  NeedsCleaning;
  uint32   NumberOfMediaSupported;
  uint32   Partitions;
  string   PNPDeviceID;
  uint16   PowerManagementCapabilities[];
  boolean  PowerManagementSupported;
  uint32   SCSIBus;
  uint16   SCSILogicalUnit;
  uint16   SCSIPort;
  uint16   SCSITargetId;
  uint32   SectorsPerTrack;
  string   SerialNumber;
  uint32   Signature;
  uint64   Size;
  string   Status;
  uint16   StatusInfo;
  string   SystemCreationClassName;
  string   SystemName;
  uint64   TotalCylinders;
  uint32   TotalHeads;
  uint64   TotalSectors;
  uint64   TotalTracks;
  uint32   TracksPerCylinder;
};

Properties
The Win32_DiskDrive class has these properties.
Availability
Data type: uint16
Access type: Read-only
Qualifiers: MappingStrings ("MIF.DMTF|Operational State|003.5", "MIB.IETF|HOST-RESOURCES-MIB.hrDeviceStatus")
Availability and status of the device.
This property is inherited from CIM_LogicalDevice.
Other (1)
Unknown (2)
Running/Full Power (3)
Running or Full Power
Warning (4)
In Test (5)
Not Applicable (6)
Power Off (7)
Off Line (8)
Off Duty (9)
Degraded (10)
Not Installed (11)
Install Error (12)
Power Save - Unknown (13)
The device is known to be in a power save mode, but its exact status is unknown.
Power Save - Low Power Mode (14)
The device is in a power save state but still functioning, and may exhibit degraded performance.
Power Save - Standby (15)
The device is not functioning, but could be brought to full power quickly.
Power Cycle (16)
Power Save - Warning (17)
The device is in a warning state, though also in a power save mode.
Paused (18)
Not Ready (19)
Not Configured (20)
Quiesced (21)
The disk drive is unavailable.
BytesPerSector
Data type: uint32
Access type: Read-only
Qualifiers: MappingStrings ("Win32API|Device Input and Output Structures|DISK_GEOMETRY|BytesPerSector"), units ("bytes")
Number of bytes in each sector for the physical disk drive.
Example: 512
Capabilities
Data type: uint16 array
Access type: Read-only
Qualifiers: ArrayType ("Indexed"), MappingStrings ("MIF.DMTF|Storage Devices|001.9", "MIF.DMTF|Storage Devices|001.11", "MIF.DMTF|Storage Devices|001.12", "MIF.DMTF|Disks|003.7"), ModelCorrespondence ("CIM_MediaAccessDevice.CapabilityDescriptions")
Array of capabilities of the media access device. For example, the device may support random access (3), removable media (7), and automatic cleaning (9).
This property is inherited from CIM_MediaAccessDevice.
Unknown (0)
Other (1)
Sequential Access (2)
Random Access (3)
Supports Writing (4)
Encryption (5)
Compression (6)
Supports Removeable Media (7)
Supports Removable Media
Manual Cleaning (8)
Automatic Cleaning (9)
SMART Notification (10)
Supports Dual Sided Media (11)
Supports Dual-Sided Media
Predismount Eject Not Required (12)
Ejection Prior to Drive Dismount Not Required
CapabilityDescriptions
Data type: string array
Access type: Read-only
Qualifiers: ArrayType ("Indexed"), ModelCorrespondence ("CIM_MediaAccessDevice.Capabilities")
List of more detailed explanations for any of the access device features indicated in the Capabilities array. Note, each entry of this array is related to the entry in the Capabilities array that is located at the same index.
This property is inherited from CIM_MediaAccessDevice.
Caption
Data type: string
Access type: Read-only
Qualifiers: MaxLen (64), DisplayName ("Caption")
Short description of the object.
This property is inherited from CIM_ManagedSystemElement.
CompressionMethod
Data type: string
Access type: Read-only
Algorithm or tool used by the device to support compression. This property is inherited from CIM_MediaAccessDevice.
The name of the compression algorithm or one of the following values.
("Unknown")
Whether the device supports compression capabilities or not is not known.
("Compressed")
The device supports compression capabilities but its compression scheme is not known or not disclosed.
("Not Compressed")
The device does not support compression.
ConfigManagerErrorCode
Data type: uint32
Access type: Read-only
Qualifiers: Schema ("Win32")
Windows Configuration Manager error code.
This property is inherited from CIM_LogicalDevice.
This device is working properly. (0)
Device is working properly.
This device is not configured correctly. (1)
Device is not configured correctly.
Windows cannot load the driver for this device. (2)
The driver for this device might be corrupted, or your system may be running low on memory or other resources. (3)
Driver for this device might be corrupted, or the system may be low on memory or other resources.
This device is not working properly. One of its drivers or your registry might be corrupted. (4)
Device is not working properly. One of its drivers or the registry might be corrupted.
The driver for this device needs a resource that Windows cannot manage. (5)
Driver for the device requires a resource that Windows cannot manage.
The boot configuration for this device conflicts with other devices. (6)
Boot configuration for the device conflicts with other devices.
Cannot filter. (7)
The driver loader for the device is missing. (8)
Driver loader for the device is missing.
This device is not working properly because the controlling firmware is reporting the resources for the device incorrectly. (9)
Device is not working properly. The controlling firmware is incorrectly reporting the resources for the device.
This device cannot start. (10)
Device cannot start.
This device failed. (11)
Device failed.
This device cannot find enough free resources that it can use. (12)
Device cannot find enough free resources to use.
Windows cannot verify this device's resources. (13)
Windows cannot verify the device's resources.
This device cannot work properly until you restart your computer. (14)
Device cannot work properly until the computer is restarted.
This device is not working properly because there is probably a re-enumeration problem. (15)
Device is not working properly due to a possible re-enumeration problem.
Windows cannot identify all the resources this device uses. (16)
Windows cannot identify all of the resources that the device uses.
This device is asking for an unknown resource type. (17)
Device is requesting an unknown resource type.
Reinstall the drivers for this device. (18)
Device drivers must be reinstalled.
Failure using the VxD loader. (19)
Your registry might be corrupted. (20)
Registry might be corrupted.
System failure: Try changing the driver for this device. If that does not work, see your hardware documentation. Windows is removing this device. (21)
System failure. If changing the device driver is ineffective, see the hardware documentation. Windows is removing the device.
This device is disabled. (22)
Device is disabled.
System failure: Try changing the driver for this device. If that doesn't work, see your hardware documentation. (23)
System failure. If changing the device driver is ineffective, see the hardware documentation.
This device is not present, is not working properly, or does not have all its drivers installed. (24)
Device is not present, not working properly, or does not have all of its drivers installed.
Windows is still setting up this device. (25)
Windows is still setting up the device.
Windows is still setting up this device. (26)
Windows is still setting up the device.
This device does not have valid log configuration. (27)
Device does not have valid log configuration.
The drivers for this device are not installed. (28)
Device drivers are not installed.
This device is disabled because the firmware of the device did not give it the required resources. (29)
Device is disabled. The device firmware did not provide the required resources.
This device is using an Interrupt Request (IRQ) resource that another device is using. (30)
Device is using an IRQ resource that another device is using.
This device is not working properly because Windows cannot load the drivers required for this device. (31)
Device is not working properly. Windows cannot load the required device drivers.
ConfigManagerUserConfig
Data type: boolean
Access type: Read-only
Qualifiers: Schema ("Win32")
If True, the device is using a user-defined configuration.
This property is inherited from CIM_LogicalDevice.
CreationClassName
Data type: string
Access type: Read-only
Qualifiers: CIM_Key
Name of the first concrete class to appear in the inheritance chain used in the creation of an instance. When used with the other key properties of the class, the property allows all instances of this class and its subclasses to be uniquely identified.
This property is inherited from CIM_LogicalDevice.
DefaultBlockSize
Data type: uint64
Access type: Read-only
Qualifiers: Units ("bytes")
Default block size, in bytes, for this device.
This property is inherited from CIM_MediaAccessDevice.
For more information about using uint64 values in scripts, see Scripting in WMI.
Description
Data type: string
Access type: Read-only
Qualifiers: DisplayName ("Description")
Description of the object.
This property is inherited from CIM_ManagedSystemElement.
DeviceID
Data type: string
Access type: Read-only
Qualifiers: Key, Override ("DeviceId"), MappingStrings ("WMI")
Unique identifier of the disk drive with other devices on the system.
This property is inherited from CIM_LogicalDevice.
ErrorCleared
Data type: boolean
Access type: Read-only
If True, the error reported in LastErrorCode is now cleared.
This property is inherited from CIM_LogicalDevice.
ErrorDescription
Data type: string
Access type: Read-only
More information about the error recorded in LastErrorCode, and information on any corrective actions that may be taken.
This property is inherited from CIM_LogicalDevice.
ErrorMethodology
Data type: string
Access type: Read-only
Type of error detection and correction supported by this device.
This property is inherited from CIM_MediaAccessDevice.
FirmwareRevision
Data type: string
Access type: Read-only
Qualifiers: MappingStrings ("Win32API|Device Input and Output Structures|STORAGE_DEVICE_DESCRIPTOR|ProductRevisionOffset")
Revision for the disk drive firmware that is assigned by the manufacturer.
Index
Data type: uint32
Access type: Read-only
Qualifiers: MappingStrings ("Win32API|Windows 95/98 Functions|DRIVE_MAP_INFO|btInt13Unit")
Physical drive number of the given drive. This property is filled by the STORAGE_DEVICE_NUMBER structure returned from the IOCTL_STORAGE_GET_DEVICE_NUMBER control code. A value of 0xffffffff indicates that the given drive does not map to a physical drive.
Example: 1
InstallDate
Data type: datetime
Access type: Read-only
Qualifiers: MappingStrings ("MIF.DMTF|ComponentID|001.5"), DisplayName ("Install Date")
Date and time the object was installed. This property does not need a value to indicate that the object is installed.
This property is inherited from CIM_ManagedSystemElement.
InterfaceType
Data type: string
Access type: Read-only
Qualifiers: MappingStrings ("Win32API|Device Input and Output Functions|DeviceIoControl")
Interface type of physical disk drive.
The values are:
SCSI
HDC
IDE
USB
1394
LastErrorCode
Data type: uint32
Access type: Read-only
Last error code reported by the logical device.
This property is inherited from CIM_LogicalDevice.
Manufacturer
Data type: string
Access type: Read-only
Qualifiers: MappingStrings ("Win32Registry|HKEY_LOCAL_MACHINE\\HARDWARE\\DEVICEMAP\\Scsi\\Scsi Port\\Scsi Bus\\Target Id\\Logical Unit Id\\Identifier", "Win32Registry|Manufacturer")
Name of the disk drive manufacturer.
Example: "Seagate"
MaxBlockSize
Data type: uint64
Access type: Read-only
Qualifiers: Units ("bytes")
Maximum block size, in bytes, for media accessed by this device.
This property is inherited from CIM_MediaAccessDevice.
For more information about using uint64 values in scripts, see Scripting in WMI.
MaxMediaSize
Data type: uint64
Access type: Read-only
Qualifiers: MappingStrings ("MIF.DMTF|Sequential Access Devices|001.2"), Units ("kilobytes")
Maximum media size, in kilobytes, of media supported by this device.
This property is inherited from CIM_MediaAccessDevice.
For more information about using uint64 values in scripts, see Scripting in WMI.
MediaLoaded
Data type: boolean
Access type: Read-only
Qualifiers: MappingStrings ("Win32API|Device Input and Output Structures|DISK_GEOMETRY|MediaType|FixedMedia")
If True, the media for a disk drive is loaded, which means that the device has a readable file system and is accessible. For fixed disk drives, this property will always be TRUE.
MediaType
Data type: string
Access type: Read-only
Qualifiers: MappingStrings ("Win32API|Device Input and Output Structures|DISK_GEOMETRY|MediaType")
Type of media used or accessed by this device.
Possible values are:
External hard disk media
Removable media ("Removable media other than floppy")
Fixed hard disk ("Fixed hard disk media")
Unknown ("Format is unknown")
MinBlockSize
Data type: uint64
Access type: Read-only
Qualifiers: Units ("bytes")
Minimum block size, in bytes, for media accessed by this device.
This property is inherited from CIM_MediaAccessDevice.
For more information about using uint64 values in scripts, see Scripting in WMI.
Model
Data type: string
Access type: Read-only
Qualifiers: MappingStrings ("Win32Registry|HKEY_LOCAL_MACHINE\\HARDWARE\\DEVICEMAP\\Scsi\\Scsi Port\\Scsi Bus\\Target Id\\Logical Unit Id\\Identifier", "Win32Registry|ProductId")
Manufacturer's model number of the disk drive.
Example: "ST32171W"
Name
Data type: string
Access type: Read-only
Qualifiers: DisplayName ("Name")
Label by which the object is known. When subclassed, the property can be overridden to be a key property.
This property is inherited from CIM_ManagedSystemElement.
NeedsCleaning
Data type: boolean
Access type: Read-only
If True, the media access device needs cleaning. Whether manual or automatic cleaning is possible is indicated in the Capabilities property.
This property is inherited from CIM_MediaAccessDevice.
NumberOfMediaSupported
Data type: uint32
Access type: Read-only
Maximum number of media which can be supported or inserted (when the media access device supports multiple individual media).
This property is inherited from CIM_MediaAccessDevice.
Partitions
Data type: uint32
Access type: Read-only
Qualifiers: MappingStrings ("Win32API|Device Input and Output Structures|PARTITION_INFORMATION|RecognizedPartition")
Number of partitions on this physical disk drive that are recognized by the operating system.
Example: 2
PNPDeviceID
Data type: string
Access type: Read-only
Qualifiers: Schema ("Win32")
Windows Plug and Play device identifier of the logical device.
This property is inherited from CIM_LogicalDevice.
Example: "*PNP030b"
PowerManagementCapabilities
Data type: uint16 array
Access type: Read-only
Array of the specific power-related capabilities of a logical device.
This property is inherited from CIM_LogicalDevice.
Unknown (0)
Not Supported (1)
Power-related capacities are not supported for this device.
Disabled (2)
Enabled (3)
The power management features are currently enabled but the exact feature set is unknown or the information is unavailable.
Power Saving Modes Entered Automatically (4)
The device can change its power state based on usage or other criteria.
Power State Settable (5)
The SetPowerState method is supported. This method is found on the parent CIM_LogicalDevice class and can be implemented. For more information, see Designing Managed Object Format (MOF) Classes.
Power Cycling Supported (6)
The SetPowerState method can be invoked with the PowerState parameter set to 5 (Power Cycle).
Timed Power On Supported (7)
Timed Power-On Supported
The SetPowerState method can be invoked with the PowerState parameter set to 5 (Power Cycle) and Time set to a specific date and time, or interval, for power-on.
PowerManagementSupported
Data type: boolean
Access type: Read-only
If True, the device can be power-managed (can be put into suspend mode, and so on). The property does not indicate that power management features are currently enabled, only that the logical device is capable of power management.
This property is inherited from CIM_LogicalDevice.
SCSIBus
Data type: uint32
Access type: Read-only
Qualifiers: MappingStrings ("Win32API|Device Input and Output Structures|SCSI_ADDRESS|PathId")
SCSI bus number of the disk drive.
Example: 0
SCSILogicalUnit
Data type: uint16
Access type: Read-only
Qualifiers: MappingStrings ("Win32API|Device Input and Output Structures|SCSI_ADDRESS|Lun")
SCSI logical unit number (LUN) of the disk drive.
Example: 0
SCSIPort
Data type: uint16
Access type: Read-only
Qualifiers: MappingStrings ("Win32API|Device Input and Output Structures|SCSI_ADDRESS|PortNumber")
SCSI port number of the disk drive.
Example: 0
SCSITargetId
Data type: uint16
Access type: Read-only
Qualifiers: MappingStrings ("Win32API|Device Input and Output Structures|SCSI_ADDRESS|TargetId")
SCSI identifier number of the disk drive.
Example: 0
SectorsPerTrack
Data type: uint32
Access type: Read-only
Qualifiers: MappingStrings ("Win32API|Device Input and Output Structures|DISK_GEOMETRY|SectorsPerTrack")
Number of sectors in each track for this physical disk drive.
Example: 63
SerialNumber
Data type: string
Access type: Read-only
Qualifiers: MappingStrings ("Win32API|Device Input and Output Structures|STORAGE_DEVICE_DESCRIPTOR|SerialNumberOffset")
Number allocated by the manufacturer to identify the physical media.
Example: WD-WM3493798728
Signature
Data type: uint32
Access type: Read-only
Qualifiers: MappingStrings ("Win32API|Device Input and Output Structures|DRIVE_LAYOUT_INFORMATION|Signature")
Disk identification. This property can be used to identify a shared resource.
Size
Data type: uint64
Access type: Read-only
Qualifiers: MappingStrings ("Win32API|Device Input and Output Structures|DISK_GEOMETRY"), units ("bytes")
Size of the disk drive. It is calculated by multiplying the total number of cylinders, tracks in each cylinder, sectors in each track, and bytes in each sector.
For more information about using uint64 values in scripts, see Scripting in WMI.
Status
Data type: string
Access type: Read-only
Qualifiers: MaxLen (10), DisplayName ("Status")
Current status of the object. Various operational and nonoperational statuses can be defined. Operational statuses include: "OK", "Degraded", and "Pred Fail" (an element, such as a SMART-enabled hard disk drive, may be functioning properly but predicting a failure in the near future). Nonoperational statuses include: "Error", "Starting", "Stopping", and "Service". The latter, "Service", could apply during mirror-resilvering of a disk, reload of a user permissions list, or other administrative work. Not all such work is online, yet the managed element is neither "OK" nor in one of the other states.
This property is inherited from CIM_ManagedSystemElement.
Values are:
OK ("OK")
Error ("Error")
Degraded ("Degraded")
Unknown ("Unknown")
Pred Fail ("Pred Fail")
Starting ("Starting")
Stopping ("Stopping")
Service ("Service")
Stressed ("Stressed")
NonRecover ("NonRecover")
No Contact ("No Contact")
Lost Comm ("Lost Comm")
StatusInfo
Data type: uint16
Access type: Read-only
Qualifiers: MappingStrings ("MIF.DMTF|Operational State|003.3")
State of the logical device. If this property does not apply to the logical device, the value 5 (Not Applicable) should be used.
This property is inherited from CIM_LogicalDevice.
Other (1)
Unknown (2)
Enabled (3)
Disabled (4)
Not Applicable (5)
SystemCreationClassName
Data type: string
Access type: Read-only
Qualifiers: Propagated ("CIM_System.CreationClassName"), CIM_Key
Value of the scoping computer's CreationClassName property.
This property is inherited from CIM_LogicalDevice.
SystemName
Data type: string
Access type: Read-only
Qualifiers: Propagated ("CIM_System.Name"), CIM_Key
Name of the scoping system.
This property is inherited from CIM_LogicalDevice.
TotalCylinders
Data type: uint64
Access type: Read-only
Qualifiers: MappingStrings ("Win32API|Device Input and Output Structures|DISK_GEOMETRY|Cylinders")
Total number of cylinders on the physical disk drive. Note: the value for this property is obtained through extended functions of BIOS interrupt 13h. The value may be inaccurate if the drive uses a translation scheme to support high-capacity disk sizes. Consult the manufacturer for accurate drive specifications.
Example: 657
For more information about using uint64 values in scripts, see Scripting in WMI.
TotalHeads
Data type: uint32
Access type: Read-only
Qualifiers: MappingStrings ("Win32API|Device Input and Output Structures|DISK_GEOMETRY|TracksPerCylinder")
Total number of heads on the disk drive. Note: the value for this property is obtained through extended functions of BIOS interrupt 13h. The value may be inaccurate if the drive uses a translation scheme to support high-capacity disk sizes. Consult the manufacturer for accurate drive specifications.
TotalSectors
Data type: uint64
Access type: Read-only
Qualifiers: MappingStrings ("Win32API|Device Input and Output Structures|DISK_GEOMETRY|SectorsPerTrack")
Total number of sectors on the physical disk drive. Note: the value for this property is obtained through extended functions of BIOS interrupt 13h. The value may be inaccurate if the drive uses a translation scheme to support high-capacity disk sizes. Consult the manufacturer for accurate drive specifications.
Example: 2649024
For more information about using uint64 values in scripts, see Scripting in WMI.
TotalTracks
Data type: uint64
Access type: Read-only
Qualifiers: MappingStrings ("Win32API|Device Input and Output Structures|DISK_GEOMETRY|TracksPerCylinder")
Total number of tracks on the physical disk drive. Note: the value for this property is obtained through extended functions of BIOS interrupt 13h. The value may be inaccurate if the drive uses a translation scheme to support high-capacity disk sizes. Consult the manufacturer for accurate drive specifications.
Example: 42048
For more information about using uint64 values in scripts, see Scripting in WMI.
TracksPerCylinder
Data type: uint32
Access type: Read-only
Qualifiers: MappingStrings ("Win32API|Device Input and Output Structures|DISK_GEOMETRY|TracksPerCylinder")
Number of tracks in each cylinder on the physical disk drive. Note: the value for this property is obtained through extended functions of BIOS interrupt 13h. The value may be inaccurate if the drive uses a translation scheme to support high-capacity disk sizes. Consult the manufacturer for accurate drive specifications.
Example: 64
*/
