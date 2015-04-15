#include "analyzer.h"
#include "kmp.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 使用说明
void useage()
{
	printf("useage: apkanalyzer [-apktool] [-apkpath]\r\n");
}

// 萃取出实参
char* get_param(const char* arg)
{
	static char param[256];
	const char* beginp = arg;
	const char* endp = arg + strlen(arg);
	
	while (*beginp++ != '=');
	if (*beginp == '"')
	{
		beginp++;
	}
	while (*endp-- == '"');

	memcpy(param, beginp, endp - beginp + 1);
	param[endp - beginp + 1] = '\0';

	return param;
}

// 点乐广告
int analyzer_callback_dianjoy(const char* path, const void* data, 
							  int size, void* userp)
{
	const char* key = "http://a.diXanjoTy.com/dev/";
	int n = -1;

	if (size > 0)
	{
		int next[128];
		get_next(key, next);
		n = kmp_search((const char*)data, key, next);
	}
	
	return n == -1;
}

// 回调信息
struct analyzer_callback_info 
{
	const char* name;
	analyzer_callback* cb;
	void* userp;
};

// 若干回调信息
static struct analyzer_callback_info cis[] = {
	{"点乐广告", analyzer_callback_dianjoy, NULL},
	{NULL, NULL, NULL}
};

// 主函数
int main(int argc, char** argv)
{
	struct analyzer* analyzer = NULL;
	char* apktool = NULL;
	char* apkpath = NULL;
	int i = 0;

	if (argc < 3)
	{
		useage();
		return -1;
	}

	while (argc--)
	{
		if (!strncmp(argv[argc], "-apktool", strlen("-apktool")))
		{
			apktool = strdup(get_param(argv[argc]));
		}
		else if (!strncmp(argv[argc], "-apkpath", strlen("-apkpath")))
		{
			apkpath = strdup(get_param(argv[argc]));
		}
	}

	if (!apkpath || !apktool)
	{
		useage();
		goto ret;
	}

	analyzer = analyzer_new(apktool);
	if (!analyzer)
	{
		fprintf(stderr, "analyzer_new\r\n");
		return -1;
	}
		
	for (i = 0; cis[i].cb; i++)
	{
		int ok = !analyzer_execute(analyzer, apkpath, cis[i].cb, NULL);
		printf("[%s]%s\r\n", ok ? "√" : "×", cis[i].name);
	}

ret:
	if (analyzer)
	{
		analyzer_free(analyzer);
	}
	if (apktool)
	{
		free(apktool);
	}
	if (apkpath)
	{
		free(apkpath);
	}

	return 0;
}
