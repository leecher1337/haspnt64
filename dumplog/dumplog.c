#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <stdio.h>
#include "log.h"

int main(int argc, char** argv)
{
	FILE* fp;
	int i;
	LOG_HDR hdr;
	const char* m_pszWhat[3] = { LOGTYPE_STR };

	if (argc < 2)
	{
		fprintf(stderr, "Usage: %s <Logfile>\n", argv[0]);
		return -1;
	}

	if (!(fp = fopen(argv[1], "rb")))
	{
		perror("Cannot open log file");
		return -1;
	}

	printf("                    %s\n", LOG_HEADER);
	while (fread(&hdr, sizeof(hdr), 1, fp) == 1)
	{
		printf ("%02d.%02d.%04d %02d:%02d:%02d.%04d %s [%02X] ", 
			hdr.st.wDay, hdr.st.wMonth, hdr.st.wYear, 
			hdr.st.wHour, hdr.st.wMinute, hdr.st.wSecond, hdr.st.wMilliseconds,
			m_pszWhat[hdr.Direction], hdr.Length);
		for (i = 0; i < hdr.Length; i++) printf("%02X ", (UCHAR)fgetc(fp));
		printf("\n");
	}
	printf("\n");
	return 0;
}
