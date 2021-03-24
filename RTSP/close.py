import os
import sys
# "0(Video)/1(Audio()" , ipAddress
# "0" "192.168.31.167"
if __name__ == '__main__':
    if len(sys.argv)!=3:
        sys.exit("need 2 extra parameters")

    kind=("Video" if sys.argv[1]=="0" else "Audio")
    TargetFile=kind+"-"+sys.argv[2]+".txt"
    os.remove(TargetFile)