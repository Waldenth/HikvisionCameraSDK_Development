import ffmpeg
import sys


#rtsp://admin:abcd1234@192.168.31.167:554/h264/ch2/sub/av_stream

if __name__ == '__main__':
    if len(sys.argv) !=3:
        sys.exit("need 2 extra parameters")
    # url filePath
    process=(
        ffmpeg
            .input(sys.argv[1],allowed_media_types='audio', rtsp_transport='tcp')['a']  # allowed_media_types='audio' 只读取音频流
            .filter('volume', 5)  # 音量大小控制
            .output(sys.argv[2], ac=1, ar='16k')  # ac是声道，ar是采样率
            .overwrite_output()
    )
    process.run(capture_stdout=True)