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

}

#endif //MEOWENGINE_MATH_HPP
