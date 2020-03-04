// pch.h: 这是预编译标头文件。
// 下方列出的文件仅编译一次，提高了将来生成的生成性能。
// 这还将影响 IntelliSense 性能，包括代码完成和许多代码浏览功能。
// 但是，如果此处列出的文件中的任何一个在生成之间有更新，它们全部都将被重新编译。
// 请勿在此处添加要频繁更新的文件，这将使得性能优势无效。

#ifndef PCH_H
#define PCH_H

// 添加要在此处预编译的标头
#include "framework.h"
#include <include/dhnetsdk.h>

extern "C" _declspec(dllexport) int Add(int a, int b);

typedef int(__stdcall *fTestCallback)(BYTE * pBuffer, DWORD bufSize);
int CALLBACK callback(BYTE * pBuffer, DWORD bufSize);
int invokeCallback(int a, int b, fTestCallback cb);
extern "C" _declspec(dllexport) void testInvokeCallback(int a, int b, BYTE *out, DWORD* size);

extern "C" _declspec(dllexport) int CALL_METHOD CICDI_CLIENT_FetchPicture_DetectTemp(
	long long lLoginID,
	int nChannelID,
	unsigned long dwAlarmType,
	bool bNeedPicFile,
	unsigned int dwUser,
	void* Reserved,
	// 下面是输出参数
	//unsigned long *dwAlarmType,
	void *pAlarmInfo,
	unsigned char *pBuffer,
	unsigned int *dwBuffSize);
// 调用其他的dll
//extern "C" _declspec(dllexport) int Sub(int a, int b);
#endif //PCH_H
