#include "systems/AnimationSystem.hpp"


raylib::Vector2 AnimationSystem::getVisualPosition(
    EntityId id,
    Cell actualPosition) const
{
    auto it = moveAnimations.find(id);

    if (it != moveAnimations.end()) {
        return it->second.getVisualPosition();
    }

    return {
        static_cast<float>(actualPosition.x),
        static_cast<float>(actualPosition.y)
    };
}

void AnimationSystem::onMove(const MoveEvent& event) {
    moveAnimations[event.id] = MoveAnimation{ event.from, event.to };
}

void AnimationSystem::onNewTurn()
{
    moveAnimations.clear();
}


void AnimationSystem::update(float deltaTime)
{
    for (auto it = moveAnimations.begin(); it != moveAnimations.end(); )
    {
        it->second.update(deltaTime);

        if (it->second.isFinished())
        {
            it = moveAnimations.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

