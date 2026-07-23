#pragma once

struct Cell {
    int x{ 0 };
    int y{ 0 };

    constexpr Cell() = default;
    constexpr Cell(int x, int y) : x(x), y(y) {}

    constexpr Cell operator+(const Cell& other) const
    {
        return {
            x + other.x,
            y + other.y
        };
    }

    constexpr Cell& operator+=(const Cell& other)
    {
        x += other.x;
        y += other.y;
        return *this;
    }

    constexpr Cell operator-(const Cell& other) const
    {
        return {
            x - other.x,
            y - other.y
        };
    }

    constexpr Cell& operator-=(const Cell& other)
    {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    constexpr Cell operator*(int scalar) const
    {
        return { x * scalar, y * scalar };
    }

    constexpr Cell& operator*=(int scalar)
    {
        x *= scalar;
        y *= scalar;
        return *this;
    }

    bool operator==(const Cell&) const = default;
};

constexpr Cell operator*(int scalar, const Cell& cell)
{
    return cell * scalar;
}