#include "fftw_plan_manager.hpp"
#include <unordered_map>
#include <stdexcept>
#include <vector>

namespace {
    struct Key { int w, h, kind; };
    struct Hash {
        size_t operator()(const Key& k) const noexcept {
            return (size_t(k.w) << 32) ^ (size_t(k.h) << 1) ^ k.kind;
        }
    };
    struct Eq {
        bool operator()(const Key& a, const Key& b) const noexcept {
            return a.w == b.w && a.h == b.h && a.kind == b.kind;
        }
    };
}

static std::unordered_map<Key, fftwf_plan, Hash, Eq> cache;

FFTPlanManager::FFTPlanManager() { }

FFTPlanManager::~FFTPlanManager() {
    for (auto& kv : cache) fftwf_destroy_plan(kv.second);
    fftwf_cleanup();
}

fftwf_plan FFTPlanManager::getPlan(int w, int h, bool forward) {
    Key k{w, h, forward ? 0 : 1};
    if (auto it = cache.find(k); it != cache.end())
        return it->second;

    // Allocate scratch buffers for planning
    std::vector<float> scratch_in(size_t(w) * h);
    std::vector<fftwf_complex> scratch_out(size_t(h) * (w/2 + 1));

    // Create the plan with real buffer pointers, using FFTW_ESTIMATE
    fftwf_plan p = forward
       ? fftwf_plan_dft_r2c_2d(h, w, scratch_in.data(), scratch_out.data(), FFTW_ESTIMATE)
       : fftwf_plan_dft_c2r_2d(h, w, scratch_out.data(), scratch_in.data(), FFTW_ESTIMATE);

    if (!p) throw std::runtime_error("FFTW plan creation failed");

    cache.emplace(k, p);
    return p;
}

FFTPlanManager& FFTPlanManager::instance() {
    static FFTPlanManager mgr;
    return mgr;
}
