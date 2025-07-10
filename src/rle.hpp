#pragma once
#include <vector>
#include <cstdint>

inline std::vector<int16_t> runLength_encode(const std::vector<int16_t>& in) {
    std::vector<int16_t> out;
    for (size_t i=0;i<in.size();) {
        int16_t v = in[i];
        size_t j=i;
        while(j<in.size() && in[j]==v && (j-i)<32767) ++j;
        out.push_back(v);
        out.push_back(int16_t(j-i));
        i=j;
    }
    return out;
}

inline std::vector<int16_t> runLength_decode(const std::vector<int16_t>& in) {
    std::vector<int16_t> out;
    for (size_t i=0;i+1<in.size(); i+=2) {
        int16_t v = in[i], c = in[i+1];
        for (int j=0;j<c;++j) out.push_back(v);
    }
    return out;
}
