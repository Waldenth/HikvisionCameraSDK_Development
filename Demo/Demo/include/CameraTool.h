#ifndef _CAMERATOOL_H_
#define _CAMERATOOL_H_
#include <Windows.h>
#include <minwindef.h>
#include <iostream>
using namespace std;
void GetTime(string input, DWORD* output);//Get time from string format like "YYYY-MM-DD hh:mm:ss"
int DownloadReplayVideo(string ipAddress,string startTimeStr, string endTimeStr, string userID, string password, char filePath[]);
#endif