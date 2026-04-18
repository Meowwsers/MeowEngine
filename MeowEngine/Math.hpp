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

}

#endif //MEOWENGINE_MATH_HPP
