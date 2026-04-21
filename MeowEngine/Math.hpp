//
// Created by Stěpán Toman on 18.04.2026.
//

#ifndef MEOWENGINE_MATH_HPP
#define MEOWENGINE_MATH_HPP

#include "Core.hpp"
namespace Math {

    inline float clamp(float value, float minValue, float maxValue) {
        if (value < minValue) {
            return minValue;
        }
        if (value > maxValue) {
            return maxValue;
        }
        return value;
    }

    inline float clamp(float value, float maxValue) {
        return clamp(value, 0.0f, maxValue);
    }

    inline float lerp(float a, float b, float t) {
        return a + t * (b - a);
    }

    inline float2 bezierPoint(const Curve& curve, float t) {
        t = clamp(t, 0.0f, 1.0f);

        const float2 q0 = {
            lerp(curve.startPoint.x, curve.controlPoint1.x, t),
            lerp(curve.startPoint.y, curve.controlPoint1.y, t)
        };
        const float2 q1 = {
            lerp(curve.controlPoint1.x, curve.controlPoint2.x, t),
            lerp(curve.controlPoint1.y, curve.controlPoint2.y, t)
        };
        const float2 q2 = {
            lerp(curve.controlPoint2.x, curve.endPoint.x, t),
            lerp(curve.controlPoint2.y, curve.endPoint.y, t)
        };

        const float2 r0 = {
            lerp(q0.x, q1.x, t),
            lerp(q0.y, q1.y, t)
        };
        const float2 r1 = {
            lerp(q1.x, q2.x, t),
            lerp(q1.y, q2.y, t)
        };

        return {
            lerp(r0.x, r1.x, t),
            lerp(r0.y, r1.y, t)
        };
    }

    inline int randNumRange(int min, int max) {
        std::random_device dev;
        std::mt19937 rng(dev());
        std::uniform_int_distribution<std::mt19937::result_type> dist6(min,max);

        return dist6(rng);
    }

    inline float randFloatRange(int min, int max, int precision) {
        std::random_device dev;
        std::mt19937 rng(dev());
        std::uniform_real_distribution<float> dist(min, max);

        float scale = std::pow(10.0f, precision);
        return std::round(dist(rng) * scale) / scale;
    }

    inline float randFloat(int precision) {
        return randFloatRange(0, 1, precision);
    }

    // Perlin Noise Implementation
    namespace PerlinNoise {
        static const int permutation[256] = {
            151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,
            140,36,103,30,69,142,8,99,37,240,21,10,23,190,6,148,
            247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,
            57,177,33,88,237,149,56,87,174,20,125,136,171,168,68,175,
            74,165,71,134,139,48,27,166,77,146,158,231,83,111,229,122,
            60,211,133,230,206,39,59,213,250,87,87,26,97,110,189,20,
            252,190,198,15,76,156,37,251,54,63,133,142,97,53,87,185,
            134,193,29,158,225,248,152,17,105,217,142,148,155,30,135,233,
            206,85,40,223,140,161,137,13,191,230,66,104,153,105,102,192,
            210,183,190,57,74,76,88,175,77,149,102,142,203,75,80,16,
            32,160,87,89,19,11,48,112,0,13,117,75,29,19,139,138,
            75,202,110,166,41,41,233,46,6,17,18,4,33,222,59,215,
            103,3,28,27,105,41,76,8,182,138,200,4,160,49,98,14,
            219,213,121,75,120,88,221,231,200,55,109,141,213,78,169,108,
            86,244,234,101,122,174,8,186,120,37,46,28,166,180,198,232,
            221,116,31,75,189,139,138,112,62,181,102,72,3,246,97,116
        };

        static inline float fade(float t) {
            return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
        }

        static inline float grad(int hash, float x, float y) {
            int h = hash & 15;
            float u = h < 8 ? x : y;
            float v = h < 8 ? y : x;
            return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
        }
    }

    inline float perlinNoise(float x, float y) {
        int xi = static_cast<int>(std::floor(x)) & 255;
        int yi = static_cast<int>(std::floor(y)) & 255;

        float xf = x - std::floor(x);
        float yf = y - std::floor(y);

        float u = PerlinNoise::fade(xf);
        float v = PerlinNoise::fade(yf);

        const int* p = PerlinNoise::permutation;

        int aa = p[(p[xi]            + yi)     & 255];
        int ab = p[(p[xi]            + yi + 1) & 255];
        int ba = p[(p[(xi + 1) & 255] + yi)     & 255];
        int bb = p[(p[(xi + 1) & 255] + yi + 1) & 255];

        float x1 = PerlinNoise::grad(aa, xf, yf);
        float x2 = PerlinNoise::grad(ba, xf - 1.0f, yf);
        float y1 = Math::lerp(x1, x2, u);

        x1 = PerlinNoise::grad(ab, xf, yf - 1.0f);
        x2 = PerlinNoise::grad(bb, xf - 1.0f, yf - 1.0f);
        float y2 = Math::lerp(x1, x2, u);

        return Math::lerp(y1, y2, v) * 0.5f + 0.5f;
    }

    // Fractal Brownian Motion — stacks `octaves` layers of Perlin noise
    inline float fractalNoise(float x, float y, int octaves = 4, float persistence = 0.5f, float lacunarity = 2.0f) {
        float value    = 0.0f;
        float amplitude = 1.0f;
        float frequency = 1.0f;
        float maxValue  = 0.0f;
        for (int i = 0; i < octaves; i++) {
            value    += perlinNoise(x * frequency, y * frequency) * amplitude;
            maxValue += amplitude;
            amplitude *= persistence;
            frequency *= lacunarity;
        }
        return value / maxValue;
    }

    // Simplex Noise
    namespace SimplexNoise {
        static const int grad3[12][3] = {
            {1,1,0},{-1,1,0},{1,-1,0},{-1,-1,0},
            {1,0,1},{-1,0,1},{1,0,-1},{-1,0,-1},
            {0,1,1},{0,-1,1},{0,1,-1},{0,-1,-1}
        };
        static inline float dot(const int g[3], float x, float y) {
            return g[0] * x + g[1] * y;
        }
    }

    inline float simplexNoise(float x, float y) {
        const float F2 = 0.5f * (std::sqrt(3.0f) - 1.0f);
        const float G2 = (3.0f - std::sqrt(3.0f)) / 6.0f;

        float s = (x + y) * F2;
        int i = static_cast<int>(std::floor(x + s));
        int j = static_cast<int>(std::floor(y + s));

        float t  = (i + j) * G2;
        float x0 = x - (i - t);
        float y0 = y - (j - t);

        int i1 = (x0 > y0) ? 1 : 0;
        int j1 = (x0 > y0) ? 0 : 1;

        float x1 = x0 - i1 + G2;
        float y1 = y0 - j1 + G2;
        float x2 = x0 - 1.0f + 2.0f * G2;
        float y2 = y0 - 1.0f + 2.0f * G2;

        const int* p = PerlinNoise::permutation;
        int ii = i & 255;
        int jj = j & 255;
        int gi0 = p[(ii +      p[jj]      ) & 255] % 12;
        int gi1 = p[(ii + i1 + p[(jj + j1) & 255]) & 255] % 12;
        int gi2 = p[(ii + 1  + p[(jj + 1)  & 255]) & 255] % 12;

        auto contrib = [&](int gi, float dx, float dy) -> float {
            float t = 0.5f - dx*dx - dy*dy;
            return t < 0.0f ? 0.0f : t*t*t*t * SimplexNoise::dot(SimplexNoise::grad3[gi], dx, dy);
        };

        float raw = 70.0f * (contrib(gi0, x0, y0) + contrib(gi1, x1, y1) + contrib(gi2, x2, y2));
        return raw * 0.5f + 0.5f; // remap [-1,1] -> [0,1]
    }

    // Voronoi (Worley) Noise — returns distance to nearest feature point, [0,1]
    namespace VoronoiNoise {
        static inline float hashX(int ix, int iy) {
            int n = ix * 127 + iy * 311;
            n = (n << 13) ^ n;
            return 0.5f + 0.5f * (1.0f - static_cast<float>((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0f);
        }
        static inline float hashY(int ix, int iy) {
            int n = ix * 269 + iy * 431;
            n = (n << 13) ^ n;
            return 0.5f + 0.5f * (1.0f - static_cast<float>((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0f);
        }
    }

    inline float voronoiNoise(float x, float y) {
        int xi = static_cast<int>(std::floor(x));
        int yi = static_cast<int>(std::floor(y));
        float minDist = 1e10f;
        for (int dy = -1; dy <= 1; dy++) {
            for (int dx = -1; dx <= 1; dx++) {
                int cx = xi + dx;
                int cy = yi + dy;
                float px = cx + VoronoiNoise::hashX(cx, cy);
                float py = cy + VoronoiNoise::hashY(cx, cy);
                float d  = std::sqrt((x - px)*(x - px) + (y - py)*(y - py));
                if (d < minDist) minDist = d;
            }
        }
        return Math::clamp(minDist, 0.0f, 1.0f);
    }

    // Value Noise — smooth random values interpolated across a grid
    namespace ValueNoise {
        static inline float hash(int ix, int iy) {
            int n = ix + iy * 57;
            n = (n << 13) ^ n;
            return (1.0f - static_cast<float>((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0f) * 0.5f + 0.5f;
        }
    }

    inline float valueNoise(float x, float y) {
        int xi = static_cast<int>(std::floor(x));
        int yi = static_cast<int>(std::floor(y));
        float xf = x - std::floor(x);
        float yf = y - std::floor(y);
        float u  = PerlinNoise::fade(xf);
        float v  = PerlinNoise::fade(yf);
        float a  = ValueNoise::hash(xi,     yi);
        float b  = ValueNoise::hash(xi + 1, yi);
        float c  = ValueNoise::hash(xi,     yi + 1);
        float d  = ValueNoise::hash(xi + 1, yi + 1);
        return Math::lerp(Math::lerp(a, b, u), Math::lerp(c, d, u), v);
    }

    // Domain Warp Noise — Perlin coordinates warped by another Perlin sample
    inline float domainWarpNoise(float x, float y, float warpStrength = 1.0f) {
        float wx = perlinNoise(x,        y)        * warpStrength;
        float wy = perlinNoise(x + 5.2f, y + 1.3f) * warpStrength;
        return perlinNoise(x + wx, y + wy);
    }

    inline float2 subtractFloat2(float2 vec, float sub) {
        float num1 = vec.x - sub;
        float num2 = vec.y - sub;
        return {num1, num2};
    }

    inline float2 addFloat2(float2 vec, float add) {
        float num1 = vec.x + add;
        float num2 = vec.y + add;
        return {num1, num2};
    }

    inline float2 subtractFloat2(float2 a, float2 b) {
        return {a.x - b.x, a.y - b.y};
    }

    inline float2 addFloat2(float2 a, float2 b) {
        return {a.x + b.x, a.y + b.y};
    }

    double calculateEquilateralHeight(float side) {
        if (side <= 0) {
            return 0.0;
        }
        return (side * std::sqrt(3.0)) / 2.0;
    }

}

#endif //MEOWENGINE_MATH_HPP
