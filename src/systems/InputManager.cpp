#include "systems/InputManager.hpp"

Action InputManager::getPressedAction() {
	for (const auto& [key, action] : keyToActionMap) {
		if (raylib::Keyboard::IsKeyPressed(key)) {
			return action;
		}
	}

#if defined(__EMSCRIPTEN__) || defined(FORCE_ENABLE_MOBILE_GAMEPAD)
	for (const auto& action : availableMobileGamepadActions) {
		if (mobileGamepad.isPressed(action)) {
			return action;
		}
	}
#endif
	for (const auto& [button, action] : mouseButtonToActionMap) {
		if (raylib::Mouse::IsButtonPressed(button)) {
			return action;
		}
	}

	for (const auto& [button, action] : gamepadButtonToActionMap) {
		if (gamepad.IsButtonPressed(button)) {
			return action;
		}
	}
	return Action::None;
}

Action InputManager::getHeldAction() {
	for (const auto& [key, action] : keyToActionMap) {
		if (raylib::Keyboard::IsKeyDown(key)) {
			return action;
		}
	}
#if defined(__EMSCRIPTEN__) || defined(FORCE_ENABLE_MOBILE_GAMEPAD)
	for (const auto& action : availableMobileGamepadActions) {
		if (mobileGamepad.isHeld(action)) {
			return action;
		}
	}
#endif

	for (const auto& [button, action] : mouseButtonToActionMap) {
		if (raylib::Mouse::IsButtonDown(button)) {
			return action;
		}
	}

	for (const auto& [button, action] : gamepadButtonToActionMap) {
		if (gamepad.IsButtonDown(button)) {
			return action;
		}
	}
	return Action::None;
}

bool InputManager::isActionStillHeld(Action action) {
	for (const auto& [key, mappedAction] : keyToActionMap) {
		if (mappedAction == action && raylib::Keyboard::IsKeyDown(key)) {
			return true;
		}
	}
	for (const auto& [button, mappedAction] : mouseButtonToActionMap) {
		if (mappedAction == action && raylib::Mouse::IsButtonDown(button)) {
			return true;
		}
	}
	for (const auto& [button, mappedAction] : gamepadButtonToActionMap) {
		if (mappedAction == action && gamepad.IsButtonDown(button)) {
			return true;
		}
	}

	return false;
}

void InputManager::processInputs(float deltaTime) {
#if defined(__EMSCRIPTEN__) || defined(FORCE_ENABLE_MOBILE_GAMEPAD)
	mobileGamepad.update();
#endif
	auto pressedAction = getPressedAction();
	if (pressedAction != Action::None) {
		isActionRepeating = false;
		keyDownTime = 0;
		lastHeldAction = pressedAction;
		activeAction = pressedAction;
		return;
	}

	auto heldAction = isActionStillHeld(lastHeldAction) ? lastHeldAction : getHeldAction();

	if (heldAction != lastHeldAction) {
		isActionRepeating = false;
		keyDownTime = 0;
		// new last held action!
		lastHeldAction = heldAction;
		activeAction = heldAction;
		return;
	}

	activeAction = Action::None;

	if (heldAction == Action::None) {
		return;
	}

	keyDownTime += deltaTime;
	if (!isActionRepeating && keyDownTime >= INITIAL_KEYDOWN_DELAY) {
		activeAction = heldAction;
		keyDownTime -= INITIAL_KEYDOWN_DELAY;
		isActionRepeating = true;
	}
	else if (isActionRepeating && keyDownTime >= REPEAT_KEYDOWN_DELAY) {
		activeAction = heldAction;
		keyDownTime -= REPEAT_KEYDOWN_DELAY;
	}
}

Action InputManager::getAction() const {
	return activeAction;
}

#if defined(__EMSCRIPTEN__) || defined(FORCE_ENABLE_MOBILE_GAMEPAD)
void InputManager::initMobileGamepad()
{
	//mobileGamepad.buildDefaultLayout();
	mobileGamepad.LoadSprites("resources/sprites/dpad_element_east.png", "resources/sprites/icon_button_a.png",
                        "resources/sprites/icon_button_b.png", "resources/sprites/icon_button_r.png",
                        "resources/sprites/icon_menu.png");
	mobileGamepad.Layout(GetScreenWidth(), GetScreenHeight()); // call on resize too
}

void InputManager::drawMobileGamepad() const
{
	mobileGamepad.draw();
}
#endif