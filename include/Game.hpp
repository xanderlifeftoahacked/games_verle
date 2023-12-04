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

  void addLink(eng::VerletObject *obj_1, eng::VerletObject *obj_2,
               float targDist) {
    Link *link = new Link(obj_1, obj_2, targDist);
    links.push_back(link);
  }

  void update(float dt) {
    std::stringstream ss;
    ss << "Objects: " << objects.size() << '\n'
       << "Frametime: " << dt * 1000 << " ms\n"
       << "PhysicsTime: " << physTime.asSeconds() * 1000 << " ms";
    std::string stats(std::istreambuf_iterator<char>(ss), {});

    statistics.setString(stats);
    int iterations = constants::physicSteps;
    dt = dt / float(iterations);
    while (iterations--) {
      applyGravity();
      applyLinks();
#ifdef FIRSTCASE
      applyConstraint();
#endif
#ifdef SECONDCASE
      // apllyScreenConstraint();
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
        // if (x < objectsInRow - 1 && y >= 1 && utils::getRandomInt(0, 1))
        //   addLink(square[y][x], square[y - 1][x + 1], linklength * sqrt(2));
      }
    }
  }

  void startMoving() {
    for (auto obj = objects.begin() + firstUnstatic; obj != objects.end();
         ++obj) {
      (*obj)->isMoveable = true;
    }
  }
  //
  // void makeBoom(int x, int y) {
  //   VerletObject *obj = new VerletObject(x, y, constants::objRadius, true,
  //                                        sf::Color::Transparent);
  //   explosionObjects.push_back(obj);
  // }

private:
  utils::ThreadPool threadpool;
  CollisionGrid grid;
  int firstUnstatic = constants::staticObjectsCount;
  sf::Clock deltaClock;
  sf::Time physTime;
  // int objectsCount = 0;
  std::vector<VerletObject *> objects;
  // std::vector<VerletObject *> explosionObjects;
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
#ifdef SECONDCASE
  void apllyScreenConstraint() {
    for (auto *object : objects) {
      if (object->positionCurrent.x > constants::boxX2) {
        object->positionCurrent.x -=
            (object->positionCurrent.x - constants::boxX2);
      }
    }
  }
#endif

  void solveCollisions() {
    std::vector<std::thread> threads;
    for (int x = 0; x < constants::numberOfThreadsX; ++x) {
      for (int y = 0; y < constants::numberOfThreadsY; ++y) {
        // solveCollisionsThread(x, y);
        threadpool.queueJob(
            [this, x, y]() { this->solveCollisionsThread(x, y); });
      }
    }

    // for (auto &th : threads) {
    //   th.join();
    // }
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
