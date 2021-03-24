import ffmpeg
import sys

# host = 'admin:abcd1234@192.168.31.167:554/h264/ch1/sub/av_stream'# 子进程

if __name__ == '__main__':
    # url  filePath
    if len(sys.argv) !=3:
        sys.exit("need 2 extra parameters")
    process=(
        ffmpeg
            .input(sys.argv[1], rtsp_transport='tcp')
            .output(sys.argv[2])  
            .overwrite_output()
    )
    process.run(capture_stdout=True)