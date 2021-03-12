#include <stdio.h>
#include <iostream>
#include "Windows.h"
#include "HCNetSDK.h"
#include <time.h>
#include <opencv2/opencv.hpp>
#include<opencv2/highgui/highgui_c.h>
#include "cameraInfo.h"

using namespace std;
using namespace cv;

FILE* g_pFile = NULL;

void CALLBACK g_ExceptionCallBack_openCV(DWORD dwType, LONG lUserID, LONG lHandle, void* pUser)
{
	char tempbuf[256] = { 0 };
	switch (dwType)
	{
	case EXCEPTION_RECONNECT: //预览时重连
		printf("----------reconnect-------- % d\n", time(NULL));
		break;
	default:
		break;
	}
	fwrite(tempbuf, sizeof(unsigned char), 256, g_pFile);
	printf("write data \n");
}


int realPlayByOpenCV(DWORD playTime) {
	//---------------------------------------
	//初始化
	NET_DVR_Init();
	//设置连接时间与重连时间
	NET_DVR_SetConnectTime(2000, 1);
	NET_DVR_SetReconnect(10000, true);

	//打开文件
	g_pFile = fopen("voice.dat", "wb");
	if (g_pFile == NULL)
	{
		printf("CreateFileHead fail\n");
		return -4;
	}

	LONG lUserID;
	NET_DVR_DEVICEINFO_V30 struDeviceInfo;
	
	lUserID = NET_DVR_Login_V30(ipAddress, port, userID, password, &struDeviceInfo);

	if (lUserID < 0)
	{
		printf("Login error, % d\n", NET_DVR_GetLastError());
		NET_DVR_Cleanup();
		return -1;
	}
	//---------------------------------------
	//设置异常消息回调函数
	NET_DVR_SetExceptionCallBack_V30(0, NULL, g_ExceptionCallBack_openCV, NULL);
	//---------------------------------------
	//启动预览并设置回调数据流
	LONG lRealPlayHandle;
	//HWND hWnd = GetConsoleHwnd(); //获取窗口句柄

	cvNamedWindow("[实时预览]By openCV", 0);
	HWND  h = (HWND)cvGetWindowHandle("[实时预览]By openCV");
	
	NET_DVR_PREVIEWINFO struPlayInfo = { 0 };
	struPlayInfo.hPlayWnd = h; //需要 SDK 解码时句柄设为有效值，仅取流不解码时可设为空
	struPlayInfo.lChannel = 1; //预览通道号
	struPlayInfo.dwStreamType = 0; //0-主码流， 1-子码流， 2-码流 3， 3-码流 4，以此类推
	struPlayInfo.dwLinkMode = 0; //0- TCP 方式， 1- UDP 方式， 2- 多播方式， 3- RTP 方式， 4-RTP/RTSP， 5-RSTP/HTTP
	struPlayInfo.bBlocked = 1; //0- 非阻塞取流， 1- 阻塞取流
	
	lRealPlayHandle = NET_DVR_RealPlay_V40(lUserID, &struPlayInfo, NULL, NULL);

	NET_DVR_OpenSound(lRealPlayHandle);//开启声音

	if (lRealPlayHandle < 0)
	{
		printf("NET_DVR_RealPlay_V40 error\n");
		NET_DVR_Logout(lUserID);
		NET_DVR_Cleanup();
		return -2;
	}

	waitKey();
	Sleep(playTime);// 敲击按键后结束播放前延滞时间

	//---------------------------------------
	//关闭预览
	NET_DVR_StopRealPlay(lRealPlayHandle);
	//注销用户
	NET_DVR_Logout(lUserID);
	//释放 SDK 资源
	NET_DVR_Cleanup();
	return 0;
}

int main() {
	realPlayByOpenCV(10000);
}