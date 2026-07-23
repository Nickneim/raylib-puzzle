#pragma once

#include <string>

class DebugDrawer {
public:
    static DebugDrawer& get();

    void beginFrame();
    void draw(const std::string& text);

	DebugDrawer(const DebugDrawer&) = delete;
	DebugDrawer& operator=(const DebugDrawer&) = delete;

private:
    DebugDrawer() = default;

    int currentY = 10;
};