#pragma once

/*
Table 1|
-------┘
╔════════════════════Ð════════════════════Ð══════════════════════════════════╗
║       SERVICE      │       CALL         │             RETURN               ║
║  BH=FUNC,  BL=PORT │                    │                                  ║
╠════════════════════Ï════════════════════Ï══════════════════════════════════╣
║1.                  │                    │    AX : 0 - HASP NOT FOUND       ║
║      ISHASP        │                    │         1 - HASP FOUND           ║
Ã────────────────────┼────────────────────┼──────────────────────────────────Â
║2.                  │  AX=SEEDCODE       │    AX : 1ST RETURN CODE          ║
║     HASPCODE       │  CX=PASSWORD 1     │    BX : 2ND RETURN CODE          ║
║                    │  DX=PASSWORD 2     │    CX : 3RD RETURN CODE          ║
║                    │                    │    DX : 4TH RETURN CODE          ║
Ã────────────────────┼────────────────────┼──────────────────────────────────Â
║3.                  │  CX=PASSWORD 1     │    BX : DATA                     ║
║     READMEMO       │  DX=PASSWORD 2     │    CX : STATUS                   ║
║                    │  DI=MEMORY ADDR.   │    (READ 1 byte at DI)           ║
Ã────────────────────┼────────────────────┼──────────────────────────────────Â
║4.                  │  CX=PASSWORD 1     │    CX : STATUS                   ║
║     WRITEMEMO      │  DX=PASSWORD 2     │                                  ║
║                    │  DI=MEMORY ADDR.   │                                  ║
║                    │  SI=MEMORY DATA.   │                                  ║
Ã────────────────────┼────────────────────┼──────────────────────────────────Â
║5.                  │  CX=PASSWORD 1     │    AX : MEMORY SIZE              ║
║    HASPSTATUS      │  DX=PASSWORD 2     │    BX : HASP TYPE                ║
║                    │                    │    CX : ACTUAL LPT_NUM           ║
Ã────────────────────┼────────────────────┼──────────────────────────────────Â
║6.                  │  CX=PASSWORD 1     │    AX : IDLOW                    ║
║      HASPID        │  DX=PASSWORD 2     │    BX : IDHIGH                   ║
║                    │                    │    CX : STATUS                   ║
Ã────────────────────┼────────────────────┼──────────────────────────────────Â
║32h.                │  CX=PASSWORD 1     │    CX : STATUS                   ║
║     READBLOCK      │  DX=PASSWORD 2     │                                  ║
║                    │  DI=MEM.START ADDR.│                                  ║
║                    │  SI=BLOCK LENGTH   │                                  ║
║                    │  ES=BUFER SEG.     │                                  ║
║                    │  AX=BUFER OFFS.    │                                  ║
Ã────────────────────┼────────────────────┼──────────────────────────────────Â
║33h.                │  CX=PASSWORD 1     │    CX : STATUS                   ║
║     WRITEBLOCK     │  DX=PASSWORD 2     │                                  ║
║                    │  DI=MEM.START ADDR.│                                  ║
║                    │  SI=BLOCK LENGTH   │                                  ║
║                    │  ES=BUFER SEG.     │                                  ║
║                    │  AX=BUFER OFFS.    │                                  ║
╚════════════════════════════════════════════════════════════════════════════╝

Table 2|
-------┘
╔════════════════════════════════════════════════════════════════════════════╗
║                                  Port_Num                                  ║
╠═══════════Ð═════════Ð══════════Ð══════════Ð══════════Ð══════════Ð══════════╣
║     0     │    1    │     2    │     3    │   101    │   102    │   103    ║
Ã───────────┼─────────┼──────────┼──────────┼──────────┼──────────┼──────────Â
║ all ports │   LPT1  │   LPT2   │   LPT3   │   3BCh   │   378h   │   278h   ║
╚═══════════¤═════════¤══════════¤══════════¤══════════¤══════════¤══════════╝
*/

/* As VDDs are always 32bit (as NTVDM is), it makes no sense to compile it
 * 64bit. However, on 64bit, we interact with the 64bit HARDLOCK.SYS driver,
 * so the structures need to be adjusted for it
 */
#ifdef W64_BUILD
#define ULONG_PTR unsigned __int64 
#endif

#pragma warning(push)
#pragma warning(disable:4201)
#pragma pack(1)
typedef struct {			// Str	DOS  HLBuf
	USHORT Service;			// 00	04	20
	USHORT Service2;		// 02	06	22
	union {
		DWORD Param1;		// 04	08	24	(On input: HIBYTE() = 1 = ProtectedMode enabled?)
		DWORD AX_Ret;
	};
	union {
		DWORD Param2;		// 08	12	28
		DWORD BX;
	};
	union {
		DWORD Param3;		// 12	16	32
		DWORD CX;
	};
	union {
		DWORD Param4;		// 16	20	36
		DWORD DX;
	};
	union {
		DWORD Param1Ret;	// 20	24	40
		DWORD DI;
	};
	union {
		DWORD Param2Ret;	// 24	28	44
		DWORD SI;
	};
	union {
		DWORD Param3Ret;	// 28	32	48
		DWORD ES;
	};
	union {
		DWORD Param4Ret;	// 32	36	52
		DWORD AX;
	};
} HaspDOSBufferStruc;		// 36   40  56 

typedef struct {
	DWORD Ticks;			//      00    ??
	HaspDOSBufferStruc DOSBuffer;
} HaspBufferInStruc;        //      0x28

typedef struct {						// 32 64 bit
	DWORD Code1;						// 0
	DWORD Code2;						// 4
	DWORD Code3;						// 8
	DWORD PacketLength;					// 12
	DWORD InBufferLength;				// 16
	HaspDOSBufferStruc DOSBuffer;		// 20
	DWORD Padding;						// 56      Overflow area for DOSBuffer (sizeof(Ticks))
	DWORD BlockLength;					// 60
	ULONG_PTR BufferOffset;				// 64 64
	ULONG_PTR BufferSegment;			// 68 72
} HaspWinBufferStruc;					// 72 80

#pragma pack()

