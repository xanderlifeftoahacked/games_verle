#include "Constants.hpp"
#include "Game.hpp"
#include "Random.hpp"
#include "Utils.hpp"
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
  // window.setVerticalSyncEnabled(1);
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

#ifdef SECONDCASE
  for (int x = constants::boxX1 + 500; x <= constants::boxX2 - 500;
       x += constants::objRadius * 2) {
    game.addObject(x, constants::boxY1 + 400, constants::objRadius);
  }

  auto objs = game.getObjects();
  objs[0]->isMoveable = false,
  objs[game.getCountOfObjects() - 1]->isMoveable = false;

  for (int i = 0; i < game.getCountOfObjects() - 1; ++i) {
    game.addLink(objs[i], objs[i + 1], constants::objRadius * 2);
  }

  for (int x = constants::boxX1; x <= constants::boxX2;
       x += constants::objRadius * 2) {
    game.addObject(x, constants::boxY1, constants::objRadius, false);
    game.addObject(x, constants::boxY2, constants::objRadius, false);
  }

  for (int y = constants::boxY1; y <= constants::boxY2;
       y += constants::objRadius * 2) {
    game.addObject(constants::boxX1, y, constants::objRadius, false);
    game.addObject(constants::boxX2, y, constants::objRadius, false);
  }

#endif

  unsigned short int counter = 0;
  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        window.close();
    }
    if (counter++ % 1 == 0) {
      game.addObject(constants::boxX1 + constants::objRadius,
                     constants::boxY1 + 20, constants::objRadius, true,
                     utils::getRainbow(counter));

      game.addObject(constants::boxX2 - constants::objRadius,
                     constants::boxY1 + 20 * 3, constants::objRadius, true,
                     utils::getRainbow(counter));
    }

    if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
      sf::Vector2i position = sf::Mouse::getPosition(window);
      game.addObject(position.x, position.y, constants::objRadius, true,
                     sf::Color::Yellow);
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
