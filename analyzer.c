#include "analyzer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef WIN32

#include <Windows.h>

#ifndef snprintf
#define snprintf _snprintf
#endif

#define PATH_SEPARATOR '\\'

#else

#define PATH_SEPARATOR '/'

#endif

struct analyzer
{
	char* apktool;
};

struct analyzer* analyzer_new(const char* apktool)
{
	struct analyzer* analyzer = (struct analyzer*)malloc(sizeof(*analyzer));
	if (analyzer)
	{
		analyzer->apktool = strdup(apktool);
	}

	return analyzer;
}

void analyzer_free(struct analyzer* analyzer)
{
	if (analyzer)
	{
		free(analyzer->apktool);
		free(analyzer);
	}
}

#ifdef WIN32

int execute_apktool(const char* path, const char* args)
{
	STARTUPINFOA si;
	PROCESS_INFORMATION pi;
	char cmd[MAX_PATH];
	BOOL ret;

	memset(&pi, 0, sizeof(pi));
	memset(&si, 0, sizeof(si));
	si.cb = sizeof(si);
	snprintf(cmd, sizeof(cmd), "cmd /C %s %s", path, args);

	ret = CreateProcessA(NULL, cmd, NULL, NULL, TRUE, 
		DETACHED_PROCESS, NULL, NULL, &si, &pi);
	WaitForSingleObject(pi.hProcess, INFINITE);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	return ret ? 0 : 1;
}

int enum_files(const char* dir, analyzer_callback* cb, void* userp)
{
	HANDLE h;
	WIN32_FIND_DATAA fd; 
	char find[MAX_PATH]; 
	int n = 1;

	strcpy(find, dir); 
	strcat(find, "\\*.*");
	h = FindFirstFileA(find, &fd); 
	if (h == INVALID_HANDLE_VALUE)
	{
		return -1;
	}

	while (1) 
	{ 
		if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) 
		{ 
			if (fd.cFileName[0] != '.') 
			{ 
				strcpy(find, dir); 
				strcat(find, "\\"); 
				strcat(find, fd.cFileName); 
				if (!enum_files(find, cb, userp))
				{
					n = 0;
					break;
				}
			}
		} 
		else 
		{ 
			FILE* fp;
			char path[MAX_PATH];
			snprintf(path, sizeof(path), "%s\\%s", dir, fd.cFileName);
			fp = fopen(path, "rb");
			if (fp)
			{
				int size;
				void* data = NULL;

				fseek(fp, 0, SEEK_END);
				size = ftell(fp);
				fseek(fp, 0, SEEK_SET);
				if (size)
				{
					data = malloc(size);
					fread(data, 1, size, fp);
				}
				n = (*cb)(path, data, size, userp);
				if (data)
				{
					free(data);
				}
				fclose(fp);
				if (!n)
				{
					break;
				}
			}
		} 

		if (!FindNextFileA(h, &fd))
		{
			break;
		}
	}
	FindClose(h);

	return n;
}

#else

int execute_apktool(const char* path, const char* args)
{
	return 1;
}

int enum_files(const char* dir, analyzer_callback* cb, void* userp)
{
	return 1;
}

#endif

int analyzer_execute(struct analyzer* analyzer, const char* path, 
					 analyzer_callback* cb, void* userp)
{
	int len;
	char buffer[256];
	snprintf(buffer, sizeof(buffer), "d %s", path);
	if (execute_apktool(analyzer->apktool, buffer))
	{
		return 1;
	}

	len = strlen(path);
	while (len && path[len] != '.')
	{
		len--;
	}
	memcpy(buffer, path, len);
	buffer[len] = '\0';
	snprintf(buffer, sizeof(buffer), "%s%csmali", buffer, PATH_SEPARATOR);

	return enum_files(buffer, cb, userp);
}
