#pragma once
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <stdexcept>

inline cv::Mat loadGray(const std::string& path) {
    cv::Mat img = cv::imread(path, cv::IMREAD_GRAYSCALE);
    if (img.empty()) throw std::runtime_error("Cannot load " + path);
    img.convertTo(img, CV_32F);
    return img;
}

inline void saveGray(const std::string& path, const cv::Mat& f32) {
    cv::Mat u8;
    f32.convertTo(u8, CV_8U);
    if (!cv::imwrite(path, u8)) throw std::runtime_error("Write failed");
}
