#include "BlurFilter.hpp"

cv::Mat BlurFilter::apply(const cv::Mat& frame) {

    cv::Mat blurred;
    cv::GaussianBlur(
        frame,
        blurred,
        cv::Size(15, 15),
        0
    );
    return blurred;
}