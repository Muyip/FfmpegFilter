#include <iostream>
#include <fstream>
#include <sstream>
#include <FfmpegFilter.h>

int main()
{
    std::string inputYuvPath = "video_1280x720.yuv";
    std::string outputYuvPath = "output_1280x720.yuv";
    int inputWidth = 1280;
    int inputHeight = 720;

    int boxPointX = 100;
    int boxPointY = 200;
    int boxWidth = 400;
    int boxHeight = 300;

    std::ifstream ifs(inputYuvPath, std::ios::in | std::ios::binary);
    std::ofstream ofs(outputYuvPath, std::ios::out | std::ios::binary);
    unsigned char* frameBuffer = new unsigned char[inputWidth * inputHeight * 3 / 2] {0};
    while (true)
    {
        ifs.read((char*)frameBuffer, inputWidth * inputHeight * 3 / 2);

        if (ifs.eof())
        {
            break;
        }

        {
            std::stringstream ss;
            ss << "drawbox=x=" << boxPointX << ":y=" << boxPointY
               << ":w=" << boxWidth << ":h=" << boxHeight
               << ":color=" << "#00FF00" << "@0.5:t=" << 5;

            FfmpegFilter filter;
            if (filter.init(ss.str(), inputWidth, inputHeight) == 0)
            {
                filter.filter(frameBuffer);
            }
        }

        {
            std::stringstream ss;
            ss << "drawtext=fontfile=weiruanyahei.ttf:fontcolor=" << "#FFFFFF"
               << ":fontsize=" << 30 << ":text=" << "FfmpegFilter" << ":x=" << boxPointX + 20
               << ":y=" << std::to_string(boxPointY + boxHeight - 24) << "-th";

            FfmpegFilter filter;
            if (filter.init(ss.str(), inputWidth, inputHeight) == 0)
            {
                filter.filter(frameBuffer);
            }
        }

        ofs.write((char*)frameBuffer, inputWidth * inputHeight * 3 / 2);
    }

    delete[] frameBuffer;
    return 0;
}
