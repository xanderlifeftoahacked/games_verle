#pragma once
#include "Constants.hpp"
#include "Grid.hpp"
#include "Link.hpp"
#include "Vector2.hpp"
#include "VerletObject.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Window.hpp>
#include <functional>
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
    int iterations = 4;
    dt = dt / float(iterations);
    while (iterations--) {
      applyGravity();
      applyLinks();
#ifdef FIRSTCASE
      applyConstraint();
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
  //
  // void makeBoom(int x, int y) {
  //   VerletObject *obj = new VerletObject(x, y, constants::objRadius, true,
  //                                        sf::Color::Transparent);
  //   explosionObjects.push_back(obj);
  // }

private:
  CollisionGrid grid;
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
    for (auto *object : objects) {
      if (object == nullptr)
        return;
      object->updatePosition(dt);
    }
  }

  void drawObjects() {
    auto object = std::begin(objects);
    while (object++ != std::end(objects)) {
      if (*object == nullptr)
        return;
      window->draw((*object)->sfShape);
    }
  }

  void drawLinks() {
    auto link = std::begin(links);
    while (link++ != std::end(links)) {
      if (*link == nullptr)
        return;
      if ((*link)->object_1 == nullptr || (*link)->object_2 == nullptr) {
        link = links.erase(link);
        continue;
      }
      window->draw((*link)->line, 2, sf::Lines);
    }
  }

  void applyGravity() {
    for (auto *object : objects) {
      if (object == nullptr)
        return;
      object->accelerate(constants::gravity);
    }
  }

  void applyLinks() {
    for (auto *link : links) {
      if (link == nullptr)
        return;
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

  void solveCollisions() {
    std::vector<std::thread *> threads;
    for (int x = 0; x < constants::numberOfThreadsX; ++x) {
      for (int y = 0; y < constants::numberOfThreadsY; ++y) {
        solveCollisionsThread(x, y);
        // std::thread th([this, x, y]() { this->solveCollisionsThread(x, y);
        // }); th.join();
      }
    }
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
