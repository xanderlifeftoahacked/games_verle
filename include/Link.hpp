#pragma once
#include "Vector2.hpp"
#include "VerletObject.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/System/Vector2.hpp>

struct Link {
  eng::VerletObject *object_1;
  eng::VerletObject *object_2;
  float targetDist;
  sf::Vertex line[2];

  Link(eng::VerletObject *obj_1, eng::VerletObject *obj_2, float targDist)
      : object_1{obj_1}, object_2{obj_2}, targetDist{targDist} {}

  void apply() {
    const eng::Vec2<float> axis =
        object_1->positionCurrent - object_2->positionCurrent;
    const float dist = axis.length();
    const eng::Vec2<float> normalized = axis / dist;
    const float delta = targetDist - dist;
    object_1->positionCurrent += normalized * delta * 0.5f;
    object_2->positionCurrent -= normalized * delta * 0.5f;

    line[0] = sf::Vertex(
        sf::Vector2f(object_1->positionCurrent.x, object_1->positionCurrent.y),
        sf::Color(42, 156, 99));
    line[1] = sf::Vertex(
        sf::Vector2f(object_2->positionCurrent.x, object_2->positionCurrent.y),
        sf::Color(42, 156, 99));
  }
};
