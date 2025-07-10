#pragma once
#include <cmath>

inline float qstep(int kx,int ky,float base) {
    return base * (1.f + 0.01f*(std::abs(kx) + std::abs(ky)));
}
