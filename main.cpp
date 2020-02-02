#include "filter.h"

#include <iostream>
#include <string>

#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
//#include <opencv2/imgproc.hpp>

using namespace std;
using namespace cv;

static void help(char* progName) 
{
    cout << endl
        << "This program applies median filtering to an image or video" << endl
        << "Usage: " << endl
        << progName << " [path to image/video]  [kernel size -- must be odd and greater than 1] [V -- for video]" << endl;
}

int main(int argc, char* argv[])
{
    if (argc < 3 || !strcmp("help", argv[1]))
    {
        help(argv[0]);
        return -1;
    }

    const string path = argv[1];
    const int ksize = atoi(argv[2]); // kernel size
    Mat image;

    if (argc == 4 && !strcmp("V", argv[3]))
    {
        VideoCapture capt_video; 
        capt_video.open(path);
        if (!capt_video.isOpened())
        {
            cerr << "Can't open video [" << path << "]" << endl;
            return -4;
        }

        const string name = path.substr(0, path.find_last_of('.')) + "_filtered.avi";
        int codec = static_cast<int>(capt_video.get(CAP_PROP_FOURCC));
        union { int v; char c[5]; } uCodec; // from int to char via union
        uCodec.v = codec;
        uCodec.c[4] = '\0';
        Size size = Size((int)capt_video.get(CAP_PROP_FRAME_WIDTH), (int)capt_video.get(CAP_PROP_FRAME_HEIGHT));
        int FPS = (int)capt_video.get(CAP_PROP_FPS);

        VideoWriter target_video;
        target_video.open(name, codec, FPS, size, true);

        if (!target_video.isOpened())
        {
            cout << "Can't open the output video for write [" << path << "]" << endl;
            return -5;
        }
     
        Mat filtered_frame;
        for (;;)
        {
            capt_video >> image;
            if (image.empty())
                break;
            filtered_frame = filter(image, ksize);
            target_video << filtered_frame;
        }
        return 0;
    } 

    image = imread(path, IMREAD_COLOR);

    if (image.empty()) 
    {
        cerr << "Can't open image [" << path << "]" << endl;
        return -2;
    }
    
    Mat res = filter(image, ksize);

    namedWindow("Original image", WINDOW_AUTOSIZE);
    imshow("Original image", image);

    namedWindow("Filtered image", WINDOW_AUTOSIZE);
    imshow("Filtered image", res);

    /*Mat test_res(image.size(), image.type());
    medianBlur(image, test_res, ksize);

    namedWindow("Library filtered", WINDOW_AUTOSIZE);
    imshow("Library filtered", test_res);*/

    waitKey();

    return 0;
}