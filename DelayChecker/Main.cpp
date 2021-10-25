#include <opencv2/opencv.hpp>
#include <Windows.h>


int main()
{
	auto conn_str = "rtsp://localhost:5555/stream";
	cv::Mat image;
	cv::VideoCapture reader(conn_str);

	while (!reader.retrieve(image) || image.empty())
	{
		reader.grab();
	}

	reader.release();

	cv::namedWindow(conn_str, cv::WINDOW_AUTOSIZE);
	cv::imshow(conn_str, image);

	cv::waitKey(10000);
	return 0;
}