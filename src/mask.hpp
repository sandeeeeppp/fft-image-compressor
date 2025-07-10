#pragma once
#include <cmath>

inline bool keep(int kx,int ky,int W,int H,int radius) {
    int dx = kx - W/2, dy = ky - H/2;
    return std::sqrt(float(dx*dx + dy*dy)) < radius;
}
