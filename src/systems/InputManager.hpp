#pragma once

#include <array>
#include <queue>
#include <string_view>
#include <vector>

#include "raylib-cpp.hpp"

#include "core/Action.hpp"
#if defined(__EMSCRIPTEN__) || defined(FORCE_ENABLE_MOBILE_GAMEPAD)
#include "MobileGamepad.h"
#endif


class InputManager {
public:
	void processInputs(float deltaTime);
	Action getAction() const;

#if defined(__EMSCRIPTEN__) || defined(FORCE_ENABLE_MOBILE_GAMEPAD)
	void initMobileGamepad();
	void drawMobileGamepad() const;
#endif
private:

	static constexpr std::array keyToActionMap{
		std::pair{KEY_ESCAPE, Action::Pause},
		std::pair{KEY_Z, Action::Undo},
		std::pair{KEY_X, Action::Interact},
		std::pair{KEY_R, Action::Restart},
		std::pair{KEY_E, Action::Editor},
		std::pair{KEY_UP, Action::MoveUp},
		std::pair{KEY_DOWN, Action::MoveDown},
		std::pair{KEY_LEFT, Action::MoveLeft},
		std::pair{KEY_RIGHT, Action::MoveRight}
	};

	static constexpr std::array mouseButtonToActionMap{
		std::pair{MOUSE_LEFT_BUTTON, Action::LMB},
		std::pair{MOUSE_RIGHT_BUTTON, Action::RMB}
	};

	static constexpr std::array gamepadButtonToActionMap{
		std::pair{GAMEPAD_BUTTON_MIDDLE_RIGHT, Action::Pause},
		std::pair{GAMEPAD_BUTTON_MIDDLE_LEFT, Action::Pause},
		std::pair{GAMEPAD_BUTTON_RIGHT_FACE_RIGHT, Action::Undo},
		std::pair{GAMEPAD_BUTTON_RIGHT_FACE_DOWN, Action::Interact},
		std::pair{GAMEPAD_BUTTON_RIGHT_FACE_UP, Action::Restart},
		std::pair{GAMEPAD_BUTTON_LEFT_FACE_UP, Action::MoveUp},
		std::pair{GAMEPAD_BUTTON_LEFT_FACE_DOWN, Action::MoveDown},
		std::pair{GAMEPAD_BUTTON_LEFT_FACE_LEFT, Action::MoveLeft},
		std::pair{GAMEPAD_BUTTON_LEFT_FACE_RIGHT, Action::MoveRight},
	};

	static constexpr float INITIAL_KEYDOWN_DELAY{ 0.30f };
	static constexpr float REPEAT_KEYDOWN_DELAY{ 0.10f };

	static Action getPressedAction();
	static Action getHeldAction();
	static bool isActionStillHeld(Action action);

	Action activeAction{ Action::None };
	Action lastHeldAction{ Action::None };
	bool isActionRepeating{ false };
	float keyDownTime{ 0.0f };

	inline static raylib::Gamepad gamepad{0};
#if defined(__EMSCRIPTEN__) || defined(FORCE_ENABLE_MOBILE_GAMEPAD)

	static constexpr std::array availableMobileGamepadActions{
		Action::Pause,
		Action::Undo,
		Action::Interact,
		Action::Restart,
		Action::MoveUp,
		Action::MoveDown,
		Action::MoveLeft,
		Action::MoveRight,
	};

	inline static MobileGamepad mobileGamepad{};
#endif
};