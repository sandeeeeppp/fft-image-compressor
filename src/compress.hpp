#pragma once
#include <opencv2/core.hpp>
#include <ostream>

void compress(const cv::Mat& img, std::ostream& os, int radius, float qbase);
