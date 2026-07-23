#include "gameplay/GameplayRenderer.hpp"

#include "core/Constants.hpp"


namespace GameplayRenderer {

	void updateCamera(raylib::Camera2D& camera, const Level& level) {

		camera.offset = {
		raylib::Window::GetWidth() / 2.0f,
		raylib::Window::GetHeight() / 2.0f
		};

		camera.target = {
			level.grid.getWidth() * Constants::CellSize / 2.0f,
			level.grid.getHeight() * Constants::CellSize / 2.0f
		};

		camera.zoom = std::min(
			static_cast<float>(raylib::Window::GetWidth() - 100) / (level.grid.getWidth() * Constants::CellSize),
			static_cast<float>(raylib::Window::GetHeight() - 100) / (level.grid.getHeight() * Constants::CellSize)
		);
	}


	void drawPlayer(const Player& player, const AnimationSystem& animationSystem)
	{
		raylib::Rectangle rectangle{
			animationSystem.getVisualPosition(
				player.id,
				player.position
			) * Constants::CellSize,
			{ static_cast<float>(Constants::CellSize), static_cast<float>(Constants::CellSize) } };

		rectangle.Draw(raylib::Color::Red());
	}

	void drawCrate(const Crate& crate, const AnimationSystem& animationSystem) {
		raylib::Rectangle rectangle{
			animationSystem.getVisualPosition(
				crate.id,
				crate.position
			) * Constants::CellSize,
			{ static_cast<float>(Constants::CellSize), static_cast<float>(Constants::CellSize) } };

		rectangle.Draw(raylib::Color::Brown());
	}

	void draw(raylib::Camera2D camera, const Level& level, const GameState& state, const AnimationSystem& animationSystem)
	{
		updateCamera(camera, level);
		camera.BeginMode();
		level.grid.draw();

		drawPlayer(state.player, animationSystem);
		for (const auto& crate : state.crates) {
			drawCrate(crate, animationSystem);
		}
		camera.EndMode();
	}

}