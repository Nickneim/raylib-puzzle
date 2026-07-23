#pragma once

#include "raylib-cpp.hpp"

#include "core/GameState.hpp"
#include "world/Level.hpp"
#include "world/Player.hpp"
#include "systems/AnimationSystem.hpp"


namespace GameplayRenderer {

	void updateCamera(raylib::Camera2D& camera, const Level& level);

	void drawPlayer(const Player& player, const AnimationSystem& animationSystem);

	void drawCrate(const Crate& crate, const AnimationSystem& animationSystem);

	void draw(raylib::Camera2D camera, const Level& level, const GameState& state, const AnimationSystem& animationSystem);

};