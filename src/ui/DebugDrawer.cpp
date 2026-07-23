#include "ui/DebugDrawer.hpp"

#include "raylib-cpp.hpp"

DebugDrawer& DebugDrawer::get() {
    static DebugDrawer instance;
    return instance;
}

void DebugDrawer::beginFrame() {
	currentY = 10;
}

void DebugDrawer::draw(const std::string& text) {
	raylib::DrawText(text, 10, currentY, 10, GREEN);
	currentY += 15;
}
