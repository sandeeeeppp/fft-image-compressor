#include <iostream>
#include <fstream>
#include <string>
#include <opencv2/core.hpp>
#include "image.hpp"
#include "compress.hpp"
#include "decompress.hpp"

int compressor_cli(int argc, char** argv) {
    if (argc < 4) {
        std::cout << "Usage:\n"
                  << "  fft_ic enc <in.png> <out.bin>\n"
                  << "  fft_ic dec <in.bin> <out.png>\n";
        return 1;
    }
    std::string mode = argv[1];
    try {
        if (mode == "enc") {
            cv::Mat img = loadGray(argv[2]);
            std::ofstream ofs(argv[3], std::ios::binary);
            compress(img, ofs, /*radius=*/60, /*qbase=*/8.f);
        } else if (mode == "dec") {
            std::ifstream ifs(argv[2], std::ios::binary);
            cv::Mat img = decompress(ifs);
            saveGray(argv[3], img);
        } else {
            throw std::runtime_error("Unknown mode");
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 2;
    }
    return 0;
}
