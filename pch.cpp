// pch.cpp: 与预编译标头对应的源文件
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
	fAnalyzerDataCallBack cbAnalyzerData,
	unsigned int dwUser,
	void* Reserved);

#ifdef _WIN64
#pragma comment(lib, "lib/win64/dhnetsdk.lib")
//#pragma comment(lib, "lib/win64/mydll.lib")
#else
#pragma comment(lib, "lib/win32/dhnetsdk.lib")
//#pragma comment(lib, "lib/win32/mydll.lib")	
#endif

typedef struct tagDEV_EVENT_ANATOMY_TEMP_DETECT_INFO_1
{
	int                         nChannelID;				// 通道号
	int                         nAction;				// 1:开始 2:停止
	char                        szName[128];			// 事件名称
	double                      PTS;					// 时间戳(单位是毫秒)
	//NET_TIME_EX                 UTC;					// 事件发生的时间
	int                         nEventID;				// 事件ID

	//EM_CLASS_TYPE               emClassType;			// 智能事件所属大类
	UINT						nPresetID;				// 事件触发的预置点号, 从1开始, 没有该字段,表示预置点未知
	//NET_MAN_TEMP_INFO			stManTempInfo;          // 区域内人员体温信息 
	//NET_VIS_SCENE_IMAGE			stVisSceneImage;		// 可见光全景图
	//NET_THERMAL_SCENE_IMAGE		stThermalSceneImage;	// 热成像全景图
	UINT						nSequence;				// 帧序号
	UINT						nEventRelevanceID;		// 事件关联ID
	BYTE						byReserved[1020];		// 预留字段
} DEV_EVENT_ANATOMY_TEMP_DETECT_INFO_1;

static DEV_EVENT_ANATOMY_TEMP_DETECT_INFO_1* gPAlarmInfo1;
static DEV_EVENT_ANATOMY_TEMP_DETECT_INFO* gPAlarmInfo;
static BYTE *gPBuffer;
static DWORD gDwBufSize;

int  CALLBACK AnalyzerDataCallBack(LLONG lAnalyzerHandle, DWORD dwAlarmType, void* pAlarmInfo, BYTE *pBuffer, DWORD dwBufSize, LDWORD dwUser, int nSequence, void *reserved)
{
	// pBuffer 中是图片信息，保存下来供返回
	gPBuffer = pBuffer;
	gDwBufSize = dwBufSize;
	if (EVENT_IVS_ANATOMY_TEMP_DETECT == dwAlarmType)
	{
		DEV_EVENT_ANATOMY_TEMP_DETECT_INFO* pInfo = (DEV_EVENT_ANATOMY_TEMP_DETECT_INFO*)pAlarmInfo;
		printf("AnalyzerDataCallBack:anatomy temp detect,nChannelID:%d, nAction:%d, szName:%s,"
			"nEventID:%d, nSequence:%d, nPresetID:%d, PTS:%d\n",
			pInfo->nChannelID,
			pInfo->nAction,
			pInfo->szName,
			pInfo->nEventID,
			pInfo->nSequence,
			pInfo->nPresetID,
			pInfo->PTS
		);
		// 保存结构体信息
		gPAlarmInfo1->nChannelID = pInfo->nChannelID;
		gPAlarmInfo1->nAction = pInfo->nAction;
		strncpy_s(gPAlarmInfo1->szName, pInfo->szName, sizeof(pInfo->szName));
		gPAlarmInfo1->nEventID = pInfo->nEventID;
		gPAlarmInfo1->nSequence = pInfo->nSequence;
		gPAlarmInfo1->nPresetID = pInfo->nPresetID;
		gPAlarmInfo1->PTS = pInfo->PTS;


		// 区域内人员体温信息 
		// 人脸小图特征值起始位置：pBuffer + stManTempInfo.nOffset
		// 人脸小图特征值长度：stManTempInfo.nLength
		const NET_MAN_TEMP_INFO	&stManTempInfo = pInfo->stManTempInfo;
		printf("区域内人员体温信息:nObjectID:%d, dbHighTemp:%d, nTempUnit:%d,bIsOverTemp:%d, bIsUnderTemp:%d\n",
			stManTempInfo.nObjectID, stManTempInfo.dbHighTemp,
			stManTempInfo.nTempUnit, stManTempInfo.bIsOverTemp,
			stManTempInfo.bIsUnderTemp);

		// 可见光全景图		
		const NET_VIS_SCENE_IMAGE	&stVisSceneImage = pInfo->stVisSceneImage;
		printf("可见光全景图信息：nOffset:%d, nLength:%d, nWidth:%d,nHeight:%d",
			stVisSceneImage.nOffset, stVisSceneImage.nLength, stVisSceneImage.nWidth, stVisSceneImage.nHeight);
		// 可见光全景图起始位置：pBuffer + stVisSceneImage.nOffset
		// 可见光全景图长度：stVisSceneImage.nLength

		// 热成像全景图		
		const NET_THERMAL_SCENE_IMAGE	&stThermalSceneImage = pInfo->stThermalSceneImage;
		printf("热成像全景图信息：nOffset:%d, nLength:%d, nWidth:%d,nHeight:%d",
			stThermalSceneImage.nOffset, stThermalSceneImage.nLength, stThermalSceneImage.nWidth, stThermalSceneImage.nHeight);

		// 热成像全景图起始位置：pBuffer + stThermalSceneImage.nOffset
		// 热成像全景图长度：stThermalSceneImage.nLength

	}

	return 0;
}


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
	DEV_EVENT_ANATOMY_TEMP_DETECT_INFO_1 *pAlarmInfo,
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
			int lRealloadHandle = (*func)(lLoginID, nChannelID, dwAlarmType, bNeedPicFile, AnalyzerDataCallBack, dwUser, Reserved);
			cout << "lRealloadHandle=" << lRealloadHandle << endl;
			if (lRealloadHandle)
			{
				cout<<"CLIENT_RealLoadPictureEx success\n";
				*dwBuffSize = gDwBufSize;
				for (int i = 0; i < gDwBufSize; i++) {
					pBuffer[i] = gPBuffer[i];
				}
				// 结构体赋值
				*pAlarmInfo = *gPAlarmInfo1;
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