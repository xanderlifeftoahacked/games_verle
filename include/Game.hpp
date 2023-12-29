#pragma once
#include "Constants.hpp"
#include "Grid.hpp"
#include "Link.hpp"
#include "ThreadPool.hpp"
#include "Utils.hpp"
#include "Vector2.hpp"
#include "VerletObject.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Window.hpp>
#include <cmath>
#include <functional>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

namespace eng {
class Game {
public:
  int firstUnstatic;

  Game(sf::RenderWindow *_window) : window{_window}, grid{}, objects{} {
    font.loadFromFile("/usr/share/fonts/TTF/FiraCode-Bold.ttf");
    statistics.setPosition(20, 20);
    statistics.setFillColor(sf::Color::White);
    statistics.setFont(font);
    threadpool.start();
  };

  std::vector<VerletObject *> getObjects() { return objects; }

  void addObject(float xPos, float yPos, float radius, bool isMoveable = true,
                 sf::Color color = sf::Color(sf::Color::White)) {
    VerletObject *obj = new VerletObject(xPos, yPos, radius, isMoveable, color);
    objects.push_back(obj);
  }

  VerletObject *getLastObj() { return objects[objects.size() - 1]; }

  void addLink(eng::VerletObject *obj_1, eng::VerletObject *obj_2,
               float targDist) {
    Link *link = new Link(obj_1, obj_2, targDist);
    links.push_back(link);
  }

  void update(float dt) {
    std::stringstream ss;
    ss << "Objects: " << objects.size() << '\n'
       << "Links: " << links.size() << '\n'
       << "Frametime: " << dt * 1000 << " ms\n"
       << "Frametime to Objects: " << dt * 1000 / objects.size() << '\n';
    std::string stats(std::istreambuf_iterator<char>(ss), {});

    statistics.setString(stats);
    int iterations = constants::physicSteps;
    dt = dt / static_cast<float>(iterations);
    while (iterations--) {
      applyGravity();
      applyLinks();
#ifdef FIRSTCASE
      applyConstraint();
#endif
#ifdef SCREENCONSTRAINT
      apllyScreenConstraint();
#endif
      solveCollisions();
      updatePositions(dt);
      grid.updateGrid(objects);
    }
    physTime = deltaClock.restart();
    drawObjects();
    drawLinks();

    window->draw(statistics);
  }

  int getCountOfObjects() { return objects.size(); }

  void addCircle(const float pos_x, const float pos_y,
                 const float radius = 50) {
    const int indexOfCenter = this->getCountOfObjects();
    const int numberOfObjects = 16;
    const float radStep = std::numbers::pi / numberOfObjects * 2;
    addObject(pos_x, pos_y, constants::objRadius, false);

    for (int i = 0; i < numberOfObjects; ++i) {
      float x = pos_x + radius * std::sin(radStep * i);
      float y = pos_y + radius * std::cos(radStep * i);
      addObject(x, y, constants::objRadius, false);
      if (!(i % 2))
        addLink(objects[objects.size() - 1], objects[indexOfCenter], radius);

      auto cur = objects[objects.size() - 1];
      auto prev = objects[objects.size() - 2];
      float dist = std::sqrt(
          std::pow((cur->positionCurrent.x - prev->positionCurrent.x), 2) +
          std::pow((cur->positionCurrent.y - prev->positionCurrent.y), 2));

      addLink(cur, prev, dist);
    }

    auto cur = objects[indexOfCenter + 1];
    auto prev = objects[objects.size() - 1];
    float dist = std::sqrt(
        std::pow((cur->positionCurrent.x - prev->positionCurrent.x), 2) +
        std::pow((cur->positionCurrent.y - prev->positionCurrent.y), 2));

    addLink(cur, prev, dist);
  }

  void addSquare(float pos_x, float pos_y, float size, sf::Color color) {

    int objectsInRow = size / constants::objRadius / 2 + 1;
    float linklength = constants::objRadius * 2.f;
    std::vector<std::vector<VerletObject *>> square(
        objectsInRow, std::vector<VerletObject *>(objectsInRow));

    for (float y = pos_y, i = 0; y <= pos_y + size;
         y += constants::objRadius * 2, ++i) {
      for (float x = pos_x, j = 0; x <= pos_x + size;
           x += constants::objRadius * 2, ++j) {

        addObject(x, y, constants::objRadius, false, color);
        square[i][j] = objects[objects.size() - 1];
      }
    }

    addLink(square[0][0], square[0][1], linklength);
    for (int y = 0; y < objectsInRow; ++y) {
      for (int x = 0; x < objectsInRow; ++x) {
        if (x < objectsInRow - 1)
          addLink(square[y][x], square[y][x + 1], linklength);
        if (y >= 1)
          addLink(square[y][x], square[y - 1][x], linklength);
        if (y >= 1 && x >= 1)
          addLink(square[y][x], square[y - 1][x - 1], linklength * sqrt(2));
      }
    }
  }

  void startMoving() {
    for (auto obj = objects.begin() + firstUnstatic; obj != objects.end();
         ++obj) {
      (*obj)->isMoveable = true;
    }
  }

private:
  utils::ThreadPool threadpool;
  CollisionGrid grid;
  sf::Clock deltaClock;
  sf::Time physTime;
  std::vector<VerletObject *> objects;
  std::vector<Link *> links;
  sf::RenderWindow *window;
  sf::Text statistics;
  sf::Font font;

  void updatePositions(float dt) {
    if (!objects.size())
      return;

    for (auto *object : objects) {
      object->updatePosition(dt);
    }
  }

  void drawObjects() {
    if (!objects.size())
      return;

    for (auto &obj : objects) {
      window->draw(obj->sfShape);
    }
  }

  void drawLinks() {
    if (!links.size())
      return;

    auto link = std::begin(links);
    while (++link != std::end(links)) {
      if ((*link)->object_1 == nullptr || (*link)->object_2 == nullptr) {
        link = links.erase(link);
        continue;
      }
      window->draw((*link)->line, 2, sf::Lines);
    }
  }

  void applyGravity() {
    if (!objects.size())
      return;

    for (auto *object : objects) {
      object->accelerate(constants::gravity);
    }
  }

  void applyLinks() {
    if (!links.size())
      return;

    for (auto *link : links) {
      link->apply();
    }
  }

#ifdef FIRSTCASE
  void applyConstraint() {
    const Vec2 centerPosition{constants::areaX, constants::areaY};

    for (auto *object : objects) {
      const Vec2 vecToObj = object->positionCurrent - centerPosition;
      const float distToObj = vecToObj.length();

      if (distToObj > constants::areaRadius - object->radius) {
        const Vec2<float> normalized = vecToObj / distToObj;
        object->positionCurrent =
            centerPosition +
            normalized * (constants::areaRadius - object->radius);
      }
    }
  }
#endif

#ifdef SCREENCONSTRAINT
  void apllyScreenConstraint() {
    for (auto *object : objects) {
      if (object->positionCurrent.x > constants::boxX2) {
        object->positionCurrent.x -=
            (object->positionCurrent.x - constants::boxX2);
      }
      if (object->positionCurrent.x < constants::boxX1) {
        object->positionCurrent.x +=
            abs(object->positionCurrent.x - constants::boxX1);
      }
      if (object->positionCurrent.y > constants::boxY2) {
        object->positionCurrent.y -=
            (object->positionCurrent.y - constants::boxY2);
      }
      if (object->positionCurrent.y < constants::boxY1) {
        object->positionCurrent.y -=
            (object->positionCurrent.y - constants::boxY1);
      }
    }
  }
#endif

  void solveCollisions() {
    for (int x = 0; x < constants::numberOfThreadsX; ++x) {
      for (int y = 0; y < constants::numberOfThreadsY; ++y) {
#ifdef ONETHREAD
        solveCollisionsThread(x, y);
#endif
#ifndef ONETHREAD
        threadpool.queueJob(
            [this, x, y]() { this->solveCollisionsThread(x, y); });
#endif
      }
    }
    threadpool.waitForCompletion();
  }

  void solveCollisionsThread(int threadNumberX, int threadNumberY) {
    int xstart =
        1 + (grid.width - 2) / constants::numberOfThreadsX * threadNumberX;
    int xend = xstart + (grid.width - 2) / constants::numberOfThreadsX - 1;

    int ystart =
        1 + (grid.height - 2) / constants::numberOfThreadsY * threadNumberY;
    int yend = ystart + (grid.height - 2) / constants::numberOfThreadsY - 1;

    for (int x = xstart; x <= xend; ++x) {
      for (int y = ystart; y <= yend; ++y) {
        grid.collidecell(x, y);
      }
    }
  }
};
} // namespace eng
