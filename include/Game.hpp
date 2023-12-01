#pragma once
#include "Constants.hpp"
#include "Link.hpp"
#include "Vector2.hpp"
#include "VerletObject.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Window.hpp>
#include <functional>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

namespace eng {
class Game {
public:
  Game(sf::RenderWindow *_window) : window{_window} {
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
       << "Frametime: " << dt * 1000 << " ms";
    std::string stats(std::istreambuf_iterator<char>(ss), {});

    statistics.setString(stats);
    int iterations = 5;
    dt = dt / float(iterations);
    while (iterations--) {
      applyGravity();
      applyLinks();
      // applyConstraint();
      solveCollisions();
      updatePositions(dt);
    }

    auto object = std::begin(objects);
    while (object != std::end(objects)) {
      if (!(*object)->isOnScreen()) {
        object = objects.erase(object);
        continue;
      }
      window->draw((*object++)->sfShape);
    }

    auto link = std::begin(links);
    while (link != std::end(links)) {
      if (!(*link)->object_1->isOnScreen() &&
          !(*link)->object_2->isOnScreen()) {
        link = links.erase(link);
        continue;
      }
      window->draw((*link++)->line, 2, sf::Lines);
    }

    window->draw(statistics);
  }

private:
  std::vector<VerletObject *> objects;
  std::vector<Link *> links;
  sf::RenderWindow *window;
  sf::Text statistics;
  sf::Font font;

  void updatePositions(float dt) {
    for (auto *object : objects) {
      object->updatePosition(dt);
    }
  }

  void applyGravity() {
    for (auto *object : objects) {
      object->accelerate(constants::gravity);
    }
  }

  void applyLinks() {
    for (auto *link : links) {
      link->apply();
    }
  }

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

  void solveCollisions() {
    for (int i = 0; i < objects.size(); ++i) {
      for (int j = 0; j < objects.size(); ++j) {
        if (j == i)
          continue;

        Vec2<float> collisionAxis =
            objects[i]->positionCurrent - objects[j]->positionCurrent;

        const float dist = collisionAxis.length();
        if (dist > objects[i]->radius + objects[j]->radius)
          continue;

        Vec2<float> normalized = collisionAxis / dist;
        const float delta = objects[i]->radius + objects[j]->radius - dist;
        float weightDiff =
            objects[j]->radius / (objects[i]->radius + objects[j]->radius);

        objects[i]->positionCurrent += normalized * delta * weightDiff;
        objects[j]->positionCurrent -= normalized * delta * (1 - weightDiff);
      }
    }
  }
};
}; // namespace eng
