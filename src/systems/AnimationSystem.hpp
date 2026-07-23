#pragma once

#include <unordered_map>

#include "raylib-cpp.hpp"

#include "core/EntityId.hpp"
#include "gameplay/Events.hpp"
#include "world/Cell.hpp"

struct MoveAnimation {
    Cell from;
    Cell to;

    float timer = 0.0f;
    float duration = 0.1f;

    void update(float deltaTime)
    {
        timer += deltaTime;
    }

    bool isFinished() const
    {
        return timer >= duration;
    }

    raylib::Vector2 getVisualPosition() const
    {
        float t = std::min(timer / duration, 1.0f);

        return {
            std::lerp(static_cast<float>(from.x),
                      static_cast<float>(to.x),
                      t),
            std::lerp(static_cast<float>(from.y),
                      static_cast<float>(to.y),
                      t)
        };
    }
};


class AnimationSystem {
public:
    raylib::Vector2 getVisualPosition(EntityId id, Cell actualPosition) const;

    void onMove(const MoveEvent& event);
    void onNewTurn();

    void update(float deltaTime);

private:
	std::unordered_map<EntityId, MoveAnimation> moveAnimations;
};