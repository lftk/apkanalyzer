#ifndef KMP__H
#define KMP__H

void get_next(const char* str, int next[]);

int kmp_search(const char* str, const char* key, int next[]);

#endif
