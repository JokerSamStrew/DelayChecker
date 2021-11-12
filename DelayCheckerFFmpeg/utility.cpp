#include "utility.h"
#include "shellapi.h"

void utility::draw_time(const cv::Mat& image)
{
	tm newtime;
	const time_t now = time(nullptr);
	errno_t err = localtime_s(&newtime, &now);
	std::string str;
	if (err)
		str = "locatime err";

	auto hour = newtime.tm_hour < 10 ? " " + std::to_string(newtime.tm_hour) : std::to_string(newtime.tm_hour);
	auto min = newtime.tm_min < 10 ? "0" + std::to_string(newtime.tm_min) : std::to_string(newtime.tm_min);
	auto sec = newtime.tm_sec < 10 ? "0" + std::to_string(newtime.tm_sec) : std::to_string(newtime.tm_sec);

	str = hour + ":" + min + ":" + sec;

	auto point = cv::Point(20, 70);
	auto font_scale = 1.1;
	putText(image, str, point, cv::FONT_HERSHEY_DUPLEX, font_scale, cv::Scalar(0, 0, 0), 6, cv::LINE_AA);
	putText(image, str, point, cv::FONT_HERSHEY_DUPLEX, font_scale, cv::Scalar(255, 255, 255), 1, cv::LINE_AA);
}

void utility::draw_num(const cv::Mat& image, int num, int draw_step)
{
	tm newtime;
	const time_t now = time(nullptr);
	errno_t err = localtime_s(&newtime, &now);
	std::string str = std::to_string(num);
	auto point = cv::Point(20, 110 + 25 * draw_step);
	auto font_scale = 0.7;
	putText(image, str, point, cv::FONT_HERSHEY_DUPLEX, font_scale, cv::Scalar(0, 0, 0), 6, cv::LINE_AA);
	putText(image, str, point, cv::FONT_HERSHEY_DUPLEX, font_scale, cv::Scalar(255, 255, 255), 1, cv::LINE_AA);
}


std::string utility::parse_cmd_args(const LPWSTR& lpCmdLine, int& next_frame_time)
{
	auto args_count = 0;
	const auto* sz_arg_list = CommandLineToArgvW(lpCmdLine, &args_count);
	auto conn_str = std::string(CW2A(sz_arg_list[0]));
	if (args_count == 2)
		next_frame_time = atoi(CW2A(sz_arg_list[1]));

	return conn_str.find("DelayChecker.exe") != std::string::npos ? "rtsp://localhost:5555/stream" : conn_str;
}
