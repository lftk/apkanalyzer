#ifndef ANALYZER__H
#define ANALYZER__H

struct analyzer;

typedef int (analyzer_callback)(const char* path, const void* data, 
								int size, void* userp);

// 创建一个分析器
struct analyzer* analyzer_new(const char* apktool);

// 销毁一个分析器
void analyzer_free(struct analyzer* analyzer);

// 执行分析过程，返回0成功
int analyzer_execute(struct analyzer* analyzer, const char* path, 
					 analyzer_callback* cb, void* userp);

#endif
