#include <stdio.h>
#include <iostream>
#include "Windows.h"
#include "HCNetSDK.h"
#include "plaympeg4.h"
#include <time.h>
#include "cameraInfo.h"
using namespace std;

typedef HWND(WINAPI* PROCGETCONSOLEWINDOW)();
PROCGETCONSOLEWINDOW GetConsoleWindowAPI_User;

LONG lPort = -1; //ȫ�ֵĲ��ſ�port��

void CALLBACK g_RealDataCallBack_V30_User(LONG lRealHandle, DWORD dwDataType, BYTE* pBuffer, DWORD dwBufSize, void* dwUser)
{
	HWND hWnd = GetConsoleWindowAPI_User();
	switch (dwDataType)
	{
		case NET_DVR_SYSHEAD: //ϵͳͷ
			if (lPort >= 0)
			{
				break;  //��ͨ��ȡ��֮ǰ�Ѿ���ȡ������������ӿڲ���Ҫ�ٵ���
			}

			if (!PlayM4_GetPort(&lPort))  //��ȡ���ſ�δʹ�õ�ͨ����
			{
				break;
			}
			//m_iPort = lPort; //��һ�λص�����ϵͳͷ������ȡ�Ĳ��ſ�port�Ÿ�ֵ��ȫ��port���´λص�����ʱ��ʹ�ô�port�Ų���
			if (dwBufSize > 0)
			{
				if (!PlayM4_SetStreamOpenMode(lPort, STREAME_REALTIME))  //����ʵʱ������ģʽ
				{
					break;
				}

				if (!PlayM4_OpenStream(lPort, pBuffer, dwBufSize, 1024 * 1024)) //�����ӿ�
				{
					break;
				}

				if (!PlayM4_Play(lPort, hWnd)) //���ſ�ʼ
				{
					break;
				}
				printf("���Ŷϵ�1\n");
				NET_DVR_SaveRealData(lRealHandle, "userTest.mp4");
			}
			break;
		case NET_DVR_STREAMDATA:   //��������
			if (dwBufSize > 0 && lPort != -1)
			{
				if (!PlayM4_InputData(lPort, pBuffer, dwBufSize))
				{
					break;
				}
			}
			break;
		default: //��������
			if (dwBufSize > 0 && lPort != -1)
			{
				if (!PlayM4_InputData(lPort, pBuffer, dwBufSize))
				{
					break;
				}
			}
			break;
	}
}

void CALLBACK g_ExceptionCallBack_User(DWORD dwType, LONG lUserID, LONG lHandle, void* pUser)
{
	char tempbuf[256] = { 0 };
	switch (dwType)
	{
	case EXCEPTION_RECONNECT:    //Ԥ��ʱ����
		printf("----------reconnect--------%d\n", time(NULL));
		break;
	default:
		break;
	}
}

void realPlayByUser(DWORD playTime) {

	//---------------------------------------
	// ��ʼ��
	NET_DVR_Init();
	//��������ʱ��������ʱ��
	NET_DVR_SetConnectTime(2000, 1);
	NET_DVR_SetReconnect(10000, true);

	//---------------------------------------
	//�����쳣��Ϣ�ص�����
	NET_DVR_SetExceptionCallBack_V30(0, NULL, g_ExceptionCallBack_User, NULL);

	//---------------------------------------
	// ��ȡ����̨���ھ��
	HMODULE hKernel32 = GetModuleHandle("kernel32");
	GetConsoleWindowAPI_User = (PROCGETCONSOLEWINDOW)GetProcAddress(hKernel32, "GetConsoleWindow");

	//---------------------------------------
	// ע���豸
	LONG lUserID;

	//��¼�����������豸��ַ����¼�û��������
	NET_DVR_USER_LOGIN_INFO struLoginInfo = { 0 };
	struLoginInfo.bUseAsynLogin = 0; //ͬ����¼��ʽ
	strcpy(struLoginInfo.sDeviceAddress, ipAddress); //�豸IP��ַ
	struLoginInfo.wPort = 8000; //�豸����˿�
	strcpy(struLoginInfo.sUserName, "admin"); //�豸��¼�û���
	strcpy(struLoginInfo.sPassword, "abcd1234"); //�豸��¼����

	//�豸��Ϣ, �������
	NET_DVR_DEVICEINFO_V40 struDeviceInfoV40 = { 0 };

	lUserID = NET_DVR_Login_V40(&struLoginInfo, &struDeviceInfoV40);
	if (lUserID < 0)
	{
		printf("Login failed, error code: %d\n", NET_DVR_GetLastError());
		NET_DVR_Cleanup();
		return;
	}

	//---------------------------------------
	//����Ԥ�������ûص�������
	LONG lRealPlayHandle;

	NET_DVR_PREVIEWINFO struPlayInfo = { 0 };
	struPlayInfo.hPlayWnd = NULL;         //��ҪSDK����ʱ�����Ϊ��Чֵ����ȡ��������ʱ����Ϊ��
	struPlayInfo.lChannel = 1;       //Ԥ��ͨ����
	struPlayInfo.dwStreamType = 0;       //0-��������1-��������2-����3��3-����4���Դ�����
	struPlayInfo.dwLinkMode = 0;       //0- TCP��ʽ��1- UDP��ʽ��2- �ಥ��ʽ��3- RTP��ʽ��4-RTP/RTSP��5-RSTP/HTTP
	struPlayInfo.bBlocked = 1;       //0- ������ȡ����1- ����ȡ��

	lRealPlayHandle = NET_DVR_RealPlay_V40(lUserID, &struPlayInfo, g_RealDataCallBack_V30_User, NULL);

	if (lRealPlayHandle < 0)
	{
		printf("NET_DVR_RealPlay_V40 error, %d\n", NET_DVR_GetLastError());
		NET_DVR_Logout(lUserID);
		NET_DVR_Cleanup();
		return;
	}

	Sleep(playTime);
	//---------------------------------------
	//�ر�Ԥ��
	NET_DVR_StopRealPlay(lRealPlayHandle);

	//�ͷŲ��ſ���Դ
	PlayM4_Stop(lPort);
	PlayM4_CloseStream(lPort);
	PlayM4_FreePort(lPort);

	//ע���û�
	NET_DVR_Logout(lUserID);
	NET_DVR_Cleanup();

	return;
}

