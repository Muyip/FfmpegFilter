#ifndef FfmpegFilter_INCLUDE
#define FfmpegFilter_INCLUDE

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>
#include <libavutil/opt.h>
#include <libavutil/imgutils.h>
}


class FfmpegFilter
{
public:
    FfmpegFilter();

    virtual ~FfmpegFilter();

    int init(const std::string& filterDescr, int width, int height);

    void filter(unsigned char* frameInData);

private:
    AVFormatContext* _fmtCtx;
    AVCodecContext*  _decCtx;
    AVFilterContext* _buffersrcCtx;
    AVFilterContext* _buffersinkCtx;
    AVFilterGraph*   _filterGraph;
    int              _width;
    int              _height;
};

#endif  // FfmpegFilter_INCLUDE
