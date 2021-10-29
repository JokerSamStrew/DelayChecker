#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>

#include <stdio.h>
#include <string>

// compatibility with newer API
#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(55,28,1)
#define av_frame_alloc avcodec_alloc_frame
#define av_frame_free avcodec_free_frame
#endif

void save_frame(AVFrame* pFrame, int width, int height) {
	FILE* pFile;
	char szFilename[32];
	int  y;

	tm newtime;
	const time_t now = time(NULL);
	errno_t err = localtime_s(&newtime, &now);
	std::string str;
	if (err)
		str = "locatime err";

	auto hour = newtime.tm_hour < 10 ? " " + std::to_string(newtime.tm_hour) : std::to_string(newtime.tm_hour);
	auto min = newtime.tm_min < 10 ? "0" + std::to_string(newtime.tm_min) : std::to_string(newtime.tm_min);
	auto sec = newtime.tm_sec < 10 ? "0" + std::to_string(newtime.tm_sec) : std::to_string(newtime.tm_sec);

	// Open file
	sprintf_s(szFilename, "frame%s:%s:%s.ppm", hour.c_str(), min.c_str(), sec.c_str());
	fopen_s(&pFile, szFilename, "wb");
	if (pFile == NULL)
		return;

	// Write header
	fprintf(pFile, "P6\n%d %d\n255\n", width, height);

	// Write pixel data
	for (y = 0; y < height; y++)
		fwrite(pFrame->data[0] + y * pFrame->linesize[0], 1, width * 3, pFile);

	// Close file
	fclose(pFile);
}

int main(int argc, char* argv[]) {
	// Initalizing these to NULL prevents segfaults!
	AVFormatContext* pFormatCtx = NULL;
	int               i, videoStream;
	AVCodecContext* pCodecCtxOrig = NULL;
	AVCodecContext* pCodecCtx = NULL;
	AVCodec* pCodec = NULL;
	AVFrame* pFrame = NULL;
	AVFrame* pFrameRGB = NULL;
	int               numBytes;
	uint8_t* buffer = NULL;
	struct SwsContext* sws_ctx = NULL;

	if (argc < 2) {
		printf("Please provide a movie file\n");
		return -1;
	}

	// Open video file
	if (avformat_open_input(&pFormatCtx, argv[1], NULL, NULL) != 0)
		return -1; // Couldn't open file

	  // Retrieve stream information
	if (avformat_find_stream_info(pFormatCtx, NULL) < 0)
		return -1; // Couldn't find stream information

	  // Dump information about file onto standard error
	av_dump_format(pFormatCtx, 0, argv[1], 0);

	// Find the first video stream
	videoStream = -1;
	for (i = 0; i < pFormatCtx->nb_streams; i++)
		if (pFormatCtx->streams[i]->codecpar->codec_type == AVMediaType::AVMEDIA_TYPE_VIDEO) {
			videoStream = i;
			break;
		}
	if (videoStream == -1)
		return -1; // Didn't find a video stream

	// Find the decoder for the video stream
	pCodec = avcodec_find_decoder(pFormatCtx->streams[videoStream]->codecpar->codec_id);
	if (pCodec == NULL) {
		fprintf(stderr, "Unsupported codec!\n");
		return -1; // Codec not found
	}
	// Copy context
	pCodecCtx = avcodec_alloc_context3(pCodec);
	if (avcodec_parameters_to_context(pCodecCtx, pFormatCtx->streams[videoStream]->codecpar) != 0) {
		fprintf(stderr, "Couldn't copy codec context");
		return -1; // Error copying codec context
	}

	// Open codec
	if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0)
		return -1; // Could not open codec

	  // Allocate video frame
	pFrame = av_frame_alloc();

	// Allocate an AVFrame structure
	pFrameRGB = av_frame_alloc();
	if (pFrameRGB == NULL)
		return -1;

	// Determine required buffer size and allocate buffer
	numBytes = av_image_get_buffer_size(AVPixelFormat::AV_PIX_FMT_RGB24, pCodecCtx->width,
		pCodecCtx->height, 1);

	buffer = (uint8_t*)av_malloc(numBytes * sizeof(uint8_t));

	// Assign appropriate parts of buffer to image planes in pFrameRGB
	// Note that pFrameRGB is an AVFrame, but AVFrame is a superset
	// of AVPicture
	auto dst_data = pFrameRGB->data;
	auto dst_linesize = pFrameRGB->linesize;
	av_image_fill_arrays(dst_data, dst_linesize, buffer, AVPixelFormat::AV_PIX_FMT_RGB24, pCodecCtx->width, pCodecCtx->height, 1);

	// initialize SWS context for software scaling
	sws_ctx = sws_getContext(pCodecCtx->width,
		pCodecCtx->height,
		pCodecCtx->pix_fmt,
		pCodecCtx->width,
		pCodecCtx->height,
		AVPixelFormat::AV_PIX_FMT_RGB24,
		SWS_BILINEAR,
		NULL,
		NULL,
		NULL
	);

	// Read frames and save first five frames to disk
	i = 0;
	while (avcodec_receive_frame(pCodecCtx, pFrame) >= 0) {
		// Is this a packet from the video stream?
		// Did we get a video frame?
		// Convert the image from its native format to RGB
		sws_scale(sws_ctx, (uint8_t const* const*)pFrame->data,
			pFrame->linesize, 0, pCodecCtx->height,
			pFrameRGB->data, pFrameRGB->linesize);

		// Save the frame to disk
		if (++i <= 5)
			save_frame(pFrameRGB, pCodecCtx->width, pCodecCtx->height);
	}

	// Free the RGB image
	av_free(buffer);
	av_frame_free(&pFrameRGB);

	// Free the YUV frame
	av_frame_free(&pFrame);

	// Close the codecs
	avcodec_close(pCodecCtx);

	// Close the video file
	avformat_close_input(&pFormatCtx);

	return 0;
}