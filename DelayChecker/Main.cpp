#include "Utility.h"
#include <opencv2/opencv.hpp>
#include <atlconv.h>
#include <atlsafe.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib,"Advapi32.lib")

void setup_logger()
{
	const auto pid = std::to_string(GetCurrentProcessId());
	const auto logger = spdlog::basic_logger_st("DC", ".\\DelayChecker_" + pid + ".log");
	spdlog::set_default_logger(logger);
	spdlog::flush_on(spdlog::level::info);
}



void log_capture_props(const cv::VideoCapture& reader)
{

    spdlog::info("{} {}", "CAP_PROP_AUTOFOCUS", reader.get(cv::VideoCaptureProperties::CAP_PROP_AUTOFOCUS));
	spdlog::info("{} {}", "CAP_PROP_AUTO_EXPOSURE", reader.get(cv::VideoCaptureProperties::CAP_PROP_AUTO_EXPOSURE));
	spdlog::info("{} {}", "CAP_PROP_AUTO_WB", reader.get(cv::VideoCaptureProperties::CAP_PROP_AUTO_WB));
	spdlog::info("{} {}", "CAP_PROP_BACKEND", reader.get(cv::VideoCaptureProperties::CAP_PROP_BACKEND));
	spdlog::info("{} {}", "CAP_PROP_BACKLIGHT", reader.get(cv::VideoCaptureProperties::CAP_PROP_BACKLIGHT));
	spdlog::info("{} {}", "CAP_PROP_BITRATE", reader.get(cv::VideoCaptureProperties::CAP_PROP_BITRATE));
	spdlog::info("{} {}", "CAP_PROP_BRIGHTNESS", reader.get(cv::VideoCaptureProperties::CAP_PROP_BRIGHTNESS));
	spdlog::info("{} {}", "CAP_PROP_BUFFERSIZE", reader.get(cv::VideoCaptureProperties::CAP_PROP_BUFFERSIZE));
	spdlog::info("{} {}", "CAP_PROP_CHANNEL", reader.get(cv::VideoCaptureProperties::CAP_PROP_CHANNEL));
	spdlog::info("{} {}", "CAP_PROP_CODEC_PIXEL_FORMAT", reader.get(cv::VideoCaptureProperties::CAP_PROP_CODEC_PIXEL_FORMAT));
	spdlog::info("{} {}", "CAP_PROP_CONTRAST", reader.get(cv::VideoCaptureProperties::CAP_PROP_CONTRAST));
	spdlog::info("{} {}", "CAP_PROP_CONVERT_RGB", reader.get(cv::VideoCaptureProperties::CAP_PROP_CONVERT_RGB));
	spdlog::info("{} {}", "CAP_PROP_EXPOSURE", reader.get(cv::VideoCaptureProperties::CAP_PROP_EXPOSURE));
	spdlog::info("{} {}", "CAP_PROP_FOCUS", reader.get(cv::VideoCaptureProperties::CAP_PROP_FOCUS));
	spdlog::info("{} {}", "CAP_PROP_FORMAT", reader.get(cv::VideoCaptureProperties::CAP_PROP_FORMAT));

	union {
		char    c[5];
		int     i;
	} myfourcc;
	myfourcc.i = static_cast<int>(reader.get(cv::VideoCaptureProperties::CAP_PROP_FOURCC));
	myfourcc.c[4] = '\0';

	spdlog::info("{} {}", "CAP_PROP_FOURCC", myfourcc.c);
	spdlog::info("{} {}", "CAP_PROP_FPS", reader.get(cv::VideoCaptureProperties::CAP_PROP_FPS));
	spdlog::info("{} {}", "CAP_PROP_FRAME_COUNT", reader.get(cv::VideoCaptureProperties::CAP_PROP_FRAME_COUNT));
	spdlog::info("{} {}", "CAP_PROP_FRAME_HEIGHT", reader.get(cv::VideoCaptureProperties::CAP_PROP_FRAME_HEIGHT));
	spdlog::info("{} {}", "CAP_PROP_FRAME_WIDTH", reader.get(cv::VideoCaptureProperties::CAP_PROP_FRAME_WIDTH));
	spdlog::info("{} {}", "CAP_PROP_GAIN", reader.get(cv::VideoCaptureProperties::CAP_PROP_GAIN));
	spdlog::info("{} {}", "CAP_PROP_GAMMA", reader.get(cv::VideoCaptureProperties::CAP_PROP_GAMMA));
	spdlog::info("{} {}", "CAP_PROP_GUID", reader.get(cv::VideoCaptureProperties::CAP_PROP_GUID));
	spdlog::info("{} {}", "CAP_PROP_HUE", reader.get(cv::VideoCaptureProperties::CAP_PROP_HUE));
	spdlog::info("{} {}", "CAP_PROP_IRIS", reader.get(cv::VideoCaptureProperties::CAP_PROP_IRIS));
	spdlog::info("{} {}", "CAP_PROP_ISO_SPEED", reader.get(cv::VideoCaptureProperties::CAP_PROP_ISO_SPEED));
	spdlog::info("{} {}", "CAP_PROP_MODE", reader.get(cv::VideoCaptureProperties::CAP_PROP_MODE));
	spdlog::info("{} {}", "CAP_PROP_MONOCHROME", reader.get(cv::VideoCaptureProperties::CAP_PROP_MONOCHROME));
	spdlog::info("{} {}", "CAP_PROP_PAN", reader.get(cv::VideoCaptureProperties::CAP_PROP_PAN));
	spdlog::info("{} {}", "CAP_PROP_POS_AVI_RATIO", reader.get(cv::VideoCaptureProperties::CAP_PROP_POS_AVI_RATIO));
	spdlog::info("{} {}", "CAP_PROP_POS_FRAMES", reader.get(cv::VideoCaptureProperties::CAP_PROP_POS_FRAMES));
	spdlog::info("{} {}", "CAP_PROP_POS_MSEC", reader.get(cv::VideoCaptureProperties::CAP_PROP_POS_MSEC));
	spdlog::info("{} {}", "CAP_PROP_RECTIFICATION", reader.get(cv::VideoCaptureProperties::CAP_PROP_RECTIFICATION));
	spdlog::info("{} {}", "CAP_PROP_ROLL", reader.get(cv::VideoCaptureProperties::CAP_PROP_ROLL));
	spdlog::info("{} {}", "CAP_PROP_SAR_DEN", reader.get(cv::VideoCaptureProperties::CAP_PROP_SAR_DEN));
	spdlog::info("{} {}", "CAP_PROP_SATURATION", reader.get(cv::VideoCaptureProperties::CAP_PROP_SATURATION));
	spdlog::info("{} {}", "CAP_PROP_SETTINGS", reader.get(cv::VideoCaptureProperties::CAP_PROP_SETTINGS));
	spdlog::info("{} {}", "CAP_PROP_SHARPNESS", reader.get(cv::VideoCaptureProperties::CAP_PROP_SHARPNESS));
	spdlog::info("{} {}", "CAP_PROP_TEMPERATURE", reader.get(cv::VideoCaptureProperties::CAP_PROP_TEMPERATURE));
	spdlog::info("{} {}", "CAP_PROP_TILT", reader.get(cv::VideoCaptureProperties::CAP_PROP_TILT));
	spdlog::info("{} {}", "CAP_PROP_TRIGGER", reader.get(cv::VideoCaptureProperties::CAP_PROP_TRIGGER));
	spdlog::info("{} {}", "CAP_PROP_TRIGGER_DELAY", reader.get(cv::VideoCaptureProperties::CAP_PROP_TRIGGER_DELAY));
	spdlog::info("{} {}", "CAP_PROP_WB_TEMPERATURE", reader.get(cv::VideoCaptureProperties::CAP_PROP_WB_TEMPERATURE));
	spdlog::info("{} {}", "CAP_PROP_WHITE_BALANCE_BLUE_U", reader.get(cv::VideoCaptureProperties::CAP_PROP_WHITE_BALANCE_BLUE_U));
	spdlog::info("{} {}", "CAP_PROP_WHITE_BALANCE_RED_V", reader.get(cv::VideoCaptureProperties::CAP_PROP_WHITE_BALANCE_RED_V));
	spdlog::info("{} {}", "CAP_PROP_ZOOM", reader.get(cv::VideoCaptureProperties::CAP_PROP_ZOOM));
	spdlog::info("{} {}", "CV__CAP_PROP_LATEST", reader.get(cv::VideoCaptureProperties::CV__CAP_PROP_LATEST));

	auto sec = static_cast<int>(reader.get(cv::VideoCaptureProperties::CAP_PROP_POS_MSEC)) / 1000;
	auto frames = static_cast<int>(reader.get(cv::VideoCaptureProperties::CAP_PROP_POS_FRAMES));
	sec = sec == 0 ? 1 : sec;
	frames = frames == 0 ? 1 : frames;
	spdlog::info("{} {}", "Current FPS", frames / sec);
}

void draw_info(const cv::Mat& image, const cv::VideoCapture& reader)
{
	auto sec = static_cast<int>(reader.get(cv::VideoCaptureProperties::CAP_PROP_POS_MSEC)) / 1000;
	auto frames = static_cast<int>(reader.get(cv::VideoCaptureProperties::CAP_PROP_POS_FRAMES));
	sec = sec == 0 ? 1 : sec;
	frames = frames == 0 ? 1 : frames;

	utility::draw_time(image);
	utility::draw_num(image, sec, 0);
	utility::draw_num(image, frames, 1);
	utility::draw_num(image, frames / sec, 2);
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                      _In_opt_ HINSTANCE hPrevInstance,
                      _In_ LPWSTR lpCmdLine,
                      _In_ int nCmdShow)
{
	
	_putenv_s("OPENCV_FFMPEG_CAPTURE_OPTIONS", "rtsp_transport;udp");
	
	int skip_frames_num = 2;
	int next_frame_time_arg = 34;
	auto conn_str = utility::parse_cmd_args(lpCmdLine, next_frame_time_arg);
	cv::Mat image;
	cv::VideoCapture reader(conn_str, cv::CAP_FFMPEG);
	if (!reader.isOpened()){
		spdlog::error("Open failed");
		ExitProcess(1);
	}

	cv::namedWindow(conn_str, cv::WINDOW_FREERATIO);
	if (!reader.read(image))
		cv::resizeWindow(conn_str, 600, 400);
	else
	{
		auto width =  static_cast<int>(reader.get(cv::CAP_PROP_FRAME_WIDTH));
		auto height = static_cast<int>(reader.get(cv::CAP_PROP_FRAME_HEIGHT));
		cv::resizeWindow(conn_str, width, height);
		cv::imshow(conn_str, image);
	}

	int next_frame_time = next_frame_time_arg == 0 ? 1000 / static_cast<int>(reader.get(cv::VideoCaptureProperties::CAP_PROP_FPS)) : next_frame_time_arg;
	setup_logger();
	spdlog::info("{} {}", "getBackendName", reader.getBackendName());
	spdlog::info(reader.getExceptionMode() ? "Exception mode is active" : "Exception mode is not active");
	log_capture_props(reader);

	reader.set(cv::VideoCaptureProperties::CAP_PROP_POS_MSEC, 0);
	while( cv::getWindowProperty(conn_str, cv::WindowPropertyFlags::WND_PROP_FULLSCREEN) != -1 )
	{
		if (!reader.read(image) || image.empty()){
			continue;
		}

		draw_info(image, reader);
		cv::imshow(conn_str, image);
		cv::waitKey(next_frame_time);

		for (size_t i = 0; i < skip_frames_num; i++)
			if ( !reader.grab() )
				break;

	}

	image.release();
	log_capture_props(reader);
	reader.release();
	ExitProcess(0);
}
