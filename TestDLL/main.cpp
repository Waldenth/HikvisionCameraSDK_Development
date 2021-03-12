#include <windows.h>
#include <stdio.h>
#include <iostream>
using namespace std;

typedef int (*function0)();
typedef int (*function1)(string ipAddress, string startTimeStr, string endTimeStr, string userID, string password, char filePath[]);

int main() {
	HINSTANCE hDllInst;
	hDllInst = LoadLibrary(L"HikVisionDLL.dll");
	if (hDllInst) {
		function0 func0= (function0)GetProcAddress(hDllInst, "testDownloadReplayVideo");
		function1 func1 = (function1)GetProcAddress(hDllInst, "DownloadReplayVideo");
		printf("%d \n", func0());
		printf("%d \n", func1("192.168.31.167", "2021-01-27 12:07:17", "2021-01-27 12:08:09", "admin", "abcd1234", "./mytooltest.mp4"));
	}
	return 0;
}