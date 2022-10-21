#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include "log.h"
#include "haspvdd.h"
#include "haspcrypt.h"
#include "haspapi.h"


static void ShowPrefix(LOG_HDR* hdr, char emu)
{
	const char* m_pszWhat[3] = { LOGTYPE_STR };

	printf("%02d.%02d.%04d %02d:%02d:%02d.%04d%c%s [%02X] ",
		hdr->st.wDay, hdr->st.wMonth, hdr->st.wYear,
		hdr->st.wHour, hdr->st.wMinute, hdr->st.wSecond, hdr->st.wMilliseconds,
		emu, m_pszWhat[hdr->Direction], hdr->Length);
}

static BYTE *PrepareDataBuffer(FILE* fp, HaspBufferInStruc* IOBuffer, LOG_HDR *phdr)
{
	BYTE* pBuffer;
	int i;

	if (!fread(phdr, sizeof(LOG_HDR), 1, fp)) return NULL;
	if (phdr->Direction == 2 && (pBuffer = malloc(IOBuffer->DOSBuffer.SI * 2)))
	{
		if (fread(pBuffer, IOBuffer->DOSBuffer.SI * 2, 1, fp))
		{
			IOBuffer->DOSBuffer.AX = (DWORD)pBuffer;
			printf("Data buffer allocated at %08X\n", IOBuffer->DOSBuffer.AX);
			ShowPrefix(phdr, ' ');
			for (i = 0; i < phdr->Length; i++) printf("%02X ", pBuffer[i]);
			printf("\n");
			return pBuffer;
		}
		free(pBuffer);
	}
	fseek(fp, (long)sizeof(LOG_HDR) * -1, SEEK_CUR);
	return NULL;
	
}

int TestEmulator(char* pszEmuDLL, FILE* fp)
{
	fnHaspIOCtl pfunc;
	HMODULE hDLL = LoadLibrary(pszEmuDLL);
	BYTE buffer[65535], *pBuffer = NULL;
	int i;
	LOG_HDR hdr, hdrdata;
	DWORD dwRead;

	if (!hDLL)
	{
		fprintf(stderr, "Loading %s failed with %d\n", pszEmuDLL, GetLastError());
		return -1;
	}

	if (!(pfunc = (fnHaspIOCtl)GetProcAddress(hDLL, "CallHardlock")))
	{
		fprintf(stderr, "Emulator DLL doesn't contain correct dispatcher function\n");
		FreeLibrary(hDLL);
		return -1;
	}

	while (fread(&hdr, sizeof(hdr), 1, fp) == 1)
	{
		ShowPrefix(&hdr, ' ');
		fread(buffer, hdr.Length, 1, fp);
		for (i = 0; i < hdr.Length; i++) printf("%02X ", buffer[i]);
		printf("\n");
		if (hdr.Direction == 0)
		{
			HaspBufferInStruc* IOBuffer = (HaspBufferInStruc*)buffer;
			printf("Press any key to send...\n");
			_getch();
			switch (IOBuffer->DOSBuffer.Service)
			{
			case MEMOHASP_READBLOCK:
			case MEMOHASP_WRITEBLOCK:
			case TIMEHASP_READBLOCK:
			case TIMEHASP_WRITEBLOCK:
			case LOCALHASP_ENCODEDATA:
			case LOCALHASP_DECODEDATA:
				if (!(pBuffer = PrepareDataBuffer(fp, IOBuffer, &hdrdata)))
				{
					fprintf(stderr, "Prepare Buffer failed, skip...\n");
					continue;
				}
				break;
			}

			Encrypt28((PUSHORT)buffer, hdr.Length);
			if (pfunc((HaspBufferInStruc*)buffer, hdr.Length, &dwRead))
			{
				GetLocalTime(&hdr.st);
				hdr.Direction = 1;
				hdr.Length = (USHORT)dwRead;
				ShowPrefix(&hdr, 'E');
				Decrypt28((PUSHORT)buffer, hdr.Length);
				for (i = 0; i < hdr.Length; i++) printf("%02X ", buffer[i]);
				printf("\n");
			}
			if (pBuffer)
			{
				GetLocalTime(&hdrdata.st);
				ShowPrefix(&hdrdata, 'E');
				for (i = 0; i < hdrdata.Length; i++) printf("%02X ", pBuffer[i]);
				printf("\n");
				free(pBuffer);
				pBuffer = NULL;
			}
		}
		printf("-------------------------------------------------------------------------------\n");
	}
	printf("\n");

	FreeLibrary(hDLL);
	return 0;
}

int main(int argc, char** argv)
{
	FILE* fp;
	int i;
	LOG_HDR hdr;
	const char* m_pszWhat[3] = { LOGTYPE_STR };

	if (argc < 2)
	{
		fprintf(stderr, "Usage: %s <Logfile> [emulator.dll]\n\n" \
			"If emualtor.dll specified, sends data to emulator, otherwise just dumps to screen\n\n", argv[0]);
		return -1;
	}

	if (!(fp = fopen(argv[1], "rb")))
	{
		perror("Cannot open log file");
		return -1;
	}

	printf("                    %s\n", LOG_HEADER);
	if (argc > 2) TestEmulator(argv[2], fp);
	else
	{
		while (fread(&hdr, sizeof(hdr), 1, fp) == 1)
		{
			ShowPrefix(&hdr, ' ');
			for (i = 0; i < hdr.Length; i++) printf("%02X ", (UCHAR)fgetc(fp));
			printf("\n");
		}
		printf("\n");
	}
	fclose(fp);
	return 0;
}
