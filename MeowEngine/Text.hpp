//
// Created by Stěpán Toman on 18.04.2026.
//

#ifndef MEOWENGINE_TEXT_HPP
#define MEOWENGINE_TEXT_HPP

#include "Core.hpp"
#include "Application.hpp"

class Text {

public:
    string text;
    explicit Text(Application& argApplication, const std::string& argText) : application(argApplication) {
        text = argText;
    }

    ~Text() {
        destroyCachedText();
    }

    Text(const Text&) = delete;
    Text& operator=(const Text&) = delete;

    Text(Text&&) = delete;
    Text& operator=(Text&&) = delete;

    void setText(const string& argText) {
        if (text == argText) {
            return;
        }

        text = argText;
        cacheDirty = true;
    }

    void drawText(float2 pos, Color color, float scale = 1.0f) {

        if (!application.textEngine || !application.font || !application.renderer) {
            return;
        }

        if (scale <= 0.0f) {
            scale = 1.0f;
        }

        if (baseFontSize <= 0.0f) {
            baseFontSize = TTF_GetFontSize(application.font);
            if (baseFontSize <= 0.0f) {
                baseFontSize = 1.0f;
            }
        }

        float scaledFontSize = baseFontSize * scale;
        if (scaledFontSize < 1.0f) {
            scaledFontSize = 1.0f;
        }

        if (cacheDirty || !cachedText || scaledFontSize != cachedScale || !isSameColor(color, cachedColor)) {
            TTF_SetFontSize(application.font, scaledFontSize);

            destroyCachedText();
            cachedText = TTF_CreateText(application.textEngine, application.font, text.c_str(), text.length());
            if (!cachedText) {
                return;
            }

            TTF_SetTextColor(cachedText, color.r, color.g, color.b, color.a);

            cachedScale = scaledFontSize;
            cachedColor = color;
            cacheDirty = false;
        }

        TTF_DrawRendererText(cachedText, pos.x, pos.y);
    }

private:
    static bool isSameColor(const Color& a, const Color& b) {
        return a.r == b.r && a.g == b.g && a.b == b.b && a.a == b.a;
    }

    void destroyCachedText() {
        if (cachedText) {
            TTF_DestroyText(cachedText);
            cachedText = nullptr;
        }
    }

    Application& application;
    TTF_Text* cachedText = nullptr;
    float baseFontSize = 0.0f;
    float cachedScale = -1.0f;
    Color cachedColor = WHITE;
    bool cacheDirty = true;
};
#endif //MEOWENGINE_TEXT_HPP
