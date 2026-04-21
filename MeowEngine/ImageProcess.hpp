//
// Created by Stěpán Toman on 18.04.2026.
//

#ifndef MEOWENGINE_IMAGEPROCESS_HPP
#define MEOWENGINE_IMAGEPROCESS_HPP
#include "Core.hpp"
#include "Application.hpp"
#include <array>
#include <cmath>
#include <cstring>

class ImageProcess {

public:
    explicit ImageProcess(Application& application1, const std::string& filePath) : application(application1) {
        loadImg(filePath);
    }

    ~ImageProcess() {
        unloadImg();
    }

    Application& application;
    Texture image;

    SDL_Texture* getGPUTexture() const {
        return gpuTexture;
    }

    int getDisplayWidth() const {
        return displayW;
    }

    int getDisplayHeight() const {
        return displayH;
    }

    void invertColors() {
        for (Color& color : image.colorData) {
            color.r = static_cast<Uint8>(255 - color.r);
            color.g = static_cast<Uint8>(255 - color.g);
            color.b = static_cast<Uint8>(255 - color.b);
        }
        uploadToGPU();
    }

    void reverseBytes() {
        std::reverse(image.colorData.begin(), image.colorData.end());
        uploadToGPU();
    }

    void randomizeBytes() {
        for (size_t i = image.colorData.size(); i > 1; --i) {
            const size_t j = static_cast<size_t>(nextRandom()) % i;
            std::swap(image.colorData[i - 1], image.colorData[j]);
        }
        uploadToGPU();
    }

    void colorSort() {
        if (image.colorData.empty()) {
            return;
        }

        // Linear-time luminance bucket sort (0..255), much faster on very large images.
        std::array<size_t, 256> counts{};
        for (const Color& color : image.colorData) {
            const int luma = (299 * color.r + 587 * color.g + 114 * color.b) / 1000;
            counts[static_cast<size_t>(luma)]++;
        }

        std::array<size_t, 256> offsets{};
        size_t running = 0;
        for (size_t i = 0; i < counts.size(); i++) {
            offsets[i] = running;
            running += counts[i];
        }

        sortScratch.resize(image.colorData.size());
        for (const Color& color : image.colorData) {
            const int luma = (299 * color.r + 587 * color.g + 114 * color.b) / 1000;
            const size_t idx = static_cast<size_t>(luma);
            sortScratch[offsets[idx]++] = color;
        }

        image.colorData.swap(sortScratch);
        uploadToGPU();
    }


private:
    void unloadImg() {
        if (gpuTexture) {
            SDL_DestroyTexture(gpuTexture);
            gpuTexture = nullptr;
        }

        image.w = 0;
        image.h = 0;
        displayW = 0;
        displayH = 0;
        std::vector<Color>().swap(image.colorData);
    }

    void loadImg(const std::string& filePath) {
        unloadImg();

        if (!application.renderer) {
            return;
        }

        SDL_Surface* raw = IMG_Load(filePath.c_str());
        if (!raw) {
            return;
        }

        // Convert to RGBA32 in one SDL call so we can do a direct memory copy
        // instead of calling SDL_ReadSurfacePixel per pixel.
        SDL_Surface* surface = SDL_ConvertSurface(raw, SDL_PIXELFORMAT_RGBA32);
        SDL_DestroySurface(raw);
        if (!surface) {
            return;
        }

        displayW = surface->w;
        displayH = surface->h;

        // Process roughly around viewport density; this keeps large-source effects real-time.
        const int windowPixels = std::max(1, application.screenWidth * application.screenHeight);
        const int maxProcessingPixels = std::max(300000, windowPixels * 2);
        const int sourcePixels = displayW * displayH;

        image.w = displayW;
        image.h = displayH;

        if (sourcePixels > maxProcessingPixels) {
            const float downscale = std::sqrt(static_cast<float>(maxProcessingPixels) / static_cast<float>(sourcePixels));
            image.w = std::max(1, static_cast<int>(displayW * downscale));
            image.h = std::max(1, static_cast<int>(displayH * downscale));
        }

        const int pixelCount = image.w * image.h;
        image.colorData.resize(pixelCount);

        const int srcPitch = surface->pitch;
        const Uint8* src = static_cast<const Uint8*>(surface->pixels);

        if (image.w == displayW && image.h == displayH) {
            const int rowBytes = image.w * static_cast<int>(sizeof(Color));
            Uint8* dst = reinterpret_cast<Uint8*>(image.colorData.data());
            for (int y = 0; y < image.h; y++) {
                std::memcpy(dst + y * rowBytes, src + y * srcPitch, rowBytes);
            }
        } else {
            // Nearest-neighbor downsample for faster per-frame image effects.
            for (int y = 0; y < image.h; y++) {
                const int srcY = (y * displayH) / image.h;
                const Uint8* srcRow = src + srcY * srcPitch;
                for (int x = 0; x < image.w; x++) {
                    const int srcX = (x * displayW) / image.w;
                    const Uint8* px = srcRow + srcX * 4;
                    image.colorData[y * image.w + x] = {px[0], px[1], px[2], px[3]};
                }
            }
        }

        SDL_DestroySurface(surface);
        uploadToGPU();
    }

    void uploadToGPU() {
        if (image.w <= 0 || image.h <= 0 || image.colorData.empty()) {
            return;
        }

        // STATIC + SDL_UpdateTexture is usually lower overhead for full-frame uploads.
        if (!gpuTexture) {
            gpuTexture = application.createTexture(
                image.w,
                image.h,
                SDL_PIXELFORMAT_RGBA32,
                SDL_TEXTUREACCESS_STATIC
            );
        }

        if (!gpuTexture) {
            return;
        }

        const int pitch = image.w * static_cast<int>(sizeof(Color));
        if (!SDL_UpdateTexture(gpuTexture, nullptr, image.colorData.data(), pitch)) {
            SDL_DestroyTexture(gpuTexture);
            gpuTexture = nullptr;
        }
    }

    Uint32 nextRandom() {
        rngState ^= (rngState << 13);
        rngState ^= (rngState >> 17);
        rngState ^= (rngState << 5);
        return rngState;
    }

    SDL_Texture* gpuTexture = nullptr;
    int displayW = 0;
    int displayH = 0;
    std::vector<Color> sortScratch;
    Uint32 rngState = 0xA341316Cu;

};

#endif //MEOWENGINE_IMAGEPROCESS_HPP
