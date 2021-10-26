#include <opencv2/opencv.hpp>
#include <ctime>
#include <Windows.h>
#include <atlconv.h>
#include <atlsafe.h>



void draw_time(const cv::Mat& image)
{
	tm newtime;
	const time_t now = time(NULL);
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
	cv::putText(image, str, point, cv::FONT_HERSHEY_DUPLEX, font_scale, cv::Scalar(0, 0, 0), 6, cv::LINE_AA);
	cv::putText(image, str, point, cv::FONT_HERSHEY_DUPLEX, font_scale, cv::Scalar(255, 255, 255), 1, cv::LINE_AA );
}


std::string parse_cmd_args(const LPWSTR& lpCmdLine, int& next_frame_time)
{
	auto args_count = 0;
	const auto* sz_arg_list = CommandLineToArgvW(lpCmdLine, &args_count);
	auto conn_str = std::string(ATL::CW2A(sz_arg_list[0]));
	if (args_count == 2)
		next_frame_time = atoi(ATL::CW2A(sz_arg_list[1]));

	return conn_str.find("DelayChecker.exe") != std::string::npos ? "rtsp://localhost:5555/stream" : conn_str;
}


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                      _In_opt_ HINSTANCE hPrevInstance,
                      _In_ LPWSTR lpCmdLine,
                      _In_ int nCmdShow)
{
	int next_frame_time = 32;
	int skip_frames_num = 2;
	auto conn_str = parse_cmd_args(lpCmdLine, next_frame_time);
	cv::Mat image;
	cv::VideoCapture reader(conn_str);
	std::cout << reader.getBackendName() << std::endl;
	std::cout << ( reader.getExceptionMode() ? "Exception mode is active" : "Exception mode is not active" ) << std::endl;
	cv::namedWindow(conn_str, cv::WINDOW_FREERATIO);
	if (!reader.read(image))
		cv::resizeWindow(conn_str, 600, 400);
	else
	{
		auto width =  static_cast<int>(reader.get(cv::CAP_PROP_FRAME_WIDTH));
		auto height = static_cast<int>(reader.get(cv::CAP_PROP_FRAME_HEIGHT));
		std::cout << "Width: "<< width << " Height: " << height << std::endl;
		cv::resizeWindow(conn_str, width, height);
	}

	while( cv::getWindowProperty(conn_str, cv::WindowPropertyFlags::WND_PROP_FULLSCREEN) != -1 )
	{
		if (!reader.read(image)){
			for (size_t i = 0; i < skip_frames_num; i++)
				reader.grab();
			continue;
		}
		
		draw_time(image);
		cv::imshow(conn_str, image);
		cv::waitKey(next_frame_time);
		image.release();
	}

	reader.release();
	ExitProcess(0);
}