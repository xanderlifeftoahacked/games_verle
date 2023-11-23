#pragma once
#include <cmath>

namespace eng {
template <typename T> struct Vec2 {
  T x, y;

  Vec2() : x{0}, y{0} {};
  Vec2(T _x, T _y) : x{_x}, y{_y} {};

  T length() const { return std::sqrt(x * x + y * y); }

  Vec2 &operator=(const Vec2 &other) {
    x = other.x;
    y = other.y;
    return *this;
  }

  Vec2 operator+(const Vec2 &other) const {
    return Vec2{x + other.x, y + other.y};
  }
  Vec2 operator-(const Vec2 &other) const {
    return Vec2{x - other.x, y - other.y};
  }

  void operator+=(const Vec2 &other) {
    x += other.x;
    y += other.y;
  }
  void operator-=(const Vec2 &other) {
    x -= other.x;
    y -= other.y;
  }

  Vec2 operator*(const T value) const { return Vec2{x * value, y * value}; }
  Vec2 operator/(const T value) const { return Vec2{x / value, y / value}; }
};
} // namespace eng
