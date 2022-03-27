/*  $Id: haspapi.h,v 1.1 2002/07/26 16:56:14 axel Exp $
 *
 *  HASPAPI.H - Service constants and declaration of the hasp() routine.
 *  Copyright (c) 2002 Aladdin Knowledge Systems Ltd.
 */

#if !defined (HASPAPI_H_)
#define HASPAPI_H_

#ifndef DATAFAR_
#  ifdef DOS
#    ifdef __386__
#      define DATAFAR_
#      define CODEFAR_
#      ifndef __32BIT__
#         define __32BIT__
#      endif
#    else
#      ifndef __16BIT__
#        define __16BIT__
#      endif
#      define DATAFAR_ far
#      define CODEFAR_ far
#    endif
#    define CONST_ const
#  else
#    define CODEFAR_
#    define DATAFAR_
#    define CONST_
#  endif
#endif

/*
 *********************************************
 *
 *  The Function hasp()
 *
 *********************************************
 */
#ifdef __cplusplus
  extern "C" {
#endif

#ifdef HASP_DLL
#  define HASP_API_ __stdcall
#else
#  define HASP_API_ CODEFAR_
#endif

void HASP_API_ hasp( int service,
                   int seed,
                   int lptnum,
                   int pass1,
                   int pass2,
                   int DATAFAR_ *p1,
                   int DATAFAR_ *p2,
                   int DATAFAR_ *p3,
                   int DATAFAR_ *p4 );

#ifdef __cplusplus
  }
#endif

/*
 ******************************
 *
 *  HASP Services
 *
 ******************************
 */
enum {
   /*
    *   Services common to all local keys. HASP3, MEMOHASP, TIMEHASP.
    */
   LOCALHASP_ISHASP         = 1,
   LOCALHASP_HASPCODE       = 2,
   LOCALHASP_HASPSTATUS     = 5,
   HASPAPI_VERSION          = 5,
   LOCALHASP_HASPGENERATION = 8,
   LOCALHASP_HASPNETSTATUS  = 9,

   /*
    *   Services supported by HASP4.
    */
   LOCALHASP_ENCODEDATA =  60,
   LOCALHASP_DECODEDATA =  61,
   NETHASP_ENCODEDATA   =  88,
   NETHASP_DECODEDATA   =  89,

   /*
    *   MemoHASP Memory services.
    */
   MEMOHASP_READMEMO    =  3,
   MEMOHASP_WRITEMEMO   =  4,
   MEMOHASP_HASPID      =  6,
   MEMOHASP_READBLOCK   =  50,
   MEMOHASP_WRITEBLOCK  =  51,

   /*
    *   TimeHASP clock services.
    */
   TIMEHASP_SETTIME     =  70,
   TIMEHASP_GETTIME     =  71,
   TIMEHASP_SETDATE     =  72,
   TIMEHASP_GETDATE     =  73,

   /*
    *   TimeHASP memory services.
    */
   TIMEHASP_READMEMO   =   75,
   TIMEHASP_READBYTE   =   75,
   TIMEHASP_WRITEMEMO  =   74,
   TIMEHASP_WRITEBYTE  =   74,
   TIMEHASP_HASPID     =   78,
   TIMEHASP_READBLOCK  =   77,
   TIMEHASP_WRITEBLOCK =   76,

   /*
    *   NetHASP network services.
    */
   NETHASP_LASTSTATUS  =   40,
   NETHASP_HASPCODE    =   41,
   NETHASP_LOGIN       =   42,
   NETHASP_LOGOUT      =   43,
   NETHASP_LOGIN_PROCESS=  110,
   NETHASP_SETIDLETIME =   48,

   /*
    *   NetHASP memory services.
    */
   NETHASP_READMEMO    =   44,
   NETHASP_WRITEMEMO   =   45,
   NETHASP_HASPID      =   46,
   NETHASP_READBLOCK   =   52,
   NETHASP_WRITEBLOCK  =   53,

   /*
    *   NetHASP License Manager configuration services.
    */
   NETHASP_SET_CONFIG_FILENAME =  85,
   NETHASP_SET_SERVER_BY_NAME  =  96,
   NETHASP_QUERY_LICENSE       =  104,
   NETHASP_GETPROTOCOL         =  120,
   NETHASP_SETPROTOCOL         =  121,
   NETHASP_DEFAULTCONFIG       =  125
};


/*
 ************************************
 *
 *  Miscellaneous HASP API constants
 *
 ************************************
 */
enum {
   /*
    *  LPT numbers for HASP and HASP36 on PC and NEC.
    */
   HASP25     = 0,
   HASP36     = 50,
   HASP36NEC  = 60,

   /*
    *  Logical LPT numbers for USB.
    */
   USBHASP_DEFAULT = 200,
   USBHASP_MIN = 201,
   USBHASP_MAX = 255,

   /*
    *  Capacity in bytes of each HASP type
    */
   MEMOHASP1_CAPACITY = 112,
   MEMOHASP4_CAPACITY = 496,
   TIMEHASP_CAPACITY  = 16,
   NETHASP_CAPACITY   = 496,

   /*
    *  HASP type constants as returned in p2 for HaspStatus service
    */
   HASPTYPE_HASP3     = 0,
   HASPTYPE_MEMOHASP  = 1,
   HASPTYPE_TIMEHASP  = 3,
   HASPTYPE_TIMEHASP4 = 5
};



/*
 *************************************************
 *
 *  HASP Errors
 *  ~~~~~~~~~~~
 *  Some symbols use the following abbreviations:
 *
 *  HDD    HASP Device Driver
 *  MH     MemoHASP
 *  TH     TimeHASP
 *  NH     NetHASP
 *  NHLM   NetHASP License Manager
 *  NHCF   NetHASP Configuration File
 *  SSBN   Set Server By Name
 *
 **************************************************
 */
enum {
  HASPERR_SUCCESS =  0,                        /* Operation successful                                                           */
  HASPERR_INVALID_SERVICE = -999,              /* HASP Invalid Service                                                           */
  HASPERR_CANT_FREE_DOSMEM = -121,             /* Cannot free DOS Memory                                                         */
  HASPERR_CANT_ALLOC_DOSMEM = -120,            /* Cannot allocate DOS Memory                                                     */
  HASPERR_DOS_CANT_CLOSE_HDD = -112,           /* Cannot close the HASP Device Driver                                            */
  HASPERR_DOS_CANT_READ_HDD = -111,            /* Cannot read the HASP Device Driver                                             */
  HASPERR_DOS_CANT_OPEN_HDD = -110,            /* Cannot open the HASP Device Driver                                             */
  HASPERR_CANT_CLOSE_HDD = -102,               /* Cannot close the HASP Device Driver                                            */
  HASPERR_CANT_READ_HDD = -101,                /* Cannot read the HASP Device Driver                                             */
  HASPERR_CANT_OPEN_HDD = -100,                /* Cannot open the HASP Device Driver                                             */
  HASPERR_NOT_A_TIMEHASP = -29,                /* A HASP was found but it is not a TimeHASP                                      */
  HASPERR_TH_INVALID_PASSWORDS = -28,          /* A HASP with specified passwords was not found                                  */
  HASPERR_TH_TIMEOUT = -27,                    /* Timeout - Write operation failed                                               */
  HASPERR_TH_INVALID_ADDRESS = -26,            /* Invalid address - Address is not in 0 - 15                                     */
  HASPERR_TH_INVALID_HOUR = -25,               /* Invalid Hour                                                                   */
  HASPERR_TH_INVALID_MINUTE = -24,             /* Invalid Minute                                                                 */
  HASPERR_TH_INVALID_SECOND = -23,             /* Invalid Second                                                                 */
  HASPERR_TH_INVALID_YEAR = -22,               /* Invalid year                                                                   */
  HASPERR_TH_INVALID_MONTH = -21,              /* Invalid month                                                                  */
  HASPERR_TH_INVALID_DAY = -20,                /* Invalid day                                                                    */
  HASPERR_VERSION_MISMATCH = -13,              /* Driver version mismatch                                                        */
  HASPERR_INVALID_PARAMETER = -12,             /* Invalid parameter                                                              */
  HASPERR_TS_SP3_FOUND = -11,                  /* Terminal Server under SP3 is not supported                                     */
  HASPERR_TS_FOUND = -10,                      /* Terminal Server was found.                                                     */
  HASPERR_INVALID_POINTER = -9,                /* Invalid pointer used by Encode Data                                            */
  HASPERR_HARDWARE_NOT_SUPPORTED = -8,         /* The hardware does not support the service                                      */
  HASPERR_DATA_TOO_SHORT = -7,                 /* The data length is too short                                                   */
  HASPERR_PORT_BUSY = -6,                      /* Parallel port is busy.                                                         */
  HASPERR_MH_WRITE_FAIL = -5,                  /* Unsuccessful Write operation                                                   */
  HASPERR_NOT_A_MEMOHASP = -4,                 /* A HASP was found but it is not a MemoHASP                                      */
  HASPERR_MH_INVALID_PASSWORDS = -3,           /* A HASP with specified passwords was not found                                  */
  HASPERR_HASP_NOT_FOUND       = -3,           /* A HASP with specified passwords was not found                                  */
  HASPERR_MH_INVALID_ADDRESS = -2,             /* Address out of range                                                           */
  HASPERR_MH_TIMEOUT = -1,                     /* Timeout - Write operation failed                                               */
  HASPERR_NO_PROTOCOLS = 1,                    /* IPX, NetBIOS, or TCP/IP protocols have not been installed properly.            */
  HASPERR_NO_SOCKET_NUMBER = 2,                /* Communication Error - unable to get the socket number (TCP/IP, IPX)            */
  HASPERR_COMM_ERROR = 3,                      /* Communication Error.                                                           */
  HASPERR_NO_NHLM = 4,                         /* No NetHASP License Manager was found.                                          */
  HASPERR_NO_NHLM_ADDRFILE = 5,                /* Cannot read NetHASP License Manager address file                               */
  HASPERR_CANT_CLOSE_NHLM_ADDRFILE = 6,        /* Cannot close NetHASP License Manager address file                              */
  HASPERR_CANT_SEND_PACKET = 7,                /* Communication error - failed to send packet (IPX, NetBIOS                      */
  HASPERR_SILENT_NHLM = 8,                     /* No answer from the NetHASP License Manager.                                    */
  HASPERR_NO_LOGIN = 10,                       /* Service requested before LOGIN                                                 */
  HASPERR_ADAPTER_ERROR = 11,                  /* NetBIOS: Communication error - adapter error                                   */
  HASPERR_NO_ACTIVE_NHLM = 15,                 /* No active NetHASP Licence Manager was found                                    */
  HASPERR_SSBN_FAILED = 18,                    /* Cannot perform LOGIN - SetServerByName failed                                  */
  HASPERR_NHCF_SYNTAX_ERROR = 19,              /* NetHASP configuration file syntax error                                        */
  HASPERR_NHCF_GENERIC_ERROR = 20,             /* Error handling NetHASP configuration file                                      */
  HASPERR_NH_ENOMEM = 21,                      /* Memory allocation error                                                        */
  HASPERR_NH_CANT_FREE_MEM = 22,               /* Memory release error                                                           */
  HASPERR_NH_INVALID_ADDRESS = 23,             /* Invalid NetHASP memory address                                                 */
  HASPERR_NH_ENCDEC_ERR = 24,                  /* Error trying to Encrypt/Decrypt                                                */
  HASPERR_CANT_LOAD_WINSOCK = 25,              /* TCP/IP: failed to load WINSOCK.DLL                                             */
  HASPERR_CANT_UNLOAD_WINSOCK = 26,            /* TCP/IP: failed to unload WINSOCK.DLL                                           */
  HASPERR_WINSOCK_ERROR = 28,                  /* TCP/IP: WINSOCK.DLL startup error                                              */
  HASPERR_CANT_CLOSE_SOCKET = 30,              /* TCP/IP: Failed to close socket.                                                */
  HASPERR_SETPROTOCOL_FAILED = 33,             /* SetProtocol service requested without performing LOGOUT                        */
  HASPERR_NH_NOT_SUPPORTED = 40,               /* NetHASP services are not supported                                             */
  HASPERR_NH_HASPNOTFOUND = 129,               /* NetHASP key is not connected to the NetHASP Licence Manager                    */
  HASPERR_INVALID_PROGNUM = 130,               /* Program Number is not in the Program List of the NetHASP memory                */
  HASPERR_NH_READ_ERROR = 131,                 /* Error reading NetHASP memory                                                   */
  HASPERR_NH_WRITE_ERROR = 132,                /* Error writing NetHASP memory                                                   */
  HASPERR_NO_MORE_STATIONS = 133,              /* Number of stations exceeded                                                    */
  HASPERR_NO_MORE_ACTIVATIONS = 134,           /* Number of activations exceeded                                                 */
  HASPERR_LOGOUT_BEFORE_LOGIN = 135,           /* LOGOUT called without LOGIN                                                    */
  HASPERR_NHLM_BUSY = 136,                     /* NetHASP Licence Manager is busy                                                */
  HASPERR_NHLM_FULL = 137,                     /* No space in NetHASP Log Table                                                  */
  HASPERR_NH_INTERNAL_ERROR = 138,             /* NetHASP Internal error                                                         */
  HASPERR_NHLM_CRASHED = 139,                  /* NetHASP Licence Manager crashed and reactivated                                */
  HASPERR_NHLM_UNSUPPORTED_NETWORK = 140,      /* NetHASP Licence Manager does not support the station's network                 */
  HASPERR_NH_INVALID_SERVICE_2 = 141,          /* Invalid Service NetHASP Licence Manager does not support the station's network */
  HASPERR_NHCF_INVALID_NHLM = 142,             /* Invalid NetHASP Licence Manager name in configuration file                     */
  HASPERR_SSBN_INVALID_NHLM = 150,             /* Invalid NetHASP Licence Manager name in SetServerByName                        */
  HASPERR_ENC_ERROR_NHLM = 152,                /* Error trying to encrypt by the LM                                              */
  HASPERR_DEC_ERROR_NHLM = 153,                /* Error trying to decrypt by the LM                                              */
  HASPERR_OLD_LM_VERSION_NHLM = 155            /* LM old version was found                                                       */
};


/*
 ***************************
 *
 *  NetHASP Warnings
 *
 ***************************
 */
enum {
  HASPERR_IPX_UNAVAILABLE = 1,                 /* IPX is enabled but it is not installed                                         */
  HASPERR_NETBIOS_UNAVAILABLE = 2,             /*  NetBIOS is enabled but it is not installed                                    */
  HASPERR_IPX_NETBIOS_UNAVAILABLE = 3,         /*  IPX and NetBIOS are enabled but both are not installed                        */
  HASPERR_TCPIP_UNAVAILABLE = 4,               /* TCP/IP is enabled but it is not installed                                      */
  HASPERR_IPX_TCPIP_UNAVAILABLE = 5,           /*  IPX and TCP/IP are enabled but both are not installed                         */
  HASPERR_TCPIP_NETBIOS_UNAVAILABLE = 6,       /* TCP/IP and NetBIOS are enabled but are not installed                           */
  HASPERR_PROTOCOLS_UNAVAILABLE = 7,           /* IPX, NetBIOS and TCP/IP are enabled but not installed                          */
  HASPERR_SUSPICIOUS_LOGOUT = 18,              /* Suspicious LOGOUT                                                              */
  HASPERR_NHCF_BAD_TOKEN = 19,                 /* Invalid keyword or value in configuration file                                 */
  HASPERR_MISSING_IP_ADDR = 20,                /* TCP or UDP were not employed due to missing IP address                         */
  HASPERR_CANT_FREE_MEM = 22                   /* HASP API cannot free memory                                                    */
};


/*
 * Include old symbols for compatibility with older releases
 * of the HASP API.
 */
#include "olddefs.h"

#endif /* include guard */
