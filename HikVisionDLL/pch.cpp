// pch.cpp: 与预编译标头对应的源文件

#include "pch.h"
#include "HCNetSDK.h"
#include <iostream>
#include <minwindef.h>
#include <string.h>
#include <windows.h>

using namespace std;

EXPORT_DLL FILE *g_pFile = NULL;

// 当使用预编译的头时，需要使用此源文件，编译才能成功。

int testDLL(char input[]) {
    printf("%s\n", input);
    return 2233;
}

int testDownloadReplayVideo() {
    //"192.168.31.167", "2021-01-27 12:07:17", "2021-01-27 12:08:09", "admin",
    //"abcd1234", "./mytooltest.mp4"
    //---------------------------------------
    //获取起始时间
    DWORD startTime[6] = {2021, 1, 27, 12, 7, 17};
    DWORD endTime[6] = {2021, 1, 27, 12, 8, 9};

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
    NET_DVR_USER_LOGIN_INFO struLoginInfo = {0};
    struLoginInfo.bUseAsynLogin = 0;                        //同步登录方式
    strcpy(struLoginInfo.sDeviceAddress, "192.168.31.167"); //设备IP地址
    struLoginInfo.wPort = 8000;                             //设备服务端口
    strcpy(struLoginInfo.sUserName, "admin");    //设备登录用户名
    strcpy(struLoginInfo.sPassword, "abcd1234"); //设备登录密码

    //设备信息, 输出参数
    NET_DVR_DEVICEINFO_V40 struDeviceInfoV40 = {0};

    lUserID = NET_DVR_Login_V40(&struLoginInfo, &struDeviceInfoV40);

    if (lUserID < 0) {
        printf("Login failed, error code: %d\n", NET_DVR_GetLastError());
        NET_DVR_Cleanup();
        return -1; //登录失败
    }

    NET_DVR_PLAYCOND struDownloadCond = {0};
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
    hPlayback = NET_DVR_GetFileByTime_V40(lUserID, "./mytooltest.mp4",
                                          &struDownloadCond);
    if (hPlayback < 0) {
        printf("NET_DVR_GetFileByTime_V40 fail,last error %d\n",
               NET_DVR_GetLastError());
        NET_DVR_Logout(lUserID);
        NET_DVR_Cleanup();
        return -2; //获取回放失败
    }

    //---------------------------------------
    //开始下载
    if (!NET_DVR_PlayBackControl_V40(hPlayback, NET_DVR_PLAYSTART, NULL, 0,
                                     NULL, NULL)) {
        printf("Play back control failed [%d]\n", NET_DVR_GetLastError());
        NET_DVR_Logout(lUserID);
        NET_DVR_Cleanup();
        return -3;
    }

    int nPos = 0;
    for (nPos = 0; nPos < 100 && nPos >= 0;
         nPos = NET_DVR_GetDownloadPos(hPlayback)) {
        printf("Be downloading... %d %%\n", nPos);
        Sleep(5000); // millisecond
    }
    if (!NET_DVR_StopGetFile(hPlayback)) {
        printf("failed to stop get file [%d]\n", NET_DVR_GetLastError());
        NET_DVR_Logout(lUserID);
        NET_DVR_Cleanup();
        return -4;
    }
    if (nPos < 0 || nPos > 100) {
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

void GetTime(char *input, DWORD *output) {
    enum states { before, inside } state;
    state = before;
    int curIndex = 0;       // for read input char
    int curOutputIndex = 0; // for write output data
    int curTmpIndex = 0;    // for tmpStr
    char tmpStr[] = "xxxxx";
    char curGetChar;
    while (curIndex < strlen(input)) {
        curGetChar = input[curIndex++];
        switch (state) {
        case before:
            switch (curGetChar) {
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
            switch (curGetChar) {
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
                if (curIndex == strlen(input)) {
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
    for (int i = 0; i < 6; i++) {
        printf("%lu ", output[i]);
    }
    printf("\n");
    return;
}

int DownloadReplayVideo(char ipAddress[], char startTimeStr[],
                        char endTimeStr[], char userID[], char password[],
                        char filePath[]) {
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
    NET_DVR_USER_LOGIN_INFO struLoginInfo = {0};
    struLoginInfo.bUseAsynLogin = 0;                 //同步登录方式
    strcpy(struLoginInfo.sDeviceAddress, ipAddress); //设备IP地址
    struLoginInfo.wPort = 8000;                      //设备服务端口
    strcpy(struLoginInfo.sUserName, userID);         //设备登录用户名
    strcpy(struLoginInfo.sPassword, password);       //设备登录密码

    //设备信息, 输出参数
    NET_DVR_DEVICEINFO_V40 struDeviceInfoV40 = {0};

    lUserID = NET_DVR_Login_V40(&struLoginInfo, &struDeviceInfoV40);
    if (lUserID < 0) {
        printf("Login failed, error code: %d\n", NET_DVR_GetLastError());
        NET_DVR_Cleanup();
        return -1; //登录失败
    }

    NET_DVR_PLAYCOND struDownloadCond = {0};
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
    hPlayback = NET_DVR_GetFileByTime_V40(lUserID, filePath, &struDownloadCond);
    if (hPlayback < 0) {
        printf("NET_DVR_GetFileByTime_V40 fail,last error %d\n",
               NET_DVR_GetLastError());
        NET_DVR_Logout(lUserID);
        NET_DVR_Cleanup();
        return -2; //获取回放失败
    }

    //---------------------------------------
    //开始下载
    if (!NET_DVR_PlayBackControl_V40(hPlayback, NET_DVR_PLAYSTART, NULL, 0,
                                     NULL, NULL)) {
        printf("Play back control failed [%d]\n", NET_DVR_GetLastError());
        NET_DVR_Logout(lUserID);
        NET_DVR_Cleanup();
        return -3;
    }

    int nPos = 0;
    for (nPos = 0; nPos < 100 && nPos >= 0;
         nPos = NET_DVR_GetDownloadPos(hPlayback)) {
        printf("Be downloading... %d %%\n", nPos);
        Sleep(5000); // millisecond
    }
    if (!NET_DVR_StopGetFile(hPlayback)) {
        printf("failed to stop get file [%d]\n", NET_DVR_GetLastError());
        NET_DVR_Logout(lUserID);
        NET_DVR_Cleanup();
        return -4;
    }
    if (nPos < 0 || nPos > 100) {
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

void PsDataCallBack(LONG lRealHandle, DWORD dwDataType, BYTE *pPacketBuffer,
                    DWORD nPacketSize, void *pUser) {
    
    
    if (dwDataType == NET_DVR_SYSHEAD) {
        printf("dataTYpe=%u\n", dwDataType);
        //写入头数据

        if (g_pFile == NULL) {
            printf("CreateFileHead fail\n");
            return;
        }

        //写入头数据
        fwrite(pPacketBuffer, sizeof(unsigned char), nPacketSize, g_pFile);
        printf("write head len=%d\n", nPacketSize);
    } else {
        if (g_pFile != NULL) {
            fwrite(pPacketBuffer, sizeof(unsigned char), nPacketSize, g_pFile);
            printf("write data len=%d\n", nPacketSize);
        }
    }
}

int RealGetVideo(char ipAddress[], char userID[], char password[], char filePath[]) {
    NET_DVR_Init();
    NET_DVR_SetLogToFile(3, "./record/");

    //打开文件
    g_pFile = fopen(filePath, "wb");

    if (g_pFile == NULL)
    {
        printf("CreateFileHead fail\n");
        fclose(g_pFile);
        g_pFile = NULL;
        return -4;
    }

    int iChannel = 1;

    NET_DVR_DEVICEINFO_V30 struDeviceInfo;
    
    
    int iUserID = NET_DVR_Login_V30(ipAddress, 8000, userID, password, &struDeviceInfo);


    if (iUserID >= 0)
    {

        //NET_DVR_CLIENTINFO ClientInfo = {0};
        //ClientInfo.lChannel     = iChannel;  //channel NO.
        //ClientInfo.lLinkMode    = 0;
        //ClientInfo.sMultiCastIP = NULL;
        //int iRealPlayHandle = NET_DVR_RealPlay_V30(iUserID, &ClientInfo, PsDataCallBack, NULL, 0);
        NET_DVR_PREVIEWINFO struPreviewInfo = { 0 };
        struPreviewInfo.lChannel = iChannel;
        struPreviewInfo.dwStreamType = 0;
        struPreviewInfo.dwLinkMode = 0;
        struPreviewInfo.bBlocked = 1;
        struPreviewInfo.bPassbackRecord = 1;


        int iRealPlayHandle = NET_DVR_RealPlay_V40(iUserID, &struPreviewInfo, PsDataCallBack, NULL);

        if (iRealPlayHandle >= 0)
        {
            printf("[GetStream]---RealPlay %s:%d success, \n", ipAddress, iChannel, NET_DVR_GetLastError());
            //int iRet = NET_DVR_SaveRealData(iRealPlayHandle, "./record/realplay.dat");
            //NET_DVR_SetStandardDataCallBack(iRealPlayHandle, StandardDataCallBack, 0);

        }
        else
        {
            printf("[GetStream]---RealPlay %s:%d failed, error = %d\n", ipAddress, iChannel, NET_DVR_GetLastError());
        }
    }
    else
    {
        printf("[GetStream]---Login %s failed, error = %d\n", ipAddress, NET_DVR_GetLastError());
    }


    char c = 0;
    while ('q' != c)
    {
        printf("input 'q' to quit\n");
        printf("input: ");
        scanf("%c", &c);
    }


    NET_DVR_Cleanup();
    return 0;
}



void CALLBACK fVoiceDataCallBack(LONG lVoiceComHandle, char* pRecvDataBuffer, DWORD dwBufSize, BYTE byAudioFlag, void* pUser)
{
    if (byAudioFlag == 1) {
        fwrite(pRecvDataBuffer, sizeof(unsigned char), dwBufSize, g_pFile);
        printf("write head len=%d\n", dwBufSize);
    }
}

int RealGetVoice(char ipAddress[], char userID[], char password[], char filePath[]) {
    //---------------------------------------
    // 初始化
    NET_DVR_Init();
    NET_DVR_SetLogToFile(3, "./record/");
    //设置连接时间与重连时间
    NET_DVR_SetConnectTime(2000, 1);
    NET_DVR_SetReconnect(10000, true);

    //打开文件
    g_pFile = fopen(filePath, "wb");

    if (g_pFile == NULL)
    {
        printf("CreateFileHead fail\n");
        fclose(g_pFile);
        g_pFile = NULL;
        return -4;
    }

    //---------------------------------------
    // 注册设备
    LONG lUserID;

    //登录参数，包括设备地址、登录用户、密码等
    NET_DVR_USER_LOGIN_INFO struLoginInfo = { 0 };
    struLoginInfo.bUseAsynLogin = 0; //同步登录方式
    strcpy(struLoginInfo.sDeviceAddress, ipAddress); //设备IP地址
    struLoginInfo.wPort = 8000; //设备服务端口
    strcpy(struLoginInfo.sUserName, userID); //设备登录用户名
    strcpy(struLoginInfo.sPassword, password); //设备登录密码

    //设备信息, 输出参数
    NET_DVR_DEVICEINFO_V40 struDeviceInfoV40 = { 0 };

    lUserID = NET_DVR_Login_V40(&struLoginInfo, &struDeviceInfoV40);
    if (lUserID < 0)
    {
        printf("Login failed, error code: %d\n", NET_DVR_GetLastError());
        NET_DVR_Cleanup();
        fclose(g_pFile);
        g_pFile = NULL;
        return -1;
    }

    //语音捕获

    //BOOL lVoiceHanle;
    //lVoiceHanle = NET_DVR_SetVoiceDataCallBack(lUserID, 1,fVoiceDataCallBack, NULL);
    LONG lVoiceComHandle = NET_DVR_StartVoiceCom_MR_V30(lUserID, 1, fVoiceDataCallBack, NULL);
    if (lVoiceComHandle < 0)
    {
        printf("NET_DVR_SetVoiceDataCallBack error, %d!\n", NET_DVR_GetLastError());
        NET_DVR_Logout(lUserID);
        NET_DVR_Cleanup();
        fclose(g_pFile);
        g_pFile = NULL;
        return -2;
    }

    char c = 0;

    while ('q' != c)
    {
        printf("input 'q' to quit\n");
        printf("input: ");
        scanf("%c", &c);
    }


    //关闭语音对讲
    if (!NET_DVR_StopVoiceCom(lVoiceComHandle))
    {
        printf("NET_DVR_StopVoiceCom error, %d!\n", NET_DVR_GetLastError());
        NET_DVR_Logout(lUserID);
        NET_DVR_Cleanup();
        fclose(g_pFile);
        g_pFile = NULL;
        return -3;
    }

    //注销用户
    NET_DVR_Logout(lUserID);
    //释放SDK资源
    NET_DVR_Cleanup();

    fclose(g_pFile);
    g_pFile = NULL;

    return 0;
}
