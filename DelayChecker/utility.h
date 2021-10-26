#pragma once
#include <ctime>
#include <opencv2/opencv.hpp>
#include <atlconv.h>
#include <atlsafe.h>
#include <Windows.h>


class utility
{
public:
	static void draw_time(const cv::Mat& image);
	static void draw_num(const cv::Mat& image, int num);
	static std::string parse_cmd_args(const LPWSTR& lpCmdLine, int& next_frame_time);
};

