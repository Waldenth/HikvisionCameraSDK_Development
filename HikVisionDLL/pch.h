// pch.h: 这是预编译标头文件。
// 下方列出的文件仅编译一次，提高了将来生成的生成性能。
// 这还将影响 IntelliSense 性能，包括代码完成和许多代码浏览功能。
// 但是，如果此处列出的文件中的任何一个在生成之间有更新，它们全部都将被重新编译。
// 请勿在此处添加要频繁更新的文件，这将使得性能优势无效。

#ifndef PCH_H
#define _CRT_SECURE_NO_WARNINGS
#define PCH_H


// 添加要在此处预编译的标头
#include "framework.h"
#include <windows.h>
#include <minwindef.h>
#include <iostream>
using namespace std;



#ifdef EXPORT_DLL
#else
#define EXPORT_DLL extern "C" _declspec(dllexport)//指定允许给其他外部调用
#endif


EXPORT_DLL int testDLL(char input[]);
EXPORT_DLL int testDownloadReplayVideo();

EXPORT_DLL void GetTime(char *input, DWORD* output);//Get time from string format like "YYYY-MM-DD hh:mm:ss"
EXPORT_DLL int DownloadReplayVideo(char ipAddress[], char startTimeStr[], char endTimeStr[], char userID[], char password[], char filePath[]);

//EXPORT_DLL void g_RealDataCallBack_V30(LONG lRealHandle, DWORD dwDataType, BYTE* pBuffer, DWORD dwBufSize, void* dwUser);
EXPORT_DLL void PsDataCallBack(LONG lRealHandle, DWORD dwDataType, BYTE* pPacketBuffer, DWORD nPacketSize, void* pUser);
EXPORT_DLL int RealGetVideo(char ipAddress[], char userID[], char password[], char filePath[]);

EXPORT_DLL void CALLBACK fVoiceDataCallBack(LONG lVoiceComHandle, char* pRecvDataBuffer, DWORD dwBufSize, BYTE byAudioFlag, void* pUser);
EXPORT_DLL int RealGetVoice(char ipAddress[], char userID[], char password[], char filePath[]);

#endif //PCH_H