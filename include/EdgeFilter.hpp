#pragma once

#include "Filter.hpp"

class EdgeFilter : public Filter {
public:
    cv::Mat apply(const cv::Mat& frame) override;
};