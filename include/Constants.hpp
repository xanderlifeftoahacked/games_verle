#pragma once
#include "Vector2.hpp"

// #define FIRSTCASE 0;
#define SECONDCASE 1;
namespace constants {
const inline eng::Vec2 gravity = {0.0f, 1000.0f};
const inline int screenWidth = 1280;
const inline int screenHeight = 720;
const inline float areaRadius = 300.f;
const inline float areaX = constants::screenWidth / 2.f;
const inline float areaY = constants::screenHeight / 2.f;
const inline float objRadius = 10;
const inline int worldBorderFromScreen = 1000;
} // namespace constants
