#include "Constants.hpp"
#include "Game.hpp"
#include "Random.hpp"
#include "Vector2.hpp"
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Mouse.hpp>
#include <SFML/Window/VideoMode.hpp>
#include <SFML/Window/Window.hpp>
#include <iostream>
#include <math.h>

int main() {
  sf::RenderWindow window(
      sf::VideoMode(constants::screenWidth, constants::screenHeight), "Satris");
  window.setVerticalSyncEnabled(1);
  eng::Game game(&window);

#ifdef FIRSTCASE
  sf::CircleShape area;
  area.setOrigin(constants::areaRadius, constants::areaRadius);
  area.setPosition(constants::areaX, constants::areaY);
  area.setRadius(constants::areaRadius);
  area.setFillColor(sf::Color::White);
  area.setPointCount(200);
#endif

  sf::Clock deltaClock;
  sf::Time dt;
  unsigned short int counter_r = 0;
  unsigned short int counter_g = 0;
  unsigned short int counter = 0;

#ifdef SECONDCASE
  for (int x = 400; x <= constants::screenWidth - 400;
       x += constants::objRadius * 2) {
    game.addObject(x, 400, constants::objRadius);
  }
  auto objs = game.getObjects();
  objs[0]->isMoveable = false, objs[objs.size() - 1]->isMoveable = false;

  for (int i = 0; i < objs.size() - 1; ++i) {
    game.addLink(objs[i], objs[i + 1], constants::objRadius * 2);
  }
#endif

  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        window.close();
    }

    if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
      sf::Vector2i position = sf::Mouse::getPosition(window);
      game.addObject(position.x, position.y, constants::objRadius, true,
                     sf::Color(++counter_r % 256, --counter_g % 256, 10));
    }

    window.clear();

#ifdef FIRSTCASE
    window.draw(area);
#endif

    game.update(dt.asSeconds());
    window.display();

    dt = deltaClock.restart();
  }
  return 0;
}
