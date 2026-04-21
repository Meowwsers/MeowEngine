//
// Created by Stěpán Toman on 19.04.2026.
//

#ifndef MEOWENGINE_GENERATENOISE_HPP
#define MEOWENGINE_GENERATENOISE_HPP

#include "Core.hpp"
#include "Math.hpp"
#include <future>
#include <thread>

// Splits pixel rows across hardware threads and fills colorData in parallel.
// 'fillPixel' receives (x, y) and returns a Color.
template<typename FillFn>
inline Texture generateParallel(int w, int h, FillFn fillPixel) {
    Texture image;
    image.w = w;
    image.h = h;
    image.colorData.resize(w * h);

    const int threadCount = std::max(1u, std::thread::hardware_concurrency());
    const int rowsPerThread = (h + threadCount - 1) / threadCount;

    std::vector<std::future<void>> futures;
    futures.reserve(threadCount);

    for (int t = 0; t < threadCount; t++) {
        int rowStart = t * rowsPerThread;
        int rowEnd   = std::min(rowStart + rowsPerThread, h);
        if (rowStart >= h) break;

        futures.push_back(std::async(std::launch::async, [&, rowStart, rowEnd]() {
            for (int y = rowStart; y < rowEnd; y++)
                for (int x = 0; x < w; x++)
                    image.colorData[y * w + x] = fillPixel(x, y);
        }));
    }

    for (auto& f : futures) f.get();
    return image;
}

inline Texture blankImage(int w, int h) {
    return generateParallel(w, h, [](int, int) -> Color { return WHITE; });
}

inline Texture generateWhiteNoise(int w, int h) {
    return generateParallel(w, h, [](int, int) -> Color {
        return {0, 0, 0, static_cast<Uint8>(Math::randNumRange(1, 255))};
    });
}

inline Texture generateBooleanNoise(int w, int h) {
    return generateParallel(w, h, [](int, int) -> Color {
        return Math::randNumRange(0, 1) == 0 ? BLACK : WHITE;
    });
}

inline Texture generateColorNoise(int w, int h) {
    return generateParallel(w, h, [](int, int) -> Color {
        return {
            static_cast<Uint8>(Math::randNumRange(1, 255)),
            static_cast<Uint8>(Math::randNumRange(1, 255)),
            static_cast<Uint8>(Math::randNumRange(1, 255)),
            255
        };
    });
}

inline Texture generatePerlinNoise(int w, int h, float scale = 4.0f, float offsetX = 0.0f, float offsetY = 0.0f) {
    return generateParallel(w, h, [=](int x, int y) -> Color {
        float nx = (static_cast<float>(x) / w) * scale + offsetX;
        float ny = (static_cast<float>(y) / h) * scale + offsetY;
        Uint8 v = static_cast<Uint8>(Math::perlinNoise(nx, ny) * 255);
        return {v, v, v, 255};
    });
}

// Fractal Brownian Motion — stacks multiple Perlin octaves for detail
inline Texture generateFractalNoise(int w, int h, float scale = 4.0f, int octaves = 4, float offsetX = 0.0f, float offsetY = 0.0f) {
    return generateParallel(w, h, [=](int x, int y) -> Color {
        float nx = (static_cast<float>(x) / w) * scale + offsetX;
        float ny = (static_cast<float>(y) / h) * scale + offsetY;
        Uint8 v = static_cast<Uint8>(Math::clamp(Math::fractalNoise(nx, ny, octaves), 0.0f, 1.0f) * 255);
        return {v, v, v, 255};
    });
}

// Simplex Noise — faster and smoother than Perlin with fewer artefacts
inline Texture generateSimplexNoise(int w, int h, float scale = 4.0f, float offsetX = 0.0f, float offsetY = 0.0f) {
    return generateParallel(w, h, [=](int x, int y) -> Color {
        float nx = (static_cast<float>(x) / w) * scale + offsetX;
        float ny = (static_cast<float>(y) / h) * scale + offsetY;
        Uint8 v = static_cast<Uint8>(Math::clamp(Math::simplexNoise(nx, ny), 0.0f, 1.0f) * 255);
        return {v, v, v, 255};
    });
}

// Voronoi (Worley) Noise — cell-based, great for stone/water patterns
inline Texture generateVoronoiNoise(int w, int h, float scale = 4.0f, float offsetX = 0.0f, float offsetY = 0.0f) {
    return generateParallel(w, h, [=](int x, int y) -> Color {
        float nx = (static_cast<float>(x) / w) * scale + offsetX;
        float ny = (static_cast<float>(y) / h) * scale + offsetY;
        Uint8 v = static_cast<Uint8>(Math::clamp(Math::voronoiNoise(nx, ny), 0.0f, 1.0f) * 255);
        return {v, v, v, 255};
    });
}

// Value Noise — simpler smooth noise, similar look to Perlin with less compute
inline Texture generateValueNoise(int w, int h, float scale = 4.0f, float offsetX = 0.0f, float offsetY = 0.0f) {
    return generateParallel(w, h, [=](int x, int y) -> Color {
        float nx = (static_cast<float>(x) / w) * scale + offsetX;
        float ny = (static_cast<float>(y) / h) * scale + offsetY;
        Uint8 v = static_cast<Uint8>(Math::clamp(Math::valueNoise(nx, ny), 0.0f, 1.0f) * 255);
        return {v, v, v, 255};
    });
}

// Domain Warp Noise — Perlin warped by Perlin, produces flowing/marble patterns
inline Texture generateDomainWarpNoise(int w, int h, float scale = 4.0f, float warpStrength = 1.0f, float offsetX = 0.0f, float offsetY = 0.0f) {
    return generateParallel(w, h, [=](int x, int y) -> Color {
        float nx = (static_cast<float>(x) / w) * scale + offsetX;
        float ny = (static_cast<float>(y) / h) * scale + offsetY;
        Uint8 v = static_cast<Uint8>(Math::clamp(Math::domainWarpNoise(nx, ny, warpStrength), 0.0f, 1.0f) * 255);
        return {v, v, v, 255};
    });
}

#endif //MEOWENGINE_GENERATENOISE_HPP
