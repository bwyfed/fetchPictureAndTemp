﻿// pch.cpp: 与预编译标头对应的源文件
#include <cstring>
#include "pch.h"
//#include "mydllpch.h"

// 当使用预编译的头时，需要使用此源文件，编译才能成功。
#include <iostream>
using namespace std;

typedef int(*funcptr) (int, int);

typedef DWORD(*errfuncptr) ();
typedef int(*func2ptr) (long long lLoginID,
	int nChannelID,
	unsigned long dwAlarmType, // DWORD
	bool bNeedPicFile,
	unsigned int dwUser,
	void* Reserved);

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

// 提供给JS调用的函数，返回照片和温度相关的信息
int CALL_METHOD CICDI_CLIENT_FetchPicture_DetectTemp(
	long long lLoginID,
	int nChannelID,
	unsigned long dwAlarmType, // DWORD
	bool bNeedPicFile,
	unsigned int dwUser,
	void* Reserved,
	// 下面为出参
	//unsigned long *dwAlarmType,
	void *pAlarmInfo,
	unsigned char *pBuffer,
	unsigned int *dwBuffSize) {
	cout << "in c++, CICDI_CLIENT_FetchPicture_DetectTemp, params:" << endl;
	cout << "lLoginID: " << lLoginID
		<< ",nChannelID: " << nChannelID
		<< ",dwAlarmType: " << dwAlarmType
		<< ",bNeedPicFile: " << bNeedPicFile
		<< ",dwUser: "<< dwUser
		<< ",Reserved: " << Reserved
		<< endl;
	// 调用大华dll
	const char *str = "dhnetsdk.dll";
	WCHAR wszClassName[256];
	memset(wszClassName, 0, sizeof(wszClassName));
	MultiByteToWideChar(CP_ACP, 0, str, strlen(str) + 1, wszClassName, sizeof(wszClassName) / sizeof(wszClassName[0]));
	const char* funcName = "CLIENT_RealLoadPictureEx";
	HMODULE hDLL = LoadLibrary(wszClassName);
	if (hDLL != NULL) {
		cout << "hahaha1" << endl;
		func2ptr func = (func2ptr)GetProcAddress(hDLL, funcName);
		errfuncptr errfunc = (errfuncptr)GetProcAddress(hDLL, "CLIENT_GetLastError");
		cout << "gagaga2" << endl;
		if (func != NULL)
		{
			int lRealloadHandle = (*func)(lLoginID, nChannelID, dwAlarmType, bNeedPicFile, dwUser, Reserved);
			cout << "lRealloadHandle=" << lRealloadHandle << endl;
			if (lRealloadHandle)
			{
				cout<<"CLIENT_RealLoadPictureEx success\n";
			}
			else
			{
				cout<<"CLIENT_RealLoadPictureEx fail, error:"<< (*errfunc)()<<endl;
			}
		}
		else
		{
			std::cout << "Cannot Find Function " << funcName << endl;
		}
	}
	else {
		cout << "wuwu" << endl;
	}
	// 内部不必再分配空间
	unsigned long initBuffSize = 8;
	for (int i = 0; i < initBuffSize; i++) {
		pBuffer[i] = 'A' + i;
	}
	*dwBuffSize = 9;
	return 200;
}