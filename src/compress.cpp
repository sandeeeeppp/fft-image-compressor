#include "compress.hpp"
#include <iostream>
#include <vector>
#include <cmath>
#include <cstdint>
#include <ostream>

#include "fft.hpp"
#include "mask.hpp"
#include "quant.hpp"
#include "rle.hpp"
#include "huffman.hpp"

void compress(const cv::Mat& img,
              std::ostream&      os,
              int                radius,
              float              qbase)
{
    std::cout << "DEBUG: Starting compression..." << std::endl;
    std::cout << "DEBUG: Image loaded - Size: " << img.cols << "x" << img.rows 
              << ", Channels: " << img.channels() << std::endl;
    
    if (img.empty()) {
        std::cerr << "ERROR: Image is empty!" << std::endl;
        return;
    }

    // Single declaration of W and H
    const uint32_t W = static_cast<uint32_t>(img.cols);
    const uint32_t H = static_cast<uint32_t>(img.rows);
    
    std::cout << "DEBUG: Writing header - W:" << W << " H:" << H << std::endl;

    os.write(reinterpret_cast<const char*>(&W), 4);
    os.write(reinterpret_cast<const char*>(&H), 4);
    os.write(reinterpret_cast<const char*>(&radius), 4);
    os.write(reinterpret_cast<const char*>(&qbase), 4);
    
    if (!os.good()) {
        std::cerr << "ERROR: Failed to write header!" << std::endl;
        return;
    }
    
    std::cout << "DEBUG: Header written successfully, starting FFT..." << std::endl;

    // Enhanced FFT debugging section
    std::cout << "DEBUG: Creating FFT object..." << std::endl;
    FFT2 fft(W, H);

    std::cout << "DEBUG: Allocating spectrum buffer..." << std::endl;
    const size_t specSize = static_cast<size_t>(H) * (W / 2 + 1);
    std::vector<float> specBuf(specSize * 2);
    auto* F = reinterpret_cast<fftwf_complex*>(specBuf.data());

    std::cout << "DEBUG: Calling FFT forward transform..." << std::endl;
    fft.forward(img.ptr<float>(), F);
    std::cout << "DEBUG: FFT forward transform completed!" << std::endl;

    std::cout << "DEBUG: FFT completed, starting quantization..." << std::endl;

    std::vector<int16_t> qvec;
    qvec.reserve(specSize * 2);

    for (size_t idx = 0; idx < specSize; ++idx) {
        const int kx = static_cast<int>(idx % (W / 2 + 1));
        const int ky = static_cast<int>(idx / (W / 2 + 1));

        if (!keep(kx, ky, static_cast<int>(W), static_cast<int>(H), radius)) {
            qvec.push_back(0);
            qvec.push_back(0);
            continue;
        }

        const float qs = qstep(kx, ky, qbase);
        qvec.push_back(static_cast<int16_t>(std::lround(F[idx][0] / qs)));
        qvec.push_back(static_cast<int16_t>(std::lround(F[idx][1] / qs)));
    }

    std::cout << "DEBUG: Quantization completed, starting compression..." << std::endl;

    const auto rle = runLength_encode(qvec);

    BitWriter bw(os);
    Huffman_encode(rle, bw);
    bw.flush();

    std::cout << "DEBUG: Compression completed successfully!" << std::endl;
}
