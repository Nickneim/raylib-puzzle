#pragma once

#include "raylib-cpp.hpp"

#include "gameplay/Events.hpp"

class SoundSystem {
public:
	void onMove(const MoveEvent& event);
	void onWin();

private:
	raylib::Sound crateMoveSound{ "resources/sounds/CrateMove.wav" };
	raylib::Sound levelWinSound{ "resources/sounds/LevelWin.wav" };
};