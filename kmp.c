#include "kmp.h"

#include <string.h>

void get_next(const char* str, int next[])
{
	int i = 0; 
	int j = -1; 
	int len = strlen(str);

	next[0] = -1;  
	while (i < len - 1)  
	{  
		if (j == -1 || str[i] == str[j])  
		{  
			i++;  
			j++;  
			if (str[i] != str[j])
			{
				next[i] = j;
			}
			else
			{
				next[i] = next[j];
			}
		}  
		else  
		{  
			j = next[j];  
		}  
	}  
}

int kmp_search(const char* str, const char* key, int next[])
{
	int i = 0;  
	int j = 0;  
	int str_len = strlen(str);  
	int key_len = strlen(key); 

	while (i < str_len && j < key_len)  
	{     
		if (j == -1 || str[i] == key[j])  
		{  
			i++;  
			j++;  
		}  
		else  
		{  
			j = next[j];  
		}  
	}  
	if (j == key_len)
	{
		return i - j;
	}
	
	return -1;  
}
