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
	case EXCEPTION_RECONNECT: //Ԥ��ʱ����
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
	//��ʼ��
	NET_DVR_Init();
	//��������ʱ��������ʱ��
	NET_DVR_SetConnectTime(2000, 1);
	NET_DVR_SetReconnect(10000, true);

	//���ļ�
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
	//�����쳣��Ϣ�ص�����
	NET_DVR_SetExceptionCallBack_V30(0, NULL, g_ExceptionCallBack_openCV, NULL);
	//---------------------------------------
	//����Ԥ�������ûص�������
	LONG lRealPlayHandle;
	//HWND hWnd = GetConsoleHwnd(); //��ȡ���ھ��

	cvNamedWindow("[ʵʱԤ��]By openCV", 0);
	HWND  h = (HWND)cvGetWindowHandle("[ʵʱԤ��]By openCV");
	
	NET_DVR_PREVIEWINFO struPlayInfo = { 0 };
	struPlayInfo.hPlayWnd = h; //��Ҫ SDK ����ʱ�����Ϊ��Чֵ����ȡ��������ʱ����Ϊ��
	struPlayInfo.lChannel = 1; //Ԥ��ͨ����
	struPlayInfo.dwStreamType = 0; //0-�������� 1-�������� 2-���� 3�� 3-���� 4���Դ�����
	struPlayInfo.dwLinkMode = 0; //0- TCP ��ʽ�� 1- UDP ��ʽ�� 2- �ಥ��ʽ�� 3- RTP ��ʽ�� 4-RTP/RTSP�� 5-RSTP/HTTP
	struPlayInfo.bBlocked = 1; //0- ������ȡ���� 1- ����ȡ��
	
	lRealPlayHandle = NET_DVR_RealPlay_V40(lUserID, &struPlayInfo, NULL, NULL);

	NET_DVR_OpenSound(lRealPlayHandle);//��������

	if (lRealPlayHandle < 0)
	{
		printf("NET_DVR_RealPlay_V40 error\n");
		NET_DVR_Logout(lUserID);
		NET_DVR_Cleanup();
		return -2;
	}

	waitKey();
	Sleep(playTime);// �û��������������ǰ����ʱ��

	//---------------------------------------
	//�ر�Ԥ��
	NET_DVR_StopRealPlay(lRealPlayHandle);
	//ע���û�
	NET_DVR_Logout(lUserID);
	//�ͷ� SDK ��Դ
	NET_DVR_Cleanup();
	return 0;
}

int main() {
	realPlayByOpenCV(10000);
}