# FFT Image Compressor (Work-in-Progress)

A lightweight C++ command-line tool that compresses images with a 2-D Fast Fourier Transform (FFT), scalar quantisation, run-length encoding and Huffman coding, then reconstructs them with an inverse FFT.

> **Current status:** compression succeeds and produces compact `.bin` files, but the reconstructed images are still *hazy / low-contrast*. Quality improvements are under active development.

---

## Features

| Area                | Implemented |
|---------------------|-------------|
| Core pipeline       | FFT → low-pass mask → quantisation → RLE → Huffman |
| CLI                 | `enc <in.png> <out.bin>` and `dec <in.bin> <out.png>` |
| Build system        | CMake + vcpkg + Visual Studio 2022 |
| External libraries  | FFTW3 (single-thread), OpenCV 4 |
| Platform tested     | Windows 10/11 x64 |

---

## Prerequisites

* Windows + Visual Studio 2022  
* CMake ≥ 3.15  
* Git  
* [vcpkg](https://github.com/microsoft/vcpkg) with ports `fftw3[threads]` and `opencv4`

---

## Build & Run

:: 1 ─ Clone repository
git clone https://github.com/sandeeeeppp/fft-image-compressor.git
cd fft-image-compressor

:: 2 ─ Bootstrap vcpkg (first time only)
cd vcpkg
.\bootstrap-vcpkg.bat
cd ..

:: 3 ─ Install dependencies
.\vcpkg\vcpkg.exe install fftw3[threads] opencv4 --triplet x64-windows

:: 4 ─ Configure & build (Release)
cmake -B build -A x64 -DCMAKE_TOOLCHAIN_FILE="vcpkg/scripts/buildsystems/vcpkg.cmake"
-DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release

:: 5 ─ Copy runtime DLLs next to the executable
cd build\Release
Copy-Item "....\vcpkg\installed\x64-windows\bin*.dll" . -Force

:: 6 ─ Example usage
.\fft_ic.exe enc "C:\path\to\input.png" compressed.bin
.\fft_ic.exe dec compressed.bin reconstructed.png


---

## Difficulties Encountered & Solutions

| Challenge | Fix / Insight |
|-----------|---------------|
| **FFTW deadlocks on Windows** | Switched to single-thread plans and supplied real scratch buffers during plan creation. |
| **0-byte output files** | Added verbose debug logs to verify each pipeline stage and identified silent failures. |
| **Oversize IDE database blocked GitHub push** | Implemented a robust `.gitignore` and rewrote Git history to remove `.vs/` artefacts. |
| **Black images after decode** | Added float-to-8-bit min-max normalisation before writing PNGs. |
| **Complex dependency management** | Standardised on vcpkg and documented one-command install steps. |

---

## Roadmap

* Adaptive quantisation & perceptual weighting to sharpen output  
* Colour-image support (current version handles single-channel images)  
* Command-line flags for `--radius`, `--qbase` and quality presets  
* Continuous-integration build & test on GitHub Actions  
* Performance optimisation (multi-thread FFT path, SIMD)

---

## Contribution

Issues, feature requests and pull requests are welcome—especially around quality tuning and performance improvements. Clone, build, test on your images and share your findings!
