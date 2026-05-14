#include "EdgeFilter.hpp"

cv::Mat EdgeFilter::apply(const cv::Mat& frame) {
    cv::Mat gray;
    cv::Mat blurred;
    cv::Mat edges;
    cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
    cv::GaussianBlur(gray, blurred, cv::Size(5, 5), 0);
    cv::Canny(blurred, edges, 50, 150);
    return edges;
}