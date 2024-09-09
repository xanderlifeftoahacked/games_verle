#pragma once

#include "Constants.hpp"
#include "VerletObject.hpp"
#include <SFML/Graphics/Color.hpp>
#include <iterator>
#include <mutex>
#include <vector>

namespace eng {
struct Cell {
  std::vector<VerletObject *> cellObjects;

  Cell() : cellObjects{} {}
};

struct CollisionGrid {
  const int width =
      (constants::screenWidth + 2 * constants::worldBorderFromScreen) /
      constants::gridStep;
  const int height =
      (constants::screenWidth + 2 * constants::worldBorderFromScreen) /
      constants::gridStep;

  std::vector<std::vector<Cell>> grid;
  std::mutex mutex;

  CollisionGrid() : grid(height, std::vector<Cell>(width)) {}

  void updateGrid(std::vector<VerletObject *> &objects) {
    for (int x = 0; x < width; ++x) {
      for (int y = 0; y < height; ++y) {
        grid[y][x].cellObjects.clear();
      }
    }

    auto object = std::begin(objects);
    while (object++ != std::end(objects)) {
      if (*object == nullptr || object == std::end(objects))
        continue;
      int objx =
          (*object)->positionCurrent.x + constants::worldBorderFromScreen;
      int objy =
          (*object)->positionCurrent.y + constants::worldBorderFromScreen;

      if (objx / constants::gridStep >= width ||
          objy / constants::gridStep >= height || objx < 0 || objy < 0) {
        objects.erase(object);
        continue;
      }

      grid[objy / constants::gridStep][objx / constants::gridStep]
          .cellObjects.push_back(*object);
    }
  }

  bool isCollideable(VerletObject *object1, VerletObject *object2) {
    if (object1 == object2 || !object1->isMoveable)
      return false;
    return true;
  }

  void collide(VerletObject *object1, VerletObject *object2) {
    Vec2<float> collisionAxis =
        object1->positionCurrent - object2->positionCurrent;
    const float dist = collisionAxis.length();

    if (dist > object1->radius + object2->radius)
      return;

    Vec2<float> normalized = collisionAxis / dist;
    const float delta = object1->radius + object2->radius - dist;

    object1->positionCurrent += normalized * delta * 0.5f;
    object2->positionCurrent -= normalized * delta * 0.5f;
  }

  void collidecell(int x, int y) {
    for (auto object1 : grid[y][x].cellObjects) {
      for (int dx = -1; dx < 2; ++dx) {
        for (int dy = -1; dy < 2; ++dy) {
          for (auto object2 : grid[y + dy][x + dx].cellObjects) {
            if (!isCollideable(object1, object2))
              continue;
            collide(object1, object2);
          }
        }
      }
    }
  }
};
} // namespace eng
