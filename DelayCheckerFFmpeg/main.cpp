#include "utility.h"
#include <opencv2/opencv.hpp>
#include <chrono>
#include <thread>
#include <stdio.h>
#include <string>
#include <opencv2/opencv.hpp>
#include <atlconv.h>
#include <atlsafe.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

#include "StreamCapture.h"
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib,"Advapi32.lib")

void setup_logger()
{
	const auto pid = std::to_string(GetCurrentProcessId());
	const auto logger = spdlog::basic_logger_st("DC", ".\\DelayChecker_" + pid + ".log");
	set_default_logger(logger);
	spdlog::flush_on(spdlog::level::info);
}

void draw_info(const cv::Mat& image, const AVFrame* vframe)
{
	auto frames = vframe->coded_picture_number;

	utility::draw_time(image);
	utility::draw_num(image, frames, 1);
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                      _In_opt_ HINSTANCE hPrevInstance,
                      _In_ LPWSTR lpCmdLine,
                      _In_ int nCmdShow)
{
	int next_frame_time_arg = 34;
	auto conn_str = utility::parse_cmd_args(lpCmdLine, next_frame_time_arg);
	StreamCapture stream_capture(conn_str);
	cv::Mat image(stream_capture.height(), stream_capture.width(), CV_8UC3);
	if (!stream_capture.is_opened())
	{
		spdlog::error("Open failed");
		ExitProcess(1);
	}

	AVFrame* v_frame = av_frame_alloc();
	v_frame->format = stream_capture.pix_fmt();
	v_frame->width = stream_capture.width();
	v_frame->height = stream_capture.height();
	v_frame->channels = stream_capture.channels();
	v_frame->channel_layout = stream_capture.channel_layout();
	v_frame->display_picture_number = -1;
	v_frame->coded_picture_number = -1;
	av_frame_get_buffer(v_frame, 0);

	namedWindow(conn_str, cv::WINDOW_FREERATIO);
	if (!stream_capture.retrieve(v_frame))
		cv::resizeWindow(conn_str, 600, 400);
	else
	{
		StreamCapture::av_frame_to_mat(v_frame, image);
		cv::resizeWindow(conn_str, stream_capture.width(), stream_capture.height());
		imshow(conn_str, image);
	}

	int next_frame_time = 15;
	while (!getWindowProperty(conn_str, cv::WindowPropertyFlags::WND_PROP_FULLSCREEN))
	{
		if (!stream_capture.retrieve(v_frame))
		{
			continue;
		}

		draw_info(image, v_frame);
		imshow(conn_str, image);
		StreamCapture::av_frame_to_mat(v_frame, image);
		cv::waitKey(next_frame_time);
	}

	stream_capture.stop();
	image.release();
	av_frame_free(&v_frame);
	ExitProcess(0);
}

