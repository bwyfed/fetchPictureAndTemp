// pch.cpp: 与预编译标头对应的源文件
#include <cstring>
#include "pch.h"
//#include "mydllpch.h"

// 当使用预编译的头时，需要使用此源文件，编译才能成功。
#include <iostream>
using namespace std;

typedef int(*funcptr) (int, int);

#ifdef _WIN64
#pragma comment(lib, "lib/win64/dhnetsdk.lib")
//#pragma comment(lib, "lib/win64/mydll.lib")
#else
#pragma comment(lib, "lib/win32/dhnetsdk.lib")
//#pragma comment(lib, "lib/win32/mydll.lib")	
#endif

static DEV_EVENT_ANATOMY_TEMP_DETECT_INFO* gPAlarmInfo;
static BYTE *gPBuffer;
static DWORD gDwBufSize;

int Add(int a, int b) {
	int result = a + b;
	cout << "In dll..., Add function, a=" << a << ",b=" << b << ", result = " << result << endl;
	return result;
}

void testInvokeCallback(int a, int b, BYTE *out, DWORD* size) {
	cout << "in testInvokeCallback, a=" << a << ",b=" << b << endl;
	//int result = Sub(a, b);
	//cout << "mydll Sub, Sub(a,b)=" << result << endl;
	const char *str = "mydll.dll";
	WCHAR wszClassName[256];
	memset(wszClassName, 0, sizeof(wszClassName));
	MultiByteToWideChar(CP_ACP, 0, str, strlen(str) + 1, wszClassName, sizeof(wszClassName) / sizeof(wszClassName[0]));
	const char* funcName = "Sub";
	HMODULE hDLL = LoadLibrary(wszClassName);
	if (hDLL != NULL) {
		cout << "hahaha1" << endl;
		funcptr func = (funcptr)GetProcAddress(hDLL, funcName);
		cout << "gagaga2" << endl;
		if (func != NULL)
		{
			int res = (*func)(120,110);
			cout << "res=" << res << endl;
		}
		else
		{
			std::cout << "Cannot Find Function " << funcName << endl;
		}
	}
	else {
		cout << "wuwu" << endl;
	}

	// 测试生成一个 BYTE 数组，并设置数组的大小
	for (int i = 0; i < 8; i++) {
		out[i] = i + 'a';
	}
	for (int i = 0; i < 8; i++) {
		cout << out[i] << ",";
	}
	cout << "*size:" << *size << endl;
	*size = 8;
	cout << "*size:" << *size << endl;
	//int invokeRes = invokeCallback(a, b, callback); // invokeCallback == CLIENT_RealLoadPictureEx
	//if (invokeRes == 200) {
	//	*size = gDwBufSize;
	//	for (int i = 0; i < *size; i++) {
	//		out[i] = gPBuffer[i];
	//	}
	//}
	return;
}

int invokeCallback(int a, int b, fTestCallback cb) {
	unsigned char * arr = nullptr;
	int size = 0;
	int result = 0;
	if (a > b) {
		arr = new unsigned char[a];
		for (int i = 0; i < a; i++) {
			arr[i] = 'a' + i;
		}
		result = cb(arr, a);
	}
	else {
		arr = new unsigned char[b];
		for (int i = 0; i < b; i++) {
			arr[i] = 'z' - i;
		}
		result = cb(arr, b);
	}
	return result;
}

int CALLBACK callback(BYTE * pBuffer, DWORD bufSize) {
	cout << "in callback, pBuffer:" << pBuffer << ",bufSize=" << bufSize << endl;
	for (int i = 0; i < bufSize; i++) {
		cout << pBuffer[i] << ",";
	}
	gPBuffer = pBuffer;
	gDwBufSize = bufSize;
	cout << endl;
	return 200;
}