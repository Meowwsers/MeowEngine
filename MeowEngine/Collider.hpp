//
// Created by Stěpán Toman on 18.04.2026.
//

#ifndef MEOWENGINE_COLLIDER_HPP
#define MEOWENGINE_COLLIDER_HPP

#include <vector>

#include "Core.hpp"

struct CollisionState {
    bool collided = false;
    bool grounded = false;
    bool hitCeiling = false;
    bool hitLeftWall = false;
    bool hitRightWall = false;
};

class Collider {
public:
    Rect rect;

    static bool intersects(const Rect& rect1, const Rect& rect2) {
        return !(rect1.pos.x + rect1.size.x <= rect2.pos.x ||
                 rect2.pos.x + rect2.size.x <= rect1.pos.x ||
                 rect1.pos.y + rect1.size.y <= rect2.pos.y ||
                 rect2.pos.y + rect2.size.y <= rect1.pos.y);
    }

    static CollisionState resolveAgainstWorld(
        Rect& movingRect,
        float2& velocity,
        float deltaTime,
        const std::vector<Collider>& worldColliders
    ) {
        CollisionState state;

        movingRect.pos.x += velocity.x * deltaTime;
        for (const Collider& collider : worldColliders) {
            if (!intersects(movingRect, collider.rect)) {
                continue;
            }

            state.collided = true;
            if (velocity.x > 0.0f) {
                movingRect.pos.x = collider.rect.pos.x - movingRect.size.x;
                state.hitRightWall = true;
            } else if (velocity.x < 0.0f) {
                movingRect.pos.x = collider.rect.pos.x + collider.rect.size.x;
                state.hitLeftWall = true;
            }
            velocity.x = 0.0f;
        }

        movingRect.pos.y += velocity.y * deltaTime;
        for (const Collider& collider : worldColliders) {
            if (!intersects(movingRect, collider.rect)) {
                continue;
            }

            state.collided = true;
            if (velocity.y > 0.0f) {
                movingRect.pos.y = collider.rect.pos.y - movingRect.size.y;
                state.grounded = true;
            } else if (velocity.y < 0.0f) {
                movingRect.pos.y = collider.rect.pos.y + collider.rect.size.y;
                state.hitCeiling = true;
            }
            velocity.y = 0.0f;
        }

        return state;
    }

    static bool resolveHorizontal(Rect& movingRect, const Rect& worldRect, float& velocityX) {
        if (!intersects(movingRect, worldRect)) {
            return false;
        }

        if (velocityX > 0.0f) {
            movingRect.pos.x = worldRect.pos.x - movingRect.size.x;
        } else if (velocityX < 0.0f) {
            movingRect.pos.x = worldRect.pos.x + worldRect.size.x;
        }

        velocityX = 0.0f;
        return true;
    }

    static bool resolveVertical(Rect& movingRect, const Rect& worldRect, float& velocityY, bool& grounded) {
        if (!intersects(movingRect, worldRect)) {
            return false;
        }

        if (velocityY > 0.0f) {
            movingRect.pos.y = worldRect.pos.y - movingRect.size.y;
            grounded = true;
        } else if (velocityY < 0.0f) {
            movingRect.pos.y = worldRect.pos.y + worldRect.size.y;
        }

        velocityY = 0.0f;
        return true;
    }

    static bool intersecting(const Rect& rect1, const Rect& rect2) {
        return intersects(rect1, rect2);
    }
};
#endif //MEOWENGINE_COLLIDER_HPP
