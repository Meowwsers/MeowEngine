//
// Created by Stěpán Toman on 17.04.2026.
//

#ifndef MEOWENGINE_CORE_HPP
#define MEOWENGINE_CORE_HPP

#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_image/SDL_image.h>
#include <algorithm>
#include <random>

// Credits to raylib for color definitions
#define LIGHTGRAY  (Color){ 200, 200, 200, 255 }   // Light Gray
#define GRAY       (Color){ 130, 130, 130, 255 }   // Gray
#define DARKGRAY   (Color){ 80, 80, 80, 255 }      // Dark Gray
#define YELLOW     (Color){ 253, 249, 0, 255 }     // Yellow
#define GOLD       (Color){ 255, 203, 0, 255 }     // Gold
#define ORANGE     (Color){ 255, 161, 0, 255 }     // Orange
#define PINK       (Color){ 255, 109, 194, 255 }   // Pink
#define RED        (Color){ 230, 41, 55, 255 }     // Red
#define MAROON     (Color){ 190, 33, 55, 255 }     // Maroon
#define GREEN      (Color){ 0, 228, 48, 255 }      // Green
#define LIME       (Color){ 0, 158, 47, 255 }      // Lime
#define DARKGREEN  (Color){ 0, 117, 44, 255 }      // Dark Green
#define SKYBLUE    (Color){ 102, 191, 255, 255 }   // Sky Blue
#define BLUE       (Color){ 0, 121, 241, 255 }     // Blue
#define DARKBLUE   (Color){ 0, 82, 172, 255 }      // Dark Blue
#define PURPLE     (Color){ 200, 122, 255, 255 }   // Purple
#define VIOLET     (Color){ 135, 60, 190, 255 }    // Violet
#define DARKPURPLE (Color){ 112, 31, 126, 255 }    // Dark Purple
#define BEIGE      (Color){ 211, 176, 131, 255 }   // Beige
#define BROWN      (Color){ 127, 106, 79, 255 }    // Brown
#define DARKBROWN  (Color){ 76, 63, 47, 255 }      // Dark Brown

#define WHITE      (Color){ 255, 255, 255, 255 }   // White
#define BLACK      (Color){ 0, 0, 0, 255 }         // Black
#define BLANK      (Color){ 0, 0, 0, 0 }           // Blank (Transparent)
#define MAGENTA    (Color){ 255, 0, 255, 255 }     // Magenta

typedef std::string string;

struct float2 {
    float x;
    float y;
};

struct float3 {
    float x;
    float y;
    float z;
};

struct float4 {
    float x;
    float y;
    float z;
    float w;
};

struct Color {
    Uint8 r;
    Uint8 g;
    Uint8 b;
    Uint8 a = 255;
};

struct Rect {
    float2 pos;
    float2 size;
};

struct Line {
    float2 startPos;
    float2 endPos;
};

struct Triangle {
    float2 a;
    float2 b;
    float2 c;
};

struct Curve {
    float2 startPoint;
    float2 controlPoint1;
    float2 controlPoint2;
    float2 endPoint;
};

struct Texture {
    int w;
    int h;
    std::vector<Color> colorData;
};

template<typename... Args>
inline void print(Args&&... args) {
    (std::cout << ... << std::forward<Args>(args));
}

template<typename... Args>
inline void println(Args&&... args) {
    (std::cout << ... << std::forward<Args>(args)) << "\n";
}

inline int getFPS(float deltaTime) {
    static float smoothedDelta = 1.0f / 60.0f;

    if (deltaTime > 0.0f) {
        smoothedDelta = smoothedDelta * 0.9f + deltaTime * 0.1f;
    }

    if (smoothedDelta <= 0.0f) {
        return 0;
    }

    return static_cast<int>(1.0f / smoothedDelta);
}

inline int getFPS(float deltaTime, Uint32 averageWindowMs, bool* hasNewValue) {
    static float accumulatedTime = 0.0f;
    static int accumulatedFrames = 0;
    static int averagedFPS = 60;

    if (hasNewValue) {
        *hasNewValue = false;
    }

    if (deltaTime <= 0.0f) {
        return averagedFPS;
    }

    const float windowSeconds = static_cast<float>(std::max<Uint32>(averageWindowMs, 1)) / 1000.0f;
    accumulatedTime += deltaTime;
    accumulatedFrames++;

    if (accumulatedTime >= windowSeconds) {
        averagedFPS = static_cast<int>(static_cast<float>(accumulatedFrames) / accumulatedTime);
        accumulatedTime = 0.0f;
        accumulatedFrames = 0;
        if (hasNewValue) {
            *hasNewValue = true;
        }
    }

    return averagedFPS;
}

inline int getAverageFPS(float deltaTime, Uint32 averageWindowMs) {
    return getFPS(deltaTime, averageWindowMs, nullptr);
}

inline int toInt(float f) {
    return static_cast<int>(f);
}

inline float toFloat(int i) {
    return static_cast<float> (i);
}

template <typename T>
void shuffleVector(std::vector<T>& vec) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(vec.begin(), vec.end(), gen);
}

template <typename T>
void quickSort(std::vector<T>& vec, int left, int right) {
    if (left >= right) return;

    T pivot = vec[right];
    int partitionIndex = left;

    for (int i = left; i < right; i++) {
        if (vec[i] < pivot) {
            std::swap(vec[i], vec[partitionIndex]);
            partitionIndex++;
        }
    }
    std::swap(vec[partitionIndex], vec[right]);

    quickSort(vec, left, partitionIndex - 1);
    quickSort(vec, partitionIndex + 1, right);
}

template <typename T>
void quickSort(std::vector<T>& vec) {
    if (!vec.empty()) {
        quickSort(vec, 0, vec.size() - 1);
    }
}

#endif //MEOWENGINE_CORE_HPP
