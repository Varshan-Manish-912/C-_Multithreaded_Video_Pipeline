#pragma once

#include "Filter.hpp"

class BlurFilter : public Filter {
public:
    cv::Mat apply(const cv::Mat& frame) override;
};