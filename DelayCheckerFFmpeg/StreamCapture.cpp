#include "StreamCapture.h"

#include <spdlog/spdlog.h>

void StreamCapture::read_loop()
{
	int ret;
	while ((ret = av_read_frame(v_format_ctx, v_packet)) >= 0)
	{
		if (v_packet->stream_index != v_stream->index)
		{
			av_packet_unref(v_packet);
			continue;
		}

		ret = avcodec_send_packet(v_codec_ctx, v_packet);
		if (ret < 0 || ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
		{
			spdlog::error("avcodec_send_packet failed {}", ret);
			av_packet_unref(v_packet);
			continue;
		}

		ret = avcodec_receive_frame(v_codec_ctx, v_frame);
		if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
		{
			spdlog::error("avcodec_receive_frame failed {}", ret);
			av_packet_unref(v_packet);
			continue;
		}

		if (v_frame->coded_picture_number % v_codec_ctx->framerate.num * 10)
		{
			spdlog::debug(
				"Frame type {} ({}) pts {} dts {} key_frame {} format {} [coded_picture_number {}, display_picture_number {}]",
				av_get_picture_type_char(v_frame->pict_type),
				v_codec_ctx->frame_number,
				v_frame->pts,
				v_frame->pkt_dts,
				v_frame->key_frame,
				v_frame->format,
				v_frame->coded_picture_number,
				v_frame->display_picture_number
			);
			//std::lock_guard<std::mutex> lock(capture_lock);
			av_frame_copy(current_frame, v_frame);
			av_frame_copy_props(current_frame, v_frame);
		}

		av_packet_unref(v_packet);
	}

	is_stream_opened = false;
	spdlog::error("av_read_frame failed {}", ret);
}

void StreamCapture::av_frame_to_mat(const AVFrame* frame, cv::Mat& image)
{
	const int width = frame->width;
	const int height = frame->height;

	int cv_linesizes[1];
	cv_linesizes[0] = image.step1();

	// Convert the colour format and write directly to the opencv matrix
	SwsContext* conversion = sws_getContext(width, height, static_cast<AVPixelFormat>(frame->format), width, height,
	                                        AV_PIX_FMT_BGR24, SWS_FAST_BILINEAR, nullptr, nullptr, nullptr);
	sws_scale(conversion, frame->data, frame->linesize, 0, height, &image.data, cv_linesizes);
	sws_freeContext(conversion);
}

StreamCapture::StreamCapture(const std::string& source)
{
	if (this->open(source))
	{
		// Allocate video frame
		if (!(current_frame = av_frame_alloc()))
		{
			spdlog::error("frame alloc failed");
			throw std::exception("fail create steamcapture thread");
		}

		current_frame->format = v_codec_ctx->pix_fmt;
		current_frame->width = v_codec_ctx->width;
		current_frame->height = v_codec_ctx->height;
		current_frame->channels = v_codec_ctx->channels;
		current_frame->channel_layout = v_codec_ctx->channel_layout;
		av_frame_get_buffer(current_frame, 0);
		start();
	}
}

StreamCapture::~StreamCapture()
{
	stop();

	// Free the RGB image
	av_packet_free(&v_packet);

	// Free the YUV frame
	if (v_frame != nullptr)
		av_frame_free(&v_frame);

	// Free the YUV frame
	if (current_frame != nullptr)
		av_frame_free(&current_frame);

	// Close the codecs
	avcodec_close(v_codec_ctx);

	// Close the video file
	avformat_close_input(&v_format_ctx);
}

bool StreamCapture::retrieve(AVFrame* copy_frame)
{
	//std::lock_guard<std::mutex> lock(capture_lock);
	if (current_frame->coded_picture_number <= copy_frame->coded_picture_number)
		return false;

	copy_frame->coded_picture_number = current_frame->coded_picture_number;
	auto ret = av_frame_copy(copy_frame, current_frame);
	if (ret < 0)
	{
		spdlog::error("av_frame_copy fail");
		return false;
	}

	return true;
}

bool StreamCapture::open(const std::string& source)
{
	is_stream_opened = false;
	spdlog::info("StreamCapture::open");

	auto ret = avformat_open_input(&v_format_ctx, source.c_str(), nullptr, nullptr) == 0;
	if (!ret)
	{
		spdlog::error("Couldn't open stream");
		return false;
	}

	ret = avformat_find_stream_info(v_format_ctx, nullptr) >= 0;
	if (!ret)
	{
		spdlog::error("Couldn't find stream information");
		return false;
	}

	// Find the first video stream
	int video_stream_id = -1;
	for (auto i = 0; i < v_format_ctx->nb_streams; i++)
		if (v_format_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			video_stream_id = i;
			break;
		}

	ret = video_stream_id != -1;
	if (!ret)
	{
		spdlog::error("Didn't find a video stream");
		return false;
	}

	v_stream = v_format_ctx->streams[video_stream_id];

	// Find the decoder for the video stream
	v_codec = avcodec_find_decoder(v_stream->codecpar->codec_id);
	ret = v_codec != nullptr;
	if (!ret)
	{
		spdlog::error("Unsupported codec!");
		return false;
	}

	v_codec_ctx = avcodec_alloc_context3(v_codec);
	ret = v_codec_ctx;
	if (!ret)
	{
		spdlog::error("Could not allocate video codec context");
		return false;
	}

	// Copy context
	ret = avcodec_parameters_to_context(v_codec_ctx, v_stream->codecpar) == 0;
	if (!ret)
	{
		spdlog::error("Couldn't copy codec context");
		return false;
	}

	// Open codec
	ret = avcodec_open2(v_codec_ctx, v_codec, nullptr) >= 0;
	if (!ret)
	{
		spdlog::error("Could not open codec");
		return false;
	}

	// Allocate video frame
	v_frame = av_frame_alloc();
	ret = v_frame != nullptr;
	if (!ret)
	{
		spdlog::error("frame alloc failed");
		return false;
	}

	v_packet = av_packet_alloc();
	ret = v_packet != nullptr;
	if (!ret)
	{
		spdlog::error("packet alloc failed");
		return false;
	}

	const auto pars = v_stream->codecpar;
	spdlog::info("Video Codec: resolution {} x {} framerate {}", pars->width, pars->height,
	             v_codec_ctx->framerate.num);
	spdlog::info("Codec {} ID {} bit_rate {} ", v_codec->long_name, v_codec->id, pars->bit_rate);

	is_stream_opened = ret;
	return ret;
}

void StreamCapture::start()
{
	if (work_thread != nullptr)
		stop();

	work_thread = new std::thread(&StreamCapture::read_loop, this);
	if (work_thread == nullptr)
		throw std::exception("fail create steamcapture thread");
}

void StreamCapture::stop()
{
	delete work_thread;
	work_thread = nullptr;
}

bool StreamCapture::is_opened()
{
	return is_stream_opened;
}

int StreamCapture::width()
{
	return v_codec_ctx != nullptr ? v_codec_ctx->width : 0;
}

AVPixelFormat StreamCapture::pix_fmt()
{
	return v_codec_ctx != nullptr ? v_codec_ctx->pix_fmt : AV_PIX_FMT_NONE;
}

int StreamCapture::channels()
{
	return v_codec_ctx != nullptr ? v_codec_ctx->channels : -1;
}

uint64_t StreamCapture::channel_layout()
{
	return v_codec_ctx != nullptr ? v_codec_ctx->channel_layout : 0;
}

int StreamCapture::height()
{
	return v_codec_ctx != nullptr ? v_codec_ctx->height : 0;
}
