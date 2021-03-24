
import time
import ffmpeg
import subprocess
import os
import pathlib
import sys
# rtsp://admin:abcd1234@192.168.31.167:554/h264/ch2/sub/av_stream


class Properties:
    rtspURL="rtsp://"
    username=""
    password=""
    ipAddress=""
    port="554"
    channel="/h264/ch1/sub/av_stream"
    filePath=""

    def __init__(self,username,password,ipAddress,port,channel,filePath):
        self.username=username
        self.password=password
        self.ipAddress=ipAddress
        self.port=port
        self.channel=channel
        self.rtspURL=self.rtspURL+username+":"+password+"@"+ipAddress+":"+port+channel
        self.filePath=filePath


#argv
# username password ipAddress port channel filepath "0(Video)/1(Audio()"
#"admin" "abcd1234" "192.168.31.167" "554" "/h264/ch1/sub/av_stream" "F:/workspace/HikVision/RTSP/test1.wav" "1"

if __name__ == '__main__':

    #"admin" "abcd1234" "192.168.31.167" "554" "/h264/ch1/sub/av_stream" "F:/workspace/HikVision/RTSP/test1.wav" "1"
    if len(sys.argv)!=8:
        sys.exit("need 7 extra parameters")

    kind=("Video" if sys.argv[7]=="0" else "Audio")

    properties=Properties(sys.argv[1],sys.argv[2],sys.argv[3],sys.argv[4],sys.argv[5],sys.argv[6])

    command="python "+"get"+kind+".py "+ properties.rtspURL+" "+properties.filePath
    print(command)


    keyFileName=kind+"-"+sys.argv[3]+".txt"
    file=open(keyFileName,'w')
    file.close()
    
    keyFile=pathlib.Path(keyFileName)
    isContinue=True


    live_stream=subprocess.Popen(command,shell=True,stdin=subprocess.PIPE)

    while(isContinue):
        time.sleep(3)
        isContinue=keyFile.exists()
        print("still continue ? "+str(isContinue))
        
    live_stream.stdin.write('q'.encode("GBK"))
    live_stream.communicate()

    print("Terminate")
