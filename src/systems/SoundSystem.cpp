#include "systems/SoundSystem.hpp"

#include "core/EntityId.hpp"

void SoundSystem::onMove(const MoveEvent& event) {
	switch (event.type) {
	case EntityType::Crate:
		crateMoveSound.Play();
	default:
		break;
	}
}

void SoundSystem::onWin() {
	levelWinSound.Play();
}
