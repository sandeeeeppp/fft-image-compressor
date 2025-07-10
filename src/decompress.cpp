#include "decompress.hpp"

#include <vector>
#include <stdexcept>
#include <cstdint>

#include <opencv2/imgproc.hpp>

#include "fft.hpp"
#include "mask.hpp"
#include "quant.hpp"
#include "rle.hpp"
#include "huffman.hpp"

cv::Mat decompress(std::istream& is)
{
    // 1. Read header written by the encoder
    uint32_t W = 0, H = 0;
    int      radius = 0;
    float    qbase  = 0.f;

    is.read(reinterpret_cast<char*>(&W), 4);
    is.read(reinterpret_cast<char*>(&H), 4);
    is.read(reinterpret_cast<char*>(&radius), 4);
    is.read(reinterpret_cast<char*>(&qbase), 4);
    if (is.fail()) throw std::runtime_error("corrupt or incomplete header");

    // 2. Huffman → RLE → coefficient vector
    BitReader br(is);
    const auto rle  = Huffman_decode(br);
    const auto qvec = runLength_decode(rle);

    const size_t specSize = static_cast<size_t>(H) * (W / 2 + 1);
    if (qvec.size() != specSize * 2)
        throw std::runtime_error("size check failed: bad bit-stream");

    // 3. Re-assemble complex spectrum
    std::vector<float> specBuf(specSize * 2);
    auto* F = reinterpret_cast<fftwf_complex*>(specBuf.data());

    for (size_t idx = 0; idx < specSize; ++idx) {
        int kx = static_cast<int>(idx % (W / 2 + 1));
        int ky = static_cast<int>(idx / (W / 2 + 1));
        float q = qstep(kx, ky, qbase);
        F[idx][0] = qvec[2*idx]     * q;  // real
        F[idx][1] = qvec[2*idx + 1] * q;  // imag
    }

    // 4. Inverse FFT → float image
    FFT2 fft(W, H);
    cv::Mat outF(static_cast<int>(H), static_cast<int>(W), CV_32F);
    fft.inverse(F, outF.ptr<float>());

    // 5. Normalize and convert to 8-bit
    double minVal, maxVal;
    cv::minMaxLoc(outF, &minVal, &maxVal);

    // Avoid divide-by-zero
    if (maxVal - minVal < 1e-6) {
        // All pixels the same; map to mid-gray
        cv::Mat gray(outF.size(), CV_8U, cv::Scalar(128));
        return gray;
    }

    // Scale float [minVal..maxVal] to [0..255]
    cv::Mat out8;
    outF.convertTo(out8, CV_8U,
                   255.0 / (maxVal - minVal),
                   -255.0 * minVal / (maxVal - minVal));

    return out8;
}
