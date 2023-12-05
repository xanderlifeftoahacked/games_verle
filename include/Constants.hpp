#pragma once
#include "Vector2.hpp"

// #define FIRSTCASE
// #define SECONDCASE
#define RAINCASE
namespace constants {
const inline eng::Vec2 gravity = {0.0f, 1000.0f};
const inline int screenWidth = 1296;
const inline int screenHeight = 816;
const inline float areaRadius = 300.f;
const inline float areaX = constants::screenWidth / 2.f;
const inline float areaY = constants::screenHeight / 2.f;
const inline float objRadius = 4.f;
const inline int physicSteps = 8;
const inline int gridStep = 2 * objRadius;
const inline int numberOfThreadsX = 4;
const inline int numberOfThreadsY = 2;
const inline int worldBorderFromScreen = 200;
const inline int boxX1 = 50;
const inline int boxY1 = 50;
const inline int boxX2 = screenWidth - boxX1;
const inline int boxY2 = screenHeight - boxY1;
} // namespace constants
