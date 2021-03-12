//#include "demoFunction.h"
//#include "cameraInfo.h"
#include <Windows.h>
#include "CameraTool.h"
#include <stdio.h>
#include <stdlib.h>

using namespace std;

/*
int downloadTest() {
	DWORD startTime[6] = { 2021,1,25,22,16,10 };
	DWORD endTime[6] = { 2021,1,25,22,17,59 };
	printf("准备下载[%d/%d/%d-%d:%d:%d]到[%d/%d/%d-%d:%d:%d]的视频:\n", startTime[0], startTime[1], startTime[2], startTime[3],
		startTime[4], startTime[5], endTime[0], endTime[1], endTime[2], endTime[3], endTime[4], endTime[5]);
	if (downloadByTime(startTime, endTime) == 0) {
		cout << "下载完成" << endl;
		system("PAUSE");
		cin.clear();
		char play = 'N';
		cout << "是否使用ffmpeg播放?[Y/N]" << endl;
		cin >> play;
		cout << "You choose " << play << endl;
		if (play == 'Y') {
			cout << "start play video" << endl;
			system("ffplay test.mp4");
		}
	}
	else {
		cout << "出现异常!" << endl;
		return -1;
	}
	system("PAUSE");
	return 0;
}
*/


int TEST() {
	//DownloadReplayVideo("192.168.3.13", "2021-01-27 12:07:17","2021-01-27 12:08:09","admin","abcd1234","./mytooltest.mp4");
	//realPlayByOpenCV();
	return 0;
}
