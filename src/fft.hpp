#pragma once
#include <vector>
#include <iostream>
#include <fftw3.h>
#include "fftw_plan_manager.hpp"

class FFT2 {
public:
    FFT2(int W, int H) : w(W), h(H) {
        buf_real.resize(size_t(w) * h);
    }

    void forward(const float* spatial, fftwf_complex* spec) const {
        std::copy(spatial, spatial + w * h, buf_real.begin());
        
        fftwf_plan plan = FFTPlanManager::instance().getPlan(w, h, true);
        fftwf_execute_dft_r2c(plan, buf_real.data(), spec);
    }

    void inverse(const fftwf_complex* spec, float* spatial) const {
        fftwf_plan plan = FFTPlanManager::instance().getPlan(w, h, false);
        fftwf_execute_dft_c2r(plan, const_cast<fftwf_complex*>(spec), buf_real.data());

        float scale = 1.f / (w * h);
        for (size_t i = 0; i < buf_real.size(); ++i) {
            spatial[i] = buf_real[i] * scale;
        }
    }

private:
    int w, h;
    mutable std::vector<float> buf_real;
};
