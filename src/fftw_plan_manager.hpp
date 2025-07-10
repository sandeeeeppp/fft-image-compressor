#pragma once
#include <fftw3.h>
// Remove #include <mutex> and std::thread
// Remove any threading-related members

class FFTPlanManager {
public:
    static FFTPlanManager& instance();
    fftwf_plan getPlan(int w, int h, bool forward);
    
private:
    FFTPlanManager();
    ~FFTPlanManager();
    
    // Remove: std::mutex mtx_;
    // Remove: any threading-related members
    
    FFTPlanManager(const FFTPlanManager&) = delete;
    FFTPlanManager& operator=(const FFTPlanManager&) = delete;
};
