#pragma once
#include <string>
#include <opencv2/opencv.hpp>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>

#pragma comment (lib,"swscale.lib")
#pragma comment (lib,"avformat.lib")
#pragma comment (lib,"avcodec.lib")
}


class StreamCapture
{
private:
	AVFormatContext* v_format_ctx = nullptr;
	AVStream* v_stream = nullptr;
	AVCodecContext* v_codec_ctx = nullptr;
	AVCodec* v_codec = nullptr;
	AVFrame* v_frame = nullptr;
	AVFrame* current_frame = nullptr;
	AVPacket* v_packet = nullptr;
	std::mutex capture_lock;
	bool is_stream_opened = false;
	std::thread* work_thread;
	void read_loop();


public:
	void start();
	void stop();
	static void av_frame_to_mat(const AVFrame* frame, cv::Mat& image);
	StreamCapture(const std::string& source);
	~StreamCapture();
	bool retrieve(AVFrame* copy_frame);
	bool open(const std::string& source);
	bool is_opened();
	int width();
	AVPixelFormat pix_fmt();
	int channels();
	uint64_t channel_layout();
	int height();
};
