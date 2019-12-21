#include "FfmpegFilter.h"

FfmpegFilter::FfmpegFilter()
    : _fmtCtx(nullptr)
    , _decCtx(nullptr)
    , _buffersrcCtx(nullptr)
    , _buffersinkCtx(nullptr)
    , _filterGraph(nullptr)
{
}

FfmpegFilter::~FfmpegFilter()
{
    avfilter_graph_free(&_filterGraph);
    avcodec_free_context(&_decCtx);
    avformat_close_input(&_fmtCtx);
}

int FfmpegFilter::init(const std::string& filterDescr, int width, int height)
{
    _width = width;
    _height = height;

    int ret = true;
    const AVFilter *buffersrc  = avfilter_get_by_name("buffer");
    const AVFilter *buffersink = avfilter_get_by_name("buffersink");
    AVFilterInOut *outputs = avfilter_inout_alloc();
    AVFilterInOut *inputs  = avfilter_inout_alloc();
    enum AVPixelFormat pixFmts[] = { AV_PIX_FMT_YUV420P, AV_PIX_FMT_NONE };
    _filterGraph = avfilter_graph_alloc();

    if (!outputs || !inputs || !_filterGraph) {
        ret = AVERROR(ENOMEM);
        goto end;
    }

    char args[512];
    snprintf(args, sizeof(args),
            "video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d",
            width, height, AV_PIX_FMT_YUV420P, 1, 25, 1, 1);

    ret = avfilter_graph_create_filter(&_buffersrcCtx, buffersrc, "in",
                                       args, NULL, _filterGraph);
    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot create buffer source\n");
        goto end;
    }

    ret = avfilter_graph_create_filter(&_buffersinkCtx, buffersink, "out",
                                       NULL, NULL, _filterGraph);
    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot create buffer sink\n");
        goto end;
    }

    ret = av_opt_set_int_list(_buffersinkCtx, "pix_fmts", pixFmts,
                              AV_PIX_FMT_NONE, AV_OPT_SEARCH_CHILDREN);
    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot set output pixel format\n");
        goto end;
    }

    outputs->name       = av_strdup("in");
    outputs->filter_ctx = _buffersrcCtx;
    outputs->pad_idx    = 0;
    outputs->next       = NULL;

    inputs->name       = av_strdup("out");
    inputs->filter_ctx = _buffersinkCtx;
    inputs->pad_idx    = 0;
    inputs->next       = NULL;

    if ((ret = avfilter_graph_parse_ptr(_filterGraph, filterDescr.c_str(), &inputs, &outputs, NULL)) < 0)
    {
        goto end;
    }

    if ((ret = avfilter_graph_config(_filterGraph, NULL)) < 0)
    {
        goto end;
    }

end:
    avfilter_inout_free(&inputs);
    avfilter_inout_free(&outputs);

    return ret;
}

void FfmpegFilter::filter(unsigned char* frameInData) 
{
    AVFrame* avFrameIn = av_frame_alloc();
    AVFrame* avFrameOut = av_frame_alloc();

    av_image_fill_arrays(avFrameIn->data, avFrameIn->linesize, NULL, AV_PIX_FMT_YUV420P, _width, _height, 1);

    unsigned char* frameBufferTmp = (unsigned char *)av_malloc(av_image_get_buffer_size(AV_PIX_FMT_YUV420P, _width, _height, 1));

    av_image_fill_arrays(avFrameOut->data, avFrameOut->linesize, frameBufferTmp, AV_PIX_FMT_YUV420P, _width, _height, 1);

    avFrameIn->width = _width;
    avFrameIn->height = _height;
    avFrameIn->format = AV_PIX_FMT_YUV420P;
    avFrameIn->data[0] = (uint8_t*)frameInData; 
    avFrameIn->data[1] = (uint8_t*)frameInData + _width * _height;
    avFrameIn->data[2] = (uint8_t*)frameInData + _width * _height * 5 / 4;

    if (av_buffersrc_add_frame_flags(_buffersrcCtx, avFrameIn, AV_BUFFERSRC_FLAG_KEEP_REF) < 0)
    {
        return; 
    }
    if (av_buffersink_get_frame(_buffersinkCtx, avFrameOut) < 0)
    {
        return;
    }

    if (avFrameOut->format == AV_PIX_FMT_YUV420P)
    {
        memcpy(frameInData, avFrameOut->data[0], _width * _height);
        memcpy(frameInData + _width * _height, avFrameOut->data[1], _width * _height / 4);
        memcpy(frameInData + _width * _height * 5 / 4, avFrameOut->data[2], _width * _height / 4);
    }

    av_frame_unref(avFrameIn);
    av_frame_unref(avFrameOut);
}

