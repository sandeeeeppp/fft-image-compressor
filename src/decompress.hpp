#pragma once
#include <opencv2/core.hpp>
#include <istream>

cv::Mat decompress(std::istream& is);
