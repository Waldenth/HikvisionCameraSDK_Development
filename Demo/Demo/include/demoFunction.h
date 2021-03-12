#ifndef _DEMOFUNCTION_H_
#define _DEMOFUNCTION_H_
#include <iostream>
#include "Windows.h"
#include "HCNetSDK.h"
#include "cameraInfo.h"
using namespace std;
int downloadByTime(DWORD startTime[], DWORD endTime[]);
void realPlay(DWORD playTime);
void realPlayByOpenCV(DWORD playTime);
void realPlayByUser(DWORD playTime);
#endif // !_DEMOFUNCTION_H_

