//
// Created by Stěpán Toman on 18.04.2026.
//

#ifndef MEOWENGINE_PLAYER_HPP
#define MEOWENGINE_PLAYER_HPP

#include <vector>

#include "Core.hpp"
#include "Collider.hpp"
#include "Input.hpp"
#include "Math.hpp"

class Player {

public:
    Rect rect = {{100.0f, 100.0f}, {50.0f, 50.0f}};
    Color color = WHITE;
    float2 velocity = {0.0f, 0.0f};
    bool grounded = false;

    Player(Application argApplication) {
        application = argApplication;
    }

    void setMovement(bool option) {
        enableMovement = option;
    }

    void setCollision(bool option) {
        enableCollision = option;
    }

    void setColiision(bool option) {
        enableCollision = option;
    }

    void setGravity(bool option) {
        enableGravity = option;
    }

    void moveSpeed(float movementSpeed) {
        movingSpeed = movementSpeed;
    }

    void gravity(float multiplier) {
        gravityMultiplier = multiplier;
    }

    void setJumpStrength(float strength) {
        jumpStrength = strength;
    }

    void update(float deltaTime, const std::vector<Collider>& worldColliders) {
        if (enableMovement && Input::IsKeyPressed(KEY_SPACE)) {
            jump(jumpStrength);
        }

        grounded = false;
        float moveDirection = 0.0f;

        if (enableMovement) {
            if (Input::IsKeyDown(KEY_D)) {
                moveDirection += 1.0f;
            }
            if (Input::IsKeyDown(KEY_A)) {
                moveDirection -= 1.0f;
            }
            velocity.x = moveDirection * movingSpeed;
        } else {
            velocity.x = 0.0f;
        }

        if (enableGravity) {
            velocity.y += gravityMultiplier * deltaTime;
        } else {
            velocity.y = 0.0f;
        }

        if (enableCollision) {
            CollisionState collisionState = Collider::resolveAgainstWorld(rect, velocity, deltaTime, worldColliders);
            grounded = collisionState.grounded;
        } else {
            rect.pos.x += velocity.x * deltaTime;
            rect.pos.y += velocity.y * deltaTime;
        }

        if (!enableOffscreen) {
            rect.pos.x = Math::clamp(rect.pos.x, 0.0f, application.screenWidth - rect.size.x);
            rect.pos.y = Math::clamp(rect.pos.y, 0.0f, application.screenHeight - rect.size.y);
        }
    }

    void jump(float jumpVelocity) {
        if (!grounded) {
            return;
        }

        velocity.y = -jumpVelocity;
        grounded = false;
    }

private:
    Application application;
    bool enableCollision = true;
    bool enableGravity = true;
    bool enableMovement = true;
    bool enableOffscreen = false;
    float movingSpeed = 300.0f;
    float gravityMultiplier = 900.0f;
    float jumpStrength = 450.0f;
};



#endif //MEOWENGINE_PLAYER_HPP
