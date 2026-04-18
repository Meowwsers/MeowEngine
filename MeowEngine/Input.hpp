//
// Created by Stěpán Toman on 17.04.2026.
//

#ifndef MEOWENGINE_INPUT_HPP
#define MEOWENGINE_INPUT_HPP

#include "Core.hpp"
#include <array>

enum KeyboardKey {
    KEY_NULL            = 0,
    // Alphanumeric keys
    KEY_APOSTROPHE      = SDL_SCANCODE_APOSTROPHE,
    KEY_COMMA           = SDL_SCANCODE_COMMA,
    KEY_MINUS           = SDL_SCANCODE_MINUS,
    KEY_PERIOD          = SDL_SCANCODE_PERIOD,
    KEY_SLASH           = SDL_SCANCODE_SLASH,
    KEY_ZERO            = SDL_SCANCODE_0,
    KEY_ONE             = SDL_SCANCODE_1,
    KEY_TWO             = SDL_SCANCODE_2,
    KEY_THREE           = SDL_SCANCODE_3,
    KEY_FOUR            = SDL_SCANCODE_4,
    KEY_FIVE            = SDL_SCANCODE_5,
    KEY_SIX             = SDL_SCANCODE_6,
    KEY_SEVEN           = SDL_SCANCODE_7,
    KEY_EIGHT           = SDL_SCANCODE_8,
    KEY_NINE            = SDL_SCANCODE_9,
    KEY_SEMICOLON       = SDL_SCANCODE_SEMICOLON,
    KEY_EQUAL           = SDL_SCANCODE_EQUALS,
    KEY_A               = SDL_SCANCODE_A,
    KEY_B               = SDL_SCANCODE_B,
    KEY_C               = SDL_SCANCODE_C,
    KEY_D               = SDL_SCANCODE_D,
    KEY_E               = SDL_SCANCODE_E,
    KEY_F               = SDL_SCANCODE_F,
    KEY_G               = SDL_SCANCODE_G,
    KEY_H               = SDL_SCANCODE_H,
    KEY_I               = SDL_SCANCODE_I,
    KEY_J               = SDL_SCANCODE_J,
    KEY_K               = SDL_SCANCODE_K,
    KEY_L               = SDL_SCANCODE_L,
    KEY_M               = SDL_SCANCODE_M,
    KEY_N               = SDL_SCANCODE_N,
    KEY_O               = SDL_SCANCODE_O,
    KEY_P               = SDL_SCANCODE_P,
    KEY_Q               = SDL_SCANCODE_Q,
    KEY_R               = SDL_SCANCODE_R,
    KEY_S               = SDL_SCANCODE_S,
    KEY_T               = SDL_SCANCODE_T,
    KEY_U               = SDL_SCANCODE_U,
    KEY_V               = SDL_SCANCODE_V,
    KEY_W               = SDL_SCANCODE_W,
    KEY_X               = SDL_SCANCODE_X,
    KEY_Y               = SDL_SCANCODE_Y,
    KEY_Z               = SDL_SCANCODE_Z,
    KEY_LEFT_BRACKET    = SDL_SCANCODE_LEFTBRACKET,
    KEY_BACKSLASH       = SDL_SCANCODE_BACKSLASH,
    KEY_RIGHT_BRACKET   = SDL_SCANCODE_RIGHTBRACKET,
    KEY_GRAVE           = SDL_SCANCODE_GRAVE,
    // Control keys
    KEY_SPACE           = SDL_SCANCODE_SPACE,
    KEY_ESCAPE          = SDL_SCANCODE_ESCAPE,
    KEY_ENTER           = SDL_SCANCODE_RETURN,
    KEY_TAB             = SDL_SCANCODE_TAB,
    KEY_BACKSPACE       = SDL_SCANCODE_BACKSPACE,
    KEY_INSERT          = SDL_SCANCODE_INSERT,
    KEY_DELETE          = SDL_SCANCODE_DELETE,
    KEY_RIGHT           = SDL_SCANCODE_RIGHT,
    KEY_LEFT            = SDL_SCANCODE_LEFT,
    KEY_DOWN            = SDL_SCANCODE_DOWN,
    KEY_UP              = SDL_SCANCODE_UP,
    KEY_PAGE_UP         = SDL_SCANCODE_PAGEUP,
    KEY_PAGE_DOWN       = SDL_SCANCODE_PAGEDOWN,
    KEY_HOME            = SDL_SCANCODE_HOME,
    KEY_END             = SDL_SCANCODE_END,
    KEY_CAPS_LOCK       = SDL_SCANCODE_CAPSLOCK,
    KEY_SCROLL_LOCK     = SDL_SCANCODE_SCROLLLOCK,
    KEY_NUM_LOCK        = SDL_SCANCODE_NUMLOCKCLEAR,
    KEY_PRINT_SCREEN    = SDL_SCANCODE_PRINTSCREEN,
    KEY_PAUSE           = SDL_SCANCODE_PAUSE,
    KEY_F1              = SDL_SCANCODE_F1,
    KEY_F2              = SDL_SCANCODE_F2,
    KEY_F3              = SDL_SCANCODE_F3,
    KEY_F4              = SDL_SCANCODE_F4,
    KEY_F5              = SDL_SCANCODE_F5,
    KEY_F6              = SDL_SCANCODE_F6,
    KEY_F7              = SDL_SCANCODE_F7,
    KEY_F8              = SDL_SCANCODE_F8,
    KEY_F9              = SDL_SCANCODE_F9,
    KEY_F10             = SDL_SCANCODE_F10,
    KEY_F11             = SDL_SCANCODE_F11,
    KEY_F12             = SDL_SCANCODE_F12,
    KEY_LEFT_SHIFT      = SDL_SCANCODE_LSHIFT,
    KEY_LEFT_CONTROL    = SDL_SCANCODE_LCTRL,
    KEY_LEFT_ALT        = SDL_SCANCODE_LALT,
    KEY_RIGHT_SHIFT     = SDL_SCANCODE_RSHIFT,
    KEY_RIGHT_CONTROL   = SDL_SCANCODE_RCTRL,
    KEY_RIGHT_ALT       = SDL_SCANCODE_RALT
};

class Input {
public:
    static void NewFrame() {
        prevKeyState = currKeyState;
        const bool* keyboardState = SDL_GetKeyboardState(nullptr);
        for (int i = 0; i < SDL_SCANCODE_COUNT; ++i) {
            currKeyState[i] = keyboardState && keyboardState[i];
        }

        prevMouseState = currMouseState;
        currMouseState = SDL_GetMouseState(&mouseX, &mouseY);
    }

    static bool ProcessEvent(class Application&, const SDL_Event& event) {
        (void)event;
        return false;
    }

    // Keyboard API using the Raylib-style enum
    static bool IsKeyDown(KeyboardKey key) { return currKeyState[static_cast<SDL_Scancode>(key)]; }
    static bool IsKeyPressed(KeyboardKey key) {
        return currKeyState[static_cast<SDL_Scancode>(key)] && !prevKeyState[static_cast<SDL_Scancode>(key)];
    }
    static bool IsKeyReleased(KeyboardKey key) {
        return !currKeyState[static_cast<SDL_Scancode>(key)] && prevKeyState[static_cast<SDL_Scancode>(key)];
    }

    // Mouse API
    static float GetMouseX() { return mouseX; }
    static float GetMouseY() { return mouseY; }
    static bool IsMouseButtonDown(int button) { return currMouseState & SDL_BUTTON_MASK(button); }
    static bool IsMouseButtonPressed(int button) {
        return (currMouseState & SDL_BUTTON_MASK(button)) && !(prevMouseState & SDL_BUTTON_MASK(button));
    }

private:
    inline static std::array<bool, SDL_SCANCODE_COUNT> currKeyState{};
    inline static std::array<bool, SDL_SCANCODE_COUNT> prevKeyState{};
    inline static Uint32 currMouseState = 0;
    inline static Uint32 prevMouseState = 0;
    inline static float mouseX = 0;
    inline static float mouseY = 0;
};

#endif //MEOWENGINE_INPUT_HPP