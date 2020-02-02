#include "filter.h"

#include <iostream>

using namespace std;
using namespace cv;

Mat filter(const Mat& image, int ksize) {
    CV_Assert(image.depth() == CV_8U);

    if (ksize < 1 || ksize % 2 == 0)
    {
        cerr << "Kernel size must be odd and greater than 1" << endl;
        exit(-3);
    }

    int padding = ksize / 2; // value for zero padding of source image to solve boundary issues

    Mat padded_image = Mat::zeros(image.rows + 2 * padding, image.cols + 2 * padding, image.type());
    image.copyTo(padded_image(Rect(padding, padding, image.cols, image.rows)));
    Mat target_image(image.size(), image.type());

    Mat source[3];
    split(padded_image, source); // splitting padded source image to 3 channels

    Mat target[3];
    split(target_image, target);

    for (int c = 0; c < 3; c++) { // channels      
        for (int x = 0; x < target[0].cols; x++) {
            for (int y = 0; y < target[0].rows; y++) {
                Rect ROI = Rect(x, y, ksize, ksize);
                Mat k_ROI = source[c](ROI); // kernel matrix
                vector<uchar> vec = {};
                vec.assign(k_ROI.begin<uchar>(), k_ROI.end<uchar>());
                nth_element(vec.begin(), vec.begin() + vec.size() / 2, vec.end()); // partial sorting of vec to get median element
                target[c].at<uchar>(y, x) = vec[vec.size() / 2];
            }
        }
    }
    merge(target, 3, target_image);
    return target_image;
}
