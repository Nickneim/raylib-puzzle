#pragma once
#include "raylib-cpp.hpp"
#include <cmath>
#include <array>

#include "core/Action.hpp"


namespace mobile_gamepad_detail {

    inline std::array<raylib::Vector2, 5> DpadLocalPolygon() {
        return {
            raylib::Vector2{ -1.0f,   0.0f },
            raylib::Vector2{ -0.35f, -1.0f },
            raylib::Vector2{  1.0f,  -1.0f },
            raylib::Vector2{  1.0f,   1.0f },
            raylib::Vector2{ -0.35f,  1.0f },
        };
    }

    // Standard even-odd ray casting point-in-polygon test.
    inline bool PointInPolygon(raylib::Vector2 pt, const std::array<raylib::Vector2, 5>& poly) {
        bool inside = false;
        size_t n = poly.size();
        for (size_t i = 0, j = n - 1; i < n; j = i++) {
            const auto& a = poly[i];
            const auto& b = poly[j];
            bool crosses =
                ((a.y > pt.y) != (b.y > pt.y)) &&
                (pt.x < (b.x - a.x) * (pt.y - a.y) / (b.y - a.y) + a.x);
            if (crosses) inside = !inside;
        }
        return inside;
    }

} // namespace mobile_gamepad_detail


struct DpadButton {
    Action          action{};
    raylib::Vector2 center{};
    float           radius = 0.f;
    float           rotationDeg = 0.f;
    raylib::Texture2D* sprite = nullptr; // not owned; lifetime managed by MobileGamepad

    std::array<raylib::Vector2, 5> WorldPolygon() const {
        using namespace mobile_gamepad_detail;
        auto local = DpadLocalPolygon();
        float rad = rotationDeg * (PI / 180.0f);
        float cosA = std::cos(rad);
        float sinA = std::sin(rad);

        std::array<raylib::Vector2, 5> world;
        for (size_t i = 0; i < local.size(); ++i) {
            float sx = local[i].x * radius;
            float sy = local[i].y * radius;
            float rx = sx * cosA - sy * sinA;
            float ry = sx * sinA + sy * cosA;
            world[i] = raylib::Vector2{ center.x + rx, center.y + ry };
        }
        return world;
    }

    bool Contains(raylib::Vector2 point) const {
        return mobile_gamepad_detail::PointInPolygon(point, WorldPolygon());
    }

    void Draw(bool pressed) const {
        if (!sprite) return;
        raylib::Color tint = pressed ? WHITE : raylib::Color{ 200, 200, 200, 255 };

        float scale = (radius * 2.f) / static_cast<float>(sprite->width);
        raylib::Rectangle src{ 0, 0, (float)sprite->width, (float)sprite->height };
        raylib::Rectangle dst{
            center.x, center.y,
            sprite->width * scale,
            sprite->height * scale
        };
        raylib::Vector2 origin{
            static_cast<float>(sprite->width) / 2.f * scale,
            static_cast<float>(sprite->height) / 2.f * scale
        };

        sprite->Draw(src, dst, origin, rotationDeg, tint);
    }
};

// One round action button (Interact / Undo / Restart / Pause).
struct ActionButton {
    Action          action{};
    raylib::Vector2 center{};
    float           radius = 0.f;
    raylib::Texture2D* sprite = nullptr; // not owned

    bool Contains(raylib::Vector2 point) const {
        return Vector2Distance(point, center) <= radius;
    }

    void Draw(bool pressed) const {
        if (!sprite) return;
        raylib::Color tint = pressed ? WHITE : raylib::Color{ 200, 200, 200, 255 };

        float scale = (radius * 2.f) / static_cast<float>(sprite->width);
        raylib::Rectangle src{ 0, 0, (float)sprite->width, (float)sprite->height };
        raylib::Rectangle dst{
            center.x, center.y,
            sprite->width * scale,
            sprite->height * scale
        };
        raylib::Vector2 origin{
            static_cast<float>(sprite->width) / 2.f * scale,
            static_cast<float>(sprite->height) / 2.f * scale
        };
        sprite->Draw(src, dst, origin, 0.f, tint);
    }
};


class MobileGamepad {
public:
    // Load all sprites once at startup (or whenever assets become available).
    void LoadSprites(const std::string& dpadPath,
        const std::string& interactPath,
        const std::string& undoPath,
        const std::string& restartPath,
        const std::string& pausePath) {
        dpadTexture_ = raylib::Texture2D(dpadPath);
        dpadTexture_.SetFilter(TEXTURE_FILTER_BILINEAR)
            .SetWrap(TEXTURE_WRAP_CLAMP);
        interactTexture_ = raylib::Texture2D(interactPath);
        undoTexture_ = raylib::Texture2D(undoPath);
        restartTexture_ = raylib::Texture2D(restartPath);
        pauseTexture_ = raylib::Texture2D(pausePath);

        left_.sprite = &dpadTexture_;
        right_.sprite = &dpadTexture_;
        up_.sprite = &dpadTexture_;
        down_.sprite = &dpadTexture_;
        interact_.sprite = &interactTexture_;
        undo_.sprite = &undoTexture_;
        restart_.sprite = &restartTexture_;
        pause_.sprite = &pauseTexture_;
    }

    // Recomputes every button's position and size from current screen
    // dimensions. Call once at startup (after the window exists) and again
    // any time the screen size changes (e.g. on IsWindowResized(), or after
    // a fullscreen toggle). All positions are percentage-of-screen based,
    // so this is cheap and safe to call every frame instead if you'd rather
    // not track resize events explicitly.
    void Layout(int screenWidth, int screenHeight) {
        const float sw = static_cast<float>(screenWidth);
        const float sh = static_cast<float>(screenHeight);

        // ---- D-pad (left side) -------------------------------------------
        const float dpadCX = sw * 0.18f;
        const float dpadCY = sh * 0.75f;
//        const float dpadR = sh * 0.055f;
        const float dpadR = sh * 0.1f;

        const float dpadGap = dpadR * std::sqrt(2.0f) * .8f;//.15f;

        left_.action = Action::MoveLeft;
        left_.center = raylib::Vector2{ dpadCX - dpadGap, dpadCY };
        left_.radius = dpadR;
        left_.rotationDeg = 180.f;  // flipped to point left

        right_.action = Action::MoveRight;
        right_.center = raylib::Vector2{ dpadCX + dpadGap, dpadCY };
        right_.radius = dpadR;
        right_.rotationDeg = 0.f;     // sprite drawn as-is: points left

        up_.action = Action::MoveUp;
        up_.center = raylib::Vector2{ dpadCX, dpadCY - dpadGap };
        up_.radius = dpadR;
        up_.rotationDeg = -90.f;     // rotated to point up

        down_.action = Action::MoveDown;
        down_.center = raylib::Vector2{ dpadCX, dpadCY + dpadGap };
        down_.radius = dpadR;
        down_.rotationDeg = 90.f;    // rotated to point down

        // ---- Action buttons (right side) ---------------------------------
        const float btnR = sh * 0.048f;
        const float btnCX = sw * 0.82f;
        const float btnCY = sh * 0.75f;
        const float btnGap = btnR * 2.6f;

        interact_.action = Action::Interact;
        interact_.center = raylib::Vector2{ btnCX + btnGap * 0.5f, btnCY - btnGap * 0.5f };
        interact_.radius = btnR;

        undo_.action = Action::Undo;
        undo_.center = raylib::Vector2{ btnCX - btnGap * 0.5f, btnCY - btnGap * 0.5f };
        undo_.radius = btnR;

        restart_.action = Action::Restart;
        restart_.center = raylib::Vector2{ btnCX + btnGap * 0.5f, btnCY + btnGap * 0.5f };
        restart_.radius = btnR;

        pause_.action = Action::Pause;
        pause_.center = raylib::Vector2{ btnCX - btnGap * 0.5f, btnCY + btnGap * 0.5f };
        pause_.radius = btnR;
    }

    // ---- Per-frame input ----------------------------------------------------

    void update() {
        prevPressed_ = pressed_;
        pressed_ = {}; // reset all flags to false

        int touchCount = GetTouchPointCount();
        for (int i = 0; i < touchCount; ++i) {
            CheckPoint(GetTouchPosition(i));
        }

        // Mouse as a fake touch point, for desktop testing.
#ifndef PLATFORM_WEB
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            CheckPoint(GetMousePosition());
        }
#endif
    }

    bool isHeld(Action action) const {
        return GetFlag(pressed_, action);
    }

    bool isPressed(Action action) const {
        return GetFlag(pressed_, action) && !GetFlag(prevPressed_, action);
    }

    // ---- Drawing --------------------------------------------------------------

    void draw() const {
        left_.Draw(isHeld(Action::MoveLeft));
        right_.Draw(isHeld(Action::MoveRight));
        up_.Draw(isHeld(Action::MoveUp));
        down_.Draw(isHeld(Action::MoveDown));

        interact_.Draw(isHeld(Action::Interact));
        undo_.Draw(isHeld(Action::Undo));
        restart_.Draw(isHeld(Action::Restart));
        pause_.Draw(isHeld(Action::Pause));
    }

private:
    // Flags for exactly the 8 known actions this gamepad produces. A small
    // fixed struct (rather than a map/set keyed by Action) keeps "what can
    // this gamepad ever report" obvious at a glance, matching the fact that
    // this button set is intentionally fixed rather than extensible.
    struct PressedFlags {
        bool moveLeft = false, moveRight = false, moveUp = false, moveDown = false;
        bool interact = false, undo = false, restart = false, pause = false;
    };

    static bool GetFlag(const PressedFlags& flags, Action action) {
        switch (action) {
        case Action::MoveLeft:  return flags.moveLeft;
        case Action::MoveRight: return flags.moveRight;
        case Action::MoveUp:    return flags.moveUp;
        case Action::MoveDown:  return flags.moveDown;
        case Action::Interact:  return flags.interact;
        case Action::Undo:      return flags.undo;
        case Action::Restart:   return flags.restart;
        case Action::Pause:     return flags.pause;
        default:                return false;
        }
    }

    static void SetFlag(PressedFlags& flags, Action action) {
        switch (action) {
        case Action::MoveLeft:  flags.moveLeft = true; break;
        case Action::MoveRight: flags.moveRight = true; break;
        case Action::MoveUp:    flags.moveUp = true; break;
        case Action::MoveDown:  flags.moveDown = true; break;
        case Action::Interact:  flags.interact = true; break;
        case Action::Undo:      flags.undo = true; break;
        case Action::Restart:   flags.restart = true; break;
        case Action::Pause:     flags.pause = true; break;
		default: 			    break; // ignore unknown actions
        }
    }

    void CheckPoint(raylib::Vector2 pt) {
        if (left_.Contains(pt))     SetFlag(pressed_, Action::MoveLeft);
        if (right_.Contains(pt))    SetFlag(pressed_, Action::MoveRight);
        if (up_.Contains(pt))       SetFlag(pressed_, Action::MoveUp);
        if (down_.Contains(pt))     SetFlag(pressed_, Action::MoveDown);
        if (interact_.Contains(pt)) SetFlag(pressed_, Action::Interact);
        if (undo_.Contains(pt))     SetFlag(pressed_, Action::Undo);
        if (restart_.Contains(pt))  SetFlag(pressed_, Action::Restart);
        if (pause_.Contains(pt))    SetFlag(pressed_, Action::Pause);
    }

    // D-pad
    DpadButton left_, right_, up_, down_;
    // Action buttons
    ActionButton interact_, undo_, restart_, pause_;

    // Textures owned here; buttons above hold non-owning pointers into these.
    raylib::Texture2D dpadTexture_;
    raylib::Texture2D interactTexture_;
    raylib::Texture2D undoTexture_;
    raylib::Texture2D restartTexture_;
    raylib::Texture2D pauseTexture_;

    PressedFlags pressed_;
    PressedFlags prevPressed_;
};