#pragma once

#include <variant>

#include "gameplay/Events.hpp"
#include "systems/AnimationSystem.hpp"
#include "systems/SoundSystem.hpp"

struct EventDispatcher {
    AnimationSystem& animationSystem;
    SoundSystem& soundSystem;

    void dispatch(const Event& event)
    {
        std::visit([&](auto&& e)
            {
                handle(e);
            }, event);
    }

private:
    void handle(const MoveEvent& e)
    {
        soundSystem.onMove(e);
        animationSystem.onMove(e);
    }

    void handle(const CrateDestroyedEvent e) {

    }

    void handle(const LevelWinEvent e) {
        soundSystem.onWin();
    }

    void handle(const NewTurnEvent e) {
        animationSystem.onNewTurn();
    }
};