
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <iostream>

#ifdef __cplusplus 
extern "C" {
#endif

//Include ffmpeg header file
#include <libavformat/avformat.h> 
#include <libavcodec/avcodec.h> 
#include <libswscale/swscale.h> 

#include <libavutil/imgutils.h>  
#include <libavutil/opt.h>     
#include <libavutil/mathematics.h>   
#include <libavutil/samplefmt.h>
#ifdef __cplusplus
}
#endif

volatile bool isquit = false;


DWORD WINAPI Quit(LPVOID lpParamter) {
	char tmp;
	std::cin >> tmp;
	isquit = true;
	return 0L;
}


int main(int argc, char* argv[]) {
	
	AVFormatContext* pFormatCtx;
	char filepath[] = "rtsp://admin:abcd1234@192.168.31.167:554/h264/ch1/sub/av_stream";
	AVPacket* packet;

	//��ʼ��
	//av_register_all();
	avformat_network_init();
	pFormatCtx = avformat_alloc_context();
	AVDictionary* options = NULL;
	av_dict_set(&options, "buffer_size", "1024000", 0); //���û����С,1080p�ɽ�ֵ�������
	av_dict_set(&options, "rtsp_transport", "tcp", 0); //��udp�ķ�ʽ��,
	av_dict_set(&options, "stimeout", "5000000", 0); //���ó�ʱ�Ͽ�����ʱ�䣬��λus
	av_dict_set(&options, "max_delay", "500000", 0); //�������ʱ��
	packet = (AVPacket*)av_malloc(sizeof(AVPacket));

	//�����������ļ���
	if (avformat_open_input(&pFormatCtx, filepath, NULL, NULL) != 0)
	{
		printf("Couldn't open input stream.\n");
		return -1;
	}

	//����������Ϣ
	if (avformat_find_stream_info(pFormatCtx, NULL) < 0)
	{
		printf("Couldn't find stream information.\n");
		return -2;
	}

	//�����������Ƿ�����Ƶ��
	int videoindex = -1;
	unsigned i = 0;
	for (i = 0; i < pFormatCtx->nb_streams; i++)
		if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
		{
			videoindex = i;
			break;
		}
	if (videoindex == -1)
	{
		printf("Didn't find a video stream.\n");
		return -3;
	}

	//����һ�ε�ʱ����Ƶ��д���ļ���
	FILE* fpSave;
	fpSave = fopen("RTSPsave.h264", "wb+");
	std::cout << "Create save File success!" << std::endl;
	for (int i = 0; i < 400;i++) {
		if (av_read_frame(pFormatCtx, packet) >= 0)
		{
			if (packet->stream_index == videoindex)
			{
				fwrite(packet->data, 1, packet->size, fpSave);
			}
			av_packet_unref(packet);
			//std::cout << "Write success!" << std::endl;
		}
		else {
			std::cout << "read frame failed!"<<std::endl;
			fclose(fpSave);
			av_free(pFormatCtx);
			av_free(packet);
			return -4;
		}
	}

	//HANDLE hThread = CreateThread(NULL, 0, Quit, NULL, 0, NULL);
	//CloseHandle(hThread);

	std::cout << "Stop save RTSP stream" << std::endl;

	fclose(fpSave);
	av_free(pFormatCtx);
	av_free(packet);
	return 0;
}

