/*
 **********************************************************
 * OLDDEFS.H - Old service macros supplied to provide
 * compatibility with older releases of the HASP API.
 *
 **********************************************************
 */
#ifndef OLDDEFS_H_
#define OLDDEFS_H_

#if !defined(HASPAPI_H_)
#error Do not include OLDDEFS.H directly. Include HASP.H instead
#endif

   /*
    *  Services common to all local HASPs.
    */
#define IS_HASP                   LOCALHASP_ISHASP
#define GET_HASP_CODE             LOCALHASP_HASPCODE
#define GET_HASP_STATUS           LOCALHASP_HASPSTATUS

   /*
    *  HASP-3, MemoHASP, HASP36, and MemoHASP36 services.
    */
#define READ_HASP_MEMO            MEMOHASP_READMEMO
#define WRITE_HASP_MEMO           MEMOHASP_WRITEMEMO
#define GET_HASP_IDNUM            MEMOHASP_HASPID
#define READ_HASP_MEMO_BLOCK      MEMOHASP_READBLOCK
#define WRITE_HASP_MEMO_BLOCK     MEMOHASP_WRITEBLOCK

   /*
    *  TimeHASP memory services.
    */
#define READ_TIME_MEMO            TIMEHASP_READMEMO
#define WRITE_TIME_MEMO           TIMEHASP_WRITEMEMO
#define GET_TIME_IDNUM            TIMEHASP_HASPID
#define READ_TIME_MEMO_BLOCK      TIMEHASP_READBLOCK
#define WRITE_TIME_MEMO_BLOCK     TIMEHASP_WRITEBLOCK

   /*
    *  TimeHASP clock services.
    */
#define SET_TIME                  TIMEHASP_SETTIME
#define GET_TIME                  TIMEHASP_GETTIME
#define SET_DATE                  TIMEHASP_SETDATE
#define GET_DATE                  TIMEHASP_GETDATE

   /*
    *  NetHASP network services.
    */
#define CHECK_LAST_STATUS         NETHASP_LASTSTATUS
#define GET_NETHASP_CODE          NETHASP_HASPCODE
#define SET_IDLE_TIME             NETHASP_SET_IDLETIME


   /*
    *  NetHASP memory services.
    */
#define READ_NETHASP_MEMO         NETHASP_READMEMO
#define WRITE_NETHASP_MEMO        NETHASP_WRITEMEMO
#define GET_NETHASP_IDNUM         NETHASO_HASPID
#define READ_NETHASP_MEMO_BLOCK   NETHASP_READBLOCK
#define WRITE_NETHASP_MEMO_BLOCK  NETHASP_WRITEBLOCK


#endif /* OLDDEFS_H_ */

