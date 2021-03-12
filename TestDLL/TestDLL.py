from ctypes import *
import os
# os.chdir("F:/workspace/HikVision/test")
pDll = cdll.LoadLibrary("./HikVisionDLL.dll")

# DLL基本测试
mystr=(c_char*100)(*bytes("Hello你好".encode("GBK")))
cast(mystr,POINTER(c_char))

#res = pDll.testDLL(mystr) # success
#res = pDll.testDownloadReplayVideo() # success

ipAddress=(c_char*100)(*bytes("192.168.31.167".encode("utf-8")))
startTimeStr=(c_char*100)(*bytes("2021-01-27 12:07:17".encode("utf-8")))
endTimeStr=(c_char*100)(*bytes("2021-01-27 12:08:09".encode("utf-8")))
userID=(c_char*100)(*bytes("admin".encode("utf-8")))
password=(c_char*100)(*bytes("abcd1234".encode("utf-8")))

# 自定义接口测试
replayFilePath=(c_char*100)(*bytes("./replay.mp4".encode("utf-8")))
realVideoPath=(c_char*100)(*bytes("./videoData.dat".encode("utf-8")))
realVoicePath=(c_char*100)(*bytes("./voiceData.dat".encode("utf-8")))

#res = pDll.DownloadReplayVideo(ipAddress,startTimeStr,endTimeStr,userID,password,replayFilePath) # success
#res=pDll.RealGetVideo(ipAddress,userID,password,realVideoPath) #success
res=pDll.RealGetVoice(ipAddress,userID,password,realVoicePath)


print(res)