# HASP / HARDLOCK Dongle driver for NTVDMx64

## The problem

For using a dongle protected DOS application on Windows, there usually are
drivers available that enable the classic NTVDM to interact with the 
HARDLOCK.SYS driver provided by Aladdin Knowledge Systems (now Safenet Inc.).
As NTVDM usually is only supported on 32bit Windows versions, the driver that
interacts with the DOS-applications is currently only available for 32bit
Windows versions.
Now that we have NTVDMx64, there is the need to interact with such Dongles
also on 64bit Windows, but no driver is available from Safenet Inc.
In order to remedy this issue, I created a driver and VDD component to allow
interaction with the Dongle also on 64bit NTVDMx64.
Unfortunately, due to technical reasons explained later, this has to be done
in the form of a driver, unless you just want to emulate a simple dongle.

## Installation instructions for userspace emulator

### About the userspace emulator

In case you use a very simple HASP dongle in your DOS application, you may
already use a Dongle Emulator like HASPHL2007, Multikey or MkBus on your 
32bit machine. If this is the case, you should also have a .reg file defining
the dongle dump and it may be possible to emulate the dongle with 
[UCLHASP](http://www.woodmann.com/crackz/Tools/Dongles/Uclhasp.zip) functions.
For instructions for creating a .reg file from your existing dongle (or 
from existing HASPHL2007 emulator), see [here](http://www.reteam.org/board/showthread.php?t=3136).
In case the DOS application doesn't use the INT 6 (Invalid opcode) services
for decryption of cypted code, it may be enough to use the Dongle usermode 
emulation. The advantage of this approach is that you don't have to install a
driver on your 64bit machine and save you the hassle with possible 
instabilities of the original driver leading to sytem crashes and the driver 
signing problems discussed in the "full driver" installation instructions.
If it doesn't work, you can always try the full driver variant.

### 1) Installing prerequesites

1) You need the [NTVDMx64](https://github.com/leecher1337/ntvdmx64), of course
2) You must have added your emulator dongle dump .reg file to registry.
   Keys are searched in the following paths:
   
	HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Services\Emulator\Hasp\Dump
	HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\MultiKey\Dumps
	HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Mkbus

### 2) Installation 

1) Download the uclhaspemu.zip from "Releases" and unpack it 
   to a directory.
2) Run install.cmd in there, that should install the VDD and the emulator
   library

## Installation instructions for full driver 

### About the full driver

The full driver tries to give NTVDM access to the real HASP driver running
on your system, so this method is i.e. suitable for connecting a real physical
key to your machine as well as using kernel mode emulators like i.e. MkBus,
where 64bit drivers exist.

### About driver signing 

As it's nearly impossible nowadays to get a driver signing certificate for 
code signing, unfortunately, Windows usually has to be set into "Test signing 
mode" in order to load that driver, but as these stupid driver limitations 
are a problem for every Software writer, it may be better running a system 
in Test signing mode anyway.

However I found a code signing certificate on the Internet that currently
works, so in the best case, the installation package just works out of the
box. So there are 2 installation packages available, one that only works 
with test signing and one that got signed using mentioned certificate. 
You may first want to try the "signed" installer and only if it doesn't 
work (or your security solution prevents installation, cert got revoked
or whatever might happen in the future), try the "testsigned" installer.

### 1) Installing prerequesites

1) you need the [NTVDMx64](https://github.com/leecher1337/ntvdmx64), of 
   course.
2) you need the the [64bit hardlock driver](https://supportportal.thalesgroup.com/csm?sys_kb_id=979a4e21db92e78cfe0aff3dbf9619c6&id=kb_article_view&sysparm_rank=1&sysparm_tsqueryId=7efc79bddb8e81105d310573f3961942&sysparm_article=KB0018319).
   Download it and install it using the following command:
   ```
       haspdinst -i -ld
   ```
   
The `-ld` switch is important, so that the "legacy device" driver gets installed.
After successful installation of the dongle driver, continue with either 
signed or unsigned driver, according to your needs. It is recommended to
first try the signed installer.


### 2a) Installation for the signed installer (recommended)

1) Download the haspnt64-signed.zip from "Releases" and unpack it 
   to a directory.
2) Run install.cmd in there, that should install the driver and VDD 

**OR**

### 2b) Installation for the testsigned installer (not recommended)

1) Put your system into test signing mode by issuing the following command 
   on an elevated command prompt as admin:
   ```
       bcdedit /set testsigning on
   ```
   If it doesn't work and you get an error, i.e. Secure Boot preventing
   you from enabling test signing, refer to [this guide](https://www.thewindowsclub.com/disable-driver-signature-enforcement-windows) 
   on how to enable test signing using the boot menu.
2) Download the haspnt64-testsigned.zip from "Releases" and unpack it
   to a directory.
3) Run install.cmd in there, that should install the driver and VDD 

**OR**

### 2c) Manual installation   

In order to know, what the installer does, here is a short description of the
steps being carried out by the installer. Normally, you don't need this.

1) For the testsigned installer, refer to Step 1 of 2b)
2) Install the `haspnt64.cer` as Trusted publisher into your local certificate
   store:
   ``` 
      certutil -addstore "TrustedPublisher" haspnt64.cer
   ```
   This is crucial, otherwise the driver won't load. Be aware that these are
   completely different certificates in signed and testsigned installer.
3) Install the haspnt64.inf either by rightclicking on the `haspnt64.inf` and
   select "Install" or by issuing the following command:
   ```
      rundll32 setupapi,InstallHinfSection DefaultInstall 132 .\haspnt64.inf
   ```
4) After the system driver has been installed successfully, you need to load 
   the DOS driver on startup of NTVDM. So add the following line to your 
   `Windows\SysWOW64\config.nt` file :
   ```
      device=%SystemRoot%\system32\haspdos.sys
   ```
5) Now start the Windows driver that does the communication with the VDD:
   ```
      net start haspnt
   ```
  
  
## Architecture of the HASP DOS support libraries

Let's start from the lowest level to the highest.

Level  | Caller       | Interface  
-------|--------------|-------------------------------------------------------
DOS    | Application  | Calls DOS Device driver HASPDOSDRV via INPUT (read)
DOS    | HASPDOS.SYS  | Calls HASPVDD interface via BOP (VDDDispatch)
NTVDM  | HASPVDD.DLL  | Calls `\\.\Hasp` device via ReadFile call
KERNEL | HASPNT.SYS   | Calls `\Device\FNT0` via intl. HASP_IOCTL_DOS_DISPATCH
KERNEL | HARDLOCK.SYS | Operates the dongle 

Now some more details for every layer:

### The HASPDOS Interfacing code

The end user application gets linked with some HASP library that does the 
interfacing either directly with the dongle or on Windows NT with the 
HASPDOS.SYS driver. Reason is that there is no direct port access to the 
hardware Dongle on Windows NT.
The detection of NTVDM works via the system call to get the DOS version 
([Function 3306h of INT 21h](http://www.ctyme.com/intr/rb-2730.htm)).
If the version number is 5.50 (BX=3205h), NTVDM is assumed and the 
HASPDOSDRV driver gets called via the standard DOS device driver read 
routines ([Function 3Fh of INT 21h](http://www.ctyme.com/intr/rb-2783.htm)).
The speciality is that the read buffer gets loaded with a command 
structure and then receives the reply in this command structure.
It basically puts the register contents from the caller into that buffer
and receives the reply in the very same buffer.
For a documentation on how this buffer may look like, see 
`HaspDOSBufferStruc` in [haspintl.h](haspvdd/haspintl.h). 
For a documented disassembly of the DOS HASP code, see the IDA DB of 
[HASP Emulator v0.10d](https://pascal.sources.ru/hacker/haspx010.htm).

Here is a table of some basic DOS HASP functions:

```
+--------------------+--------------------+----------------------------------+
|       SERVICE      |       CALL         |             RETURN               |
|  BH=FUNC,  BL=PORT |                    |                                  |
+--------------------+--------------------+----------------------------------+
| 1.                 |                    |    AX : 0 - HASP NOT FOUND       |
|      ISHASP        |                    |         1 - HASP FOUND           |
+--------------------+--------------------+----------------------------------|
|2.                  |  AX=SEEDCODE       |    AX : 1ST RETURN CODE          |
|     HASPCODE       |  CX=PASSWORD 1     |    BX : 2ND RETURN CODE          |
|                    |  DX=PASSWORD 2     |    CX : 3RD RETURN CODE          |
|                    |                    |    DX : 4TH RETURN CODE          |
+--------------------+--------------------+----------------------------------+
|3.                  |  CX=PASSWORD 1     |    BX : DATA                     |
|     READMEMO       |  DX=PASSWORD 2     |    CX : STATUS                   |
|                    |  DI=MEMORY ADDR.   |    (READ 1 byte at DI)           |
+--------------------+--------------------+----------------------------------+
|4.                  |  CX=PASSWORD 1     |    CX : STATUS                   |
|     WRITEMEMO      |  DX=PASSWORD 2     |                                  |
|                    |  DI=MEMORY ADDR.   |                                  |
|                    |  SI=MEMORY DATA.   |                                  |
+--------------------+--------------------+----------------------------------+
|5.                  |  CX=PASSWORD 1     |    AX : MEMORY SIZE              |
|    HASPSTATUS      |  DX=PASSWORD 2     |    BX : HASP TYPE                |
|                    |                    |    CX : ACTUAL LPT_NUM           |
+--------------------+--------------------+----------------------------------+
|6.                  |  CX=PASSWORD 1     |    AX : IDLOW                    |
|      HASPID        |  DX=PASSWORD 2     |    BX : IDHIGH                   |
|                    |                    |    CX : STATUS                   |
+--------------------+--------------------+----------------------------------+
|32h.                |  CX=PASSWORD 1     |    CX : STATUS                   |
|     READBLOCK      |  DX=PASSWORD 2     |                                  |
|                    |  DI=MEM.START ADDR.|                                  |
|                    |  SI=BLOCK LENGTH   |                                  |
|                    |  ES=BUFER SEG.     |                                  |
|                    |  AX=BUFER OFFS.    |                                  |
+--------------------+--------------------+----------------------------------+
|33h.                |  CX=PASSWORD 1     |    CX : STATUS                   |
|     WRITEBLOCK     |  DX=PASSWORD 2     |                                  |
|                    |  DI=MEM.START ADDR.|                                  |
|                    |  SI=BLOCK LENGTH   |                                  |
|                    |  ES=BUFER SEG.     |                                  |
|                    |  AX=BUFER OFFS.    |                                  |
+--------------------+--------------------+----------------------------------+
```

### The HASPDOS.SYS DOS device driver 

The DOS device driver must be loaded via the CONFIG.NT file (NTVDM variant
of CONFIG.SYS). This is done via DEVICE= statement:
```
      device=%SystemRoot%\system32\haspdos.sys
```
The device driver flags indicate a character device driver that goes by
the name `HASPDOSDRV`. In its initialization routine, it registers an 
[INT 2f Multiplexter Interrupt](http://www.ctyme.com/intr/int-2f.htm)
handler for the function 5000h. When called, it returns some magic numbers
in register AX-DX. The purpose is currently unknown, maybe some kind of
additional installation check?
Besides that, it uses handler for OPEN, CLOSE and INPUT (read). 
On OPEN, it registers with the HASPVDD.DLL via well-known BOP interface.
The init-function is `VDDRegisterInit`, the Dispatch-function is 
`VDDDispatch`.
On CLOSE, it unregisters itself and on INPUT, it passes on the buffer
to the VDD. On error, the errorcode 8003h is being set.
Pretty straighforward.

You can see my sample implementation of the DOS driver in the 
[haspdos](haspdos/) directory in files [HASPDRV.INC](haspdos/HASPDRV.INC)
and [HASPDOS.ASM](haspdos/HASPDOS.ASM).
You can compile and link them via [TASM](https://en.wikipedia.org/wiki/Turbo_Assembler).
`c1.bat` shows how to compile and link it. You need `exe2bin` to convert it 
to a .sys file.


### The HASPVDD driver

As mentioned before, this driver includes the dispatch routines driven by the
HASPDOSDRV driver. The dispatch routine gets the pointer in ES:DI and a count 
in CX and then calls its internal `CALLVDDHASP` routine with this as 
parameters. Interestingly, there also is an unused routine called 
`CALLNEWVDDHASP` in it, which would pretty much resemble the interface we get
to know later in the next driver HASPNT.SYS. However, the routines that are 
called by HASPNT.SYS into HARDLOCK.SYS, which would be pretty much the 
structure of the `CALLNEWVDDHASP` call is a `IRP_MJ_INTERNAL_DEVICE_CONTROL`
routine, so it cannot be called from the VDD and thus we can forget about it.
Anyway, the `CALLVDDHASP` routine first decodes the "encrypted" buffer it got
passed. For  the encryption and decryption scheme, either see the 
`Decrypt28` routine in [haspio.c](haspvdd/haspio.c) for a C implementation or
the [HASPEMU.ASM](haspdos/HASPEMU.ASM) for an assembler implementation 
(stolen from [UCLHASP]( http://www.woodmann.com/crackz/Tools/Dongles/Uclhasp.zip)).
UCLHASP implementation had a bug of not initializing tmp_decrypt buffer 
properly, this has been fixed here.
If there is a buffer to be transferred from DOS memory to the driver, it 
translates the DOS pointer to a memory address readable by the driver and then
re-encrypts the buffer again (as DOS addresses cannot be directly mapped without
translation).
It opens a handle to the next driver in chain that is listening to the device
path `\\.\HASP`. Then it calls the [ReadFile](https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-readfile)
API on the local buffer it copied from DOS and by doing so calls the next driver
which takes the input buffer, and places the result in the same buffer as output.
For an example, see the `CallLegacyHardlock` function in the 
[hardlockdrv.c](haspvdd/hardlockdrv.c).

### The HASPNT driver 

The driver exposes mentioned device node `\\.\HASP` and is classified as a 
parallel port driver. 
On initialization, it opens a device handle to the downlevel HARDLOCK.SYS 
device node `\Device\FNT0`. It then sends an initialization request IRP 
`HASP_IOCTL_DOS_INITIALIZE` to it, so that it reads its settings from the 
registry for processing incoming packets. Please note that I made the IOCTL 
names up, but the IOCTL codes are the important part. For IOCTLS, see 
[hardlock.h](haspvdd\hardlock.h).
It is listening for `IRP_MJ_READ` requests and decrypts the passed-in buffer 
via the previously mentioned decryption routines. It then converts to buffer 
to a driver-internal format which is documented as 
`HaspWinBufferStruc` in [haspintl.h](haspvdd/haspintl.h). It mainly consists 
of 3 operation codes (see function `Convert28ToHL` in [haspio.c](haspvdd/haspio.c)),
a packet length of the whole struct, a packet length of the payload struct,
the payload buffer (passed in DOS-buffer except the first 4 bytes + 4 byte 
padding), followed by the length of a potentially encoded memory buffer 
following the structure on certain opcodes that requires such, as well as 
segment and offset addresses AX:ES, both in the pointer size native to the 
architecture of the target system, so the architecture of the driver.
As mentioned, payload can follow or not.
For a better understanding, have a look at [haspio.c](haspvdd/haspio.c).
This converted buffer structure then gets passed down to HARDLOCK.SYS driver 
via IOCTL `HASP_IOCTL_DOS_DISPATCH`. The bad thing about this IOCTL is,
that it is - as previously mentioned - a `IRP_MJ_INTERNAL_DEVICE_CONTROL`
control code, so it cannot be accessed by a usermode application (which 
would normally work fine and also was attempted by the not-used 
`CALLNEWVDDHASP` function in HASPVDD.DLL), so it is mandatory to implement 
a driver to access this IOCTL code unfortunately. HASPNT.SYS driver 
unfortunately cannot be bypassed.

This is just for the workflow of the HASPVDD.DLL incoming data packets, the 
driver offers far more functionality, but it's not intersting to us for our
usecase.

### The HARDLOCK.SYS driver 

The HARDLOCK.SYS driver is the core of the whole process, taking IOCTLS from 
the callers and create a request to the dongle from the passed-in buffer 
structure. It has to be said that there are multiple possible structures and
the structure here is not the only one being used. 

This is just for the workflow of the HASPVDD.DLL incoming data packets, the 
driver offers far more functionality, but it's not intersting to us for our
usecase.

## Implementation workflow of drivers on this repository

The drivers implemented here have a focus on stability and on the shortest 
code path. Therefore, i.e. translation functions that are normally in 
HASPNT.SYS have been transferred to usermode HASPVDD.DLL driver, as it 
eliminates potential bugs and crashes in the driver that can cause BSODs
or hangs.  So let's walk down the call chain of our drivers and see, which
options you have on the path.

### The HASPDOS.SYS DOS device driver 

This one is mandatory so that your DOS application has a connection point 
to the upper level drivers. The driver comes in 2 flavours:

1) The reimplemented HASPDOS.SYS driver that communicates with HASPVDD.DLL
2) A HASPEMU.SYS emulation driver that just does a "replay" on packets of 
   Calltype #2 (`HASPCODE`) for very dumb applications.
   
#### HASPDOS.SYS driver 
The driver isn't really needed to be built yourself, just take the original 
HASPDOS.SYS and you are good to go. There are no modifications in the workflow
compared to the original driver (only a few code optimizaions). The driver 
is just there for illustration purposes so that you have a commented version 
and see what it is basically doing. The driver only works in NTVDM, as it 
uses the well-known BOPping mechanism to call the VDD 
(see [isvbop.inc](haspdos/isvbop.inc)).


#### HASPEMU.SYS driver 
I also wrote a simple Dongle emulator for Call #2 (`HASPCODE`). 
This is for pretty stupid applications that just check the presence of the 
dongle by always using the same seed and the same passwords and just compare 
the return value. 
To get it working, you can sniff the communication with the dongle via 
a normal setup with using HASPVDD.DLL (so as a first step, you have to 
do a normal setup with HASPDOS.SYS driver and HASPVDD and then you can later
shortcut it by just using HASPEMU.SYS skipping the rest of the drivers in 
chain when you know the valid responses the driver has to send). By using 
[DebugView](https://docs.microsoft.com/en-us/sysinternals/downloads/debugview)
you can see the communcation between the dongle and the application.
Should there always be command 2 with the same Param1..4 and the result is
always the same in 1..4, you can write down these 4 values as hex and then 
just install the device driver with
```
    device=%SystemRoot%\system32\haspemu.sys <param1> <param2> <param3> <param4>
```
instead of haspdos.sys.

param1..4 have to be in hex without a 0x prefix, i.e.:
```
    device=%SystemRoot%\system32\haspemu.sys 1234 5678 9abc def0
```

Be aware that the values are little endian (so read from right to left).
Here is an example. Given an application that just checks for presence with 
call 1 and then always does the same HASPCODE, you get something like 
this on DebugView and can interpret it according to the table in 
[haspintl.h](haspvdd/haspintl.h):

```
________ [Ln] [ Ticks   ] [Service  ] [Param1/AX] [Param2/BX] [Param3/CX] [Param4/DX] [Para1R/DI] [Para2R/SI] [Para3R/ES] [Para4R/AX]
HASP OUT [28] AB 00 00 00 01 00 00 00 AD BA 00 00 00 00 00 00 EF BE 00 00 AD DE 00 00 00 00 00 00 00 00 00 00 00 00 00 00 AD BA 00 00
HASP IN  [28] AB 00 00 00 01 00 01 00 01 00 00 00 01 00 00 00 00 00 00 00 AD BA 00 00 01 00 00 00 01 00 00 00 00 00 00 00 AD BA 00 00
```

So let's interpret this:
```
Service:          1
Input parameters: Don't care
Returns:
  AX = 1  - HASP FOUND
  BX = 1  - Address of HASP dongle = 1
  CX = 0  - Success
```

Now for the next call (the one we are interested in):
```
________ [Ln] [ Ticks   ] [Service  ] [Param1/AX] [Param2/BX] [Param3/CX] [Param4/DX] [Para1R/DI] [Para2R/SI] [Para3R/ES] [Para4R/AX]
HASP OUT [28] C8 02 00 00 02 00 00 00 AD BA 00 00 00 00 00 00 EF BE 00 00 AD DE 00 00 01 00 00 00 01 00 00 00 00 00 00 00 AD BA 00 00
HASP IN  [28] C8 02 00 00 02 00 02 00 34 12 00 00 78 56 00 00 BC 9A 00 00 F0 DE 00 00 34 12 00 00 78 56 00 00 BC 9A 00 00 F0 DE 00 00
```

Interpreted:
```
Service:          2
Input parameters:
  AX = BAAD = Seed code
  CX = BEEF = Password 1
  DX = DEAD = Password 2

Returns:
  AX = 1234 = Return code 1
  BX = 5678 = Return code 2
  CX = 9ABC = Return code 3
  DX = DEF0 = Return code 4
```

So in this case, you have to load the device driver with the parameters 
mentioned in the example above (just put ax, bx, cx, dx results seperated by 
blanks in hex as parameters).

There also is a DEVLOAD.COM in the haspdos directory which allows you to load 
it afterwards and only on demand instead of everytime in config.nt at start:

```
    devload haspemu.sys 1234 5678 9abc def0
```

This way, you can also use the driver with other DOS emulators as long as they
are compliant with the DOS driver specifications.

### The HASPVDD.DLL driver

Unlike the original implementation, the driver has multiple operation modes 
that can be controlled by registry keys. 

#### The IOCTL based interface (default)

Unlike the original HASTNT.SYS driver, the driver from this package offers 
a direct means of communication via `HASP_IOCTL_DOS_DISPATCH` IOCTL code, 
so the same that gets used by the underlying HARDLOCK.SYS driver, but 
packets have to be forwarded due to the limitation of this being an 
internal IOCTL, as previously mentioned. This basically resembles the 
`CALLNEWVDDHASP` interface. The advantage is that all processing and 
preparation of the expected buffer is being done in usermode by the 
HASPVDD.DLL driver so that potential crashes don't make the whole system 
unstable. The call itself can be seen in `CallHardlock` function in 
[hardlockdrv.c](haspvdd/hardlockdrv.c). 
*NOTE*: This interface is *NOT* compatible with the original HASPNT.SYS 
driver!

#### The (original) ReadFile based interface 

This is mainly useful for logging purposes on a 32bit system where you 
installed the normal [HDD32 device driver 4.102.5.22 from Aladdin](https://www.caddiesoftware.com/index.php/download/summary/5-hasp-drivers/4-hasp-4-driver-version-4-102-5-22). 
With the help of this mode, you can play man-in-the-middle and sniff the
calls to the underlying dongle. For this, just replace the original 
HASPVDD.DLL driver with this one and set the following registry keys
in `HKEY_LOCAL_MACHINE\System\CurrentControlSet\Services\HaspNt\Parameter`:

```
+--------------------+-----------+--------+
| Key Name           | Type      | Value  |
+--------------------+-----------+--------+
| LegacyVDDInterface | REG_DWORD | 1      |
+--------------------+-----------+--------+
```

When enabled, you can see the calls being sent and received from and to the
dongle via [DebugView](https://docs.microsoft.com/en-us/sysinternals/downloads/debugview).
If you want to record the calls and check them later, you can set the 
following key to log them into a binary logfile:

```
+--------------------+-----------+-----------------------------------------+
| Key Name           | Type      | Value                                   |
+--------------------+-----------+-----------------------------------------+
| LogFile            | REG_SZ    | Full path and name of a file to log to. |
|                    |           | i.e. C:\Temp\hasplog.dmp                |
+--------------------+-----------+-----------------------------------------+
```

The logged calls can then be later dumped in human readable form by the 
[dumplog](dumplog/) utility. 

#### The Call 02 emulation interface (obsolete, better use UCLHASP!)

As UCLHASP emulation method offers more functionality, this method is not 
recommended anymore and this chapter just stays here for reference purposes:

As prevously mentioned, there may be applications where it is sufficient to 
just return static values to the 02 request. See chapter about the HASPEMU.SYS
driver for details. This is basically the same facility, but you don't need 
a seperate HASPEMU.SYS driver, but can use the normal HASPDOS.SYS driver and
just let the HASPVDD.DLL driver do the emulation.
The reason that there are 2 ways to accomplish the same goal is that the
HASPEMU.SYS driver may also work with other DOS emulations or even directly
in good old plain DOS, whereas the HASPDOS.SYS driver only works in conjunction
with NTVDM. If you can emulate the dongle using this method, you don't need 
the HASPNT64.SYS driver, as the function calls are directly answered by the 
HASPVDD.DLL driver and no more calls down the chain are necessary.
So in short, the shortest path is to use HASPEMU.SYS, the second shortest 
path is to use this emulation interface (HASPDOS.SYS + HASPVDD.DLL).

So the first step to use this (if the target application is suitable for 
this sort of emulation which I doubt many applications are) is to sniff the 
traffic between the application and the dongle by using before mentioned 
ReadFile based interface and gather the logs from it.
When you have captured the call, you need to create a registry key named
`EmulateParams` that contains the binary representation of the expected 
answer from Param1-4. 
Using the example from HASPEMU.SYS driver, say, you have this static reply
from the dongle:

```
________ [Ln] [ Ticks   ] [Service  ] [Param1/AX] [Param2/BX] [Param3/CX] [Param4/DX] [Para1R/DI] [Para2R/SI] [Para3R/ES] [Para4R/AX]
HASP OUT [28] C8 02 00 00 02 00 00 00 AD BA 00 00 00 00 00 00 EF BE 00 00 AD DE 00 00 01 00 00 00 01 00 00 00 00 00 00 00 AD BA 00 00
HASP IN  [28] C8 02 00 00 02 00 02 00 34 12 00 00 78 56 00 00 BC 9A 00 00 F0 DE 00 00 34 12 00 00 78 56 00 00 BC 9A 00 00 F0 DE 00 00
```

You now take Param1-Param4 reply, that would be the following 16 bytes:
`34 12 00 00 78 56 00 00 BC 9A 00 00 F0 DE 00 00`

So you create a registry value `EmulateParams` with these 16 bytes under 
`HKEY_LOCAL_MACHINE\System\CurrentControlSet\Services\HaspNt\Parameter`,
set `LegacyVDDInterface`to 2 and if will start emulating it. 
To end emulation, delete the keys.

```
+--------------------+-----------+------------------------------------------+
| Key Name           | Type      | Value                                    |
+------------------- +-----------+------------------------------------------+
| EmulateParams      | REG_BINARY| Binary dump of Param1..4 response from   |
|                    |           | Dongle call 2 to respond with. Has to be |
|                    |           | exactly 16 bytes long (4 * 4)            |
+--------------------+-----------+------------------------------------------+
| LegacyVDDInterface | REG_DWORD | 2                                        |
+--------------------+-----------+------------------------------------------+
```

#### The UCLHASP emulator interface

As mentioned in the introduction, it's always desirable to do as much in 
usermode as possible and not having to use kernel mode drivers saves you from 
potential problems. Therefore it is preferrable to do a usermode dongle 
emulation for dongles that just use simple functions and where the UCLHASP 
emulator may be enough.

For this reason, the HASPVDD driver supports the use of a "plugin" DLL
that offers emulation functions. The driver just has to export the function 
`CallHardlock` that simply gets called whenever there is a call from the DOS 
driver and is expected to handle it accordingly. For a prototype definition,
see [uclhasp.c](uclhasp/uclhasp.c).
The emulator registers itself with HASPVDD by creating the registry value 
`EmulatorDLL` with a path to the driver under 
`HKEY_LOCAL_MACHINE\System\CurrentControlSet\Services\HaspNt\Parameter`,
therefore to use uclhasp (given that uclhasp.dll is present in the 
Windows\SysWOW64 directory), the following entry has to be made:

```
+--------------------+-----------+------------------------------------------+
| Key Name           | Type      | Value                                    |
+------------------- +-----------+------------------------------------------+
| EmulatorDLL        | REG_SZ    | uclhasp.dll                              |
+--------------------+-----------+------------------------------------------+
```

To unregister the emulator, just remove the key again and then it will talk
to the normal HASP driver again.

The UCLHASP driver searches for dongle key definitions in the following
registry paths:

	HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Services\Emulator\Hasp\Dump
	HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\MultiKey\Dumps
	HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Mkbus

The key format is like the one specified for these emulators, so it should
work out of the box.

Citing the UCLHASP documentation (as an example):

All dumps are contained in the registry along the path
`HKEY_LOCAL_MACHINE\System\CurrentControlSet\Services\Emulator\HASP\Dump\xxxxxxxx`
where xxxxxxxx - key opening passwords (must be 8 digits!).

DemoMA key registry example (3c39:25a0)

```
REGEDIT4

[HKEY_LOCAL_MACHINE\System\CurrentControlSet\Services\Emulator\HASP\Dump\3c3925a0]
"Name"="DemoMA dongle"
"Internal Name"="[DemoMA]"
"SN"=dword:DEADBEEF
"pwd"=dword:3c3925a0
"Type"=dword:01
"Memory"=dword:01
"Data"=hex:ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff, \
  ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff, \
  ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff, \
  ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff, \
  ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff,ff
"Created"="12/12/1998 6:59 PM"
"Copyright"="(c)1998 by MeteO //UCL dongle labs"
"Expired"=dword:1dae282
"CRC"=dword:00000000
```
Main fields used:

```
SN   - key serial number
Pwd  - open key passwords
Data - contents of the dongle memory
```
 
#### Summary of registry settings 

These have to go into the following Registry path:
`HKEY_LOCAL_MACHINE\System\CurrentControlSet\Services\HaspNt\Parameter`

```
+--------------------+-----------+-------------------------------------------+
| Key Name           | Type      | Description                               |
+--------------------+-----------+-------------------------------------------+
| LegacyVDDInterface | REG_DWORD | Val | Description                         |
|                    |           | --- | ----------------------------------- |
|                    |           | 0   | Use IOCTL based interface           |
|                    |           | 1   | Use Legacy ReadFile based interface |
|                    |           | 2   | Emulate calls only (EmulateParams)  |
+--------------------+-----------+-------------------------------------------+
| ErrorReportingMode | REG_DWORD | Val | Description                         |
|                    |           | --- | ----------------------------------- |
|                    |           | 0   | Show MessageBox on fatal error      |
|                    |           | 1   | Write them only to Debug Console    |
|                    |           | 2   | Do not display them at all          |
+--------------------+-----------+-------------------------------------------+
| LogFile            | REG_SZ    | Specify path and filename of the file to  |
|                    |           | log all dongle traffic to. The file can   |
|                    |           | then be fed into dumplog to show the calls|
+--------------------+-----------+-------------------------------------------+
| EmulateParams      | REG_BINARY| Binary dump of Param1..4 response from    |
|                    |           | Dongle call 2 to respond with. Has to be  |
|                    |           | exactly 16 bytes long (4 * 4)             |
|                    |           | Only applies to LegacyVDDInterface = 2    |
+--------------------+-----------+-------------------------------------------+
| EmulatorDLL        | REG_SZ    | DLL to register with driver to do dongle  |
|                    |           | Emulation. Disables normal operation and  |
|                    |           | dispatches every call to emulator DLL.    |
|                    |           | Do NOT confuse with LegacyVDDInterface=2! |
+--------------------+-----------+-------------------------------------------+
```

#### Build configurations notes 

As you may have realized, the HASPVDD.DLL driver has a `Release` and a 
`Release64` build configuration setting, but only allows x32 architecture. The
reason for this is pretty clear: As NTVDM is always a 32bit application, even 
NTVDMx64 on 64bit machines, it expects all the modules it loads to also be 
32bit modules. Now why is there a `Release64` configuration then? As previously
mentioned, the underlying HARDLOCK.SYS driver on a 64bit system is 64bit and 
therefore it expects the internal buffer to contain 2 64bit values in the end.
See the `BufferOffset` and `BufferSegment` members of the `HaspWinBufferStruc`
in [haspintl.h](haspvdd/haspintl.h).
So you need a different HASPVDD.DLL on 64bit than on 32bit.
However this isn't really a problem, if you just use the HASPVDD.DLL driver 
for monitoring/sniffing HASP traffic on a 32bit system, as you have to use 
the ReadFile based I/O interface to interact with the original HASPNT.SYS 
driver anyway and thus, HASPVDD.DLL doesn't generate any internal packages 
itself, but relies on the underlying HASPNT.SYS driver to generate them.
So, when you build this driver, use the `Release64` configuration and you 
should be fine. Should you want to not use the original HASPNT.SYS on a 32bit 
system, but use our HASPNT64.SYS (it actually IS possible to compile it for 
x86 and maybe you want to use it due to stability issues that can be seen 
with the original driver), you have to take care of the different build 
configurations for HASPVDD.DLL, if you want to use it in conjunction with 
our own HASPVDD.DLL. But be aware, that we ONLY support DOS applications!

Regarding the UCLHASP driver, it contains the ASM source in 
[asm\src](uclhasp/asm/src). The source can only be compiled with TASM, not
with MASM, therefore the .asm files cannot be directly linked with Visual
Studio, which would use integrated MASM. So the object files are linked to the
project instead. To generate the object files, use TASM. However TASM outputs
OMF object format, not COFF and newer Microsoft linkers and even the editbin
utility in more recent Visual Studio Versions (like VS2019) do not seem to 
support OMF anymore, no idea, why that got removed. So in order to link the
.obj files with newer VS versions, they first need to be converted to COFF 
via Visual Studio 6 version of editbin which still supoprts OMF.
The MAKE.BAT in asm-Directory does that for you. To make the project 
compilable out of the box for you, the repository therefore contains .obj 
files of UCLHASP.


### The HASPNT64 driver 

The driver should theoretically also expose the same ReadFile based interface 
as the original HASPNT.SYS driver has, but it has not been thouroughly tested.
It poses the risk of data processing in kernel and thus should be avoided in
favour of the IOCTL based interface that our HASPVDD.DLL driver uses per 
default.
The driver is listening for the `HASP_IOCTL_DOS_DISPATCH` IOCTL and dispatches
the call via `IRP_MJ_INTERNAL_DEVICE_CONTROL` to the HARDLOCK.SYS driver.
Be aware that on 64bit systems, the underlying HARDLOCK.SYS driver expects
its previously documented internal structure with 2 pointers as 64bit values,
so if using this interface, the upper-level HASPVDD.DLL has to take care of 
this (see previous chapter).
Be aware that the HASPNT64.SYS driver is not identical to the original 
HASPNT.SYS driver, which exposes a lot more interfaces to the callers. So 
the driver really is just meant to be used for DOS support, other callers that
rely on different HASPNT.SYS interface or even packet sizes won't work with
it. It is possible to compile and run this driver on a x86 32bit system, for
details see previous chapter.

The driver requires to have a working 64bit HARDLOCK.SYS driver on the system,
see Prerequisites section from installation instructions for details. 

## Conclusion

With this package, you get a well-documented example for the workflow of 
HASP dongles for applications running on DOS on Windows NTVDM. These drivers
allow you to run HASP Dongle protected applications on NTVDMx64 on 64bit 
Windows the same as they would run natively on 32bit Windows and is therefore
a useful  extension for NTVDMx64 in order to run Dongle-Protected commercial
DOS applications.

## Contact

For questions, please use the Issue Tracker or other Github facilities.
