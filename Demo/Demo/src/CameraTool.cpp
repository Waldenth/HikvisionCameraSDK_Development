#include <Windows.h>
#include <minwindef.h>
#include <iostream>
#include "HCNetSDK.h"

using namespace std;

void GetTime(string input, DWORD* output);//Get time from string format like "YYYY-MM-DD hh:mm:ss"
int DownloadReplayVideo(string ipAddress, string startTimeStr, string endTimeStr, string userID, string password, char filePath[]);

int DownloadReplayVideo(string ipAddress,string startTimeStr,string endTimeStr,string userID,string password,char filePath[]) {
    //---------------------------------------
    //获取起始时间
    DWORD startTime[6];
    DWORD endTime[6];
    GetTime(startTimeStr, startTime);
    GetTime(endTimeStr, endTime); 
    //---------------------------------------
    // 初始化
    NET_DVR_Init();
    //设置连接时间与重连时间
    NET_DVR_SetConnectTime(2000, 1);
    NET_DVR_SetReconnect(10000, true);
    //---------------------------------------
    // 注册设备
    LONG lUserID;
    //登录参数，包括设备地址、登录用户、密码等
    NET_DVR_USER_LOGIN_INFO struLoginInfo = { 0 };
    struLoginInfo.bUseAsynLogin = 0; //同步登录方式
    strcpy(struLoginInfo.sDeviceAddress, ipAddress.c_str()); //设备IP地址
    struLoginInfo.wPort = 8000; //设备服务端口
    strcpy(struLoginInfo.sUserName, userID.c_str()); //设备登录用户名
    strcpy(struLoginInfo.sPassword, password.c_str()); //设备登录密码

    //设备信息, 输出参数
    NET_DVR_DEVICEINFO_V40 struDeviceInfoV40 = { 0 };

    lUserID = NET_DVR_Login_V40(&struLoginInfo, &struDeviceInfoV40);
    if (lUserID < 0)
    {
        printf("Login failed, error code: %d\n", NET_DVR_GetLastError());
        NET_DVR_Cleanup();
        return -1; //登录失败
    }

    NET_DVR_PLAYCOND struDownloadCond = { 0 };
    struDownloadCond.dwChannel = 1;

    struDownloadCond.struStartTime.dwYear = startTime[0];
    struDownloadCond.struStartTime.dwMonth = startTime[1];
    struDownloadCond.struStartTime.dwDay = startTime[2];
    struDownloadCond.struStartTime.dwHour = startTime[3];
    struDownloadCond.struStartTime.dwMinute = startTime[4];
    struDownloadCond.struStartTime.dwSecond = startTime[5];
    struDownloadCond.struStopTime.dwYear = endTime[0];
    struDownloadCond.struStopTime.dwMonth = endTime[1];
    struDownloadCond.struStopTime.dwDay = endTime[2];
    struDownloadCond.struStopTime.dwHour = endTime[3];
    struDownloadCond.struStopTime.dwMinute = endTime[4];
    struDownloadCond.struStopTime.dwSecond = endTime[5];

    //---------------------------------------
    //按时间下载
    int hPlayback;
    hPlayback = NET_DVR_GetFileByTime_V40(lUserID,filePath, &struDownloadCond);
    if (hPlayback < 0){
        printf("NET_DVR_GetFileByTime_V40 fail,last error %d\n", NET_DVR_GetLastError());
        NET_DVR_Logout(lUserID);
        NET_DVR_Cleanup();
        return -2; //获取回放失败
    }

    //---------------------------------------
    //开始下载
    if (!NET_DVR_PlayBackControl_V40(hPlayback, NET_DVR_PLAYSTART, NULL, 0, NULL, NULL))
    {
        printf("Play back control failed [%d]\n", NET_DVR_GetLastError());
        NET_DVR_Logout(lUserID);
        NET_DVR_Cleanup();
        return -3; 
    }

    int nPos = 0;
    for (nPos = 0; nPos < 100 && nPos >= 0; nPos = NET_DVR_GetDownloadPos(hPlayback))
    {
        printf("Be downloading... %d %%\n", nPos);
        Sleep(5000);  //millisecond
    }
    if (!NET_DVR_StopGetFile(hPlayback))
    {
        printf("failed to stop get file [%d]\n", NET_DVR_GetLastError());
        NET_DVR_Logout(lUserID);
        NET_DVR_Cleanup();
        return -4;
    }
    if (nPos < 0 || nPos>100)
    {
        printf("download err [%d]\n", NET_DVR_GetLastError());
        NET_DVR_Logout(lUserID);
        NET_DVR_Cleanup();
        return -5;
    }
    printf("Be downloading... %d %%\n", nPos);

    //注销用户
    NET_DVR_Logout(lUserID);
    //释放SDK资源
    NET_DVR_Cleanup();
    return 0;
}




void GetTime(string input, DWORD* output) {
    enum states {
        before, inside
    } state;
    state = before;
    int curIndex = 0;//for read input char
    int curOutputIndex = 0;//for write output data
    int curTmpIndex = 0;//for tmpStr
    char tmpStr[] = "xxxxx";
    char curGetChar;
    while (curIndex < input.length()) {
        curGetChar = input.at(curIndex++);
        switch (state)
        {
        case before:
            switch (curGetChar)
            {
            case ' ':
            case '-':
            case ':':
                break;
            default:
                tmpStr[curTmpIndex++] = curGetChar;
                state = inside;
                break;
            }
            break;
        case inside:
            switch (curGetChar)
            {
            case ' ':
            case '-':
            case ':':
                tmpStr[curTmpIndex] = '\0';
                output[curOutputIndex++] = atoi(tmpStr);
                curTmpIndex = 0;
                state = before;
                break;
            default:
                tmpStr[curTmpIndex++] = curGetChar;
                if (curIndex == input.length()) {
                    tmpStr[curTmpIndex] = '\0';
                    output[curOutputIndex++] = atoi(tmpStr);
                    curTmpIndex = 0;
                    state = before;
                }
                break;
            }
            break;

        default:
            break;
        }
    }
    return;
}
