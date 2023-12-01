#pragma once
#include "Constants.hpp"
#include "Vector2.hpp"
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Color.hpp>
#include <iostream>

namespace eng {
struct VerletObject {
  Vec2<float> positionCurrent;
  Vec2<float> positionOld;
  Vec2<float> acceleration;
  bool isMoveable;
  sf::CircleShape sfShape;
  float radius;

  VerletObject(float xPos, float yPos, float _radius, bool _isMoveable,
               sf::Color color)
      : positionCurrent{xPos, yPos}, positionOld{xPos, yPos}, radius{_radius},
        isMoveable{_isMoveable} {
    sfShape.setRadius(radius);
    sfShape.setOrigin(radius, radius);
    sfShape.setPosition(xPos, yPos);
    sfShape.setFillColor(color);
  }

  void updatePosition(float dt) {
    if (!isMoveable) {
      positionCurrent = positionOld;
      return;
    }

    Vec2<float> velocity = positionCurrent - positionOld;
    positionOld = positionCurrent;

    positionCurrent += velocity + constants::gravity * dt * dt;

    acceleration = {};

    sfShape.setPosition(positionCurrent.x, positionCurrent.y);
  }

  bool isOnScreen() {
    if (positionCurrent.x > -constants::worldBorderFromScreen &&
        positionCurrent.x <
            constants::screenWidth + constants::worldBorderFromScreen &&
        positionCurrent.y > -constants::worldBorderFromScreen &&
        positionCurrent.y <
            constants::screenWidth + constants::worldBorderFromScreen)
      return true;
    return false;
  }

  void accelerate(Vec2<float> acc) { acceleration += acc; }
};
} // namespace eng
