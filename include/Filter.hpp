#pragma once

#include <opencv2/opencv.hpp>

class Filter {
public:
    virtual ~Filter() = default;
    virtual cv::Mat apply(const cv::Mat& frame) = 0;
};