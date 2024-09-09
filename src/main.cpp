#include "Constants.hpp"
#include "Game.hpp"
#include "Utils.hpp"
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Mouse.hpp>
#include <SFML/Window/VideoMode.hpp>
#include <SFML/Window/Window.hpp>

int main() {
  sf::RenderWindow window(
      sf::VideoMode(constants::screenWidth, constants::screenHeight), "Satris");
  eng::Game game(&window);

#ifdef FIRSTCASE
  sf::CircleShape area;
  area.setOrigin(constants::areaRadius, constants::areaRadius);
  area.setPosition(constants::areaX, constants::areaY);
  area.setRadius(constants::areaRadius);
  area.setFillColor(sf::Color::White);
  area.setPointCount(200);
#endif

#ifdef SECONDCASE
  for (size_t x = constants::boxX1 + 300; x <= constants::boxX2 - 300;
       x += constants::objRadius * 2) {
    game.addObject(x, constants::boxY1 + 400, constants::objRadius);
  }

  auto objs = game.getObjects();
  objs[0]->isMoveable = false,
  objs[game.getCountOfObjects() - 1]->isMoveable = false;
  for (int i = 0; i < game.getCountOfObjects() - 1; ++i) {
    game.addLink(objs[i], objs[i + 1], constants::objRadius * 2);
  }

  for (size_t x = constants::boxX1; x <= constants::boxX2;
       x += constants::objRadius * 2) {
    game.addObject(x, constants::boxY1, constants::objRadius, false);
    game.addObject(x, constants::boxY2, constants::objRadius, false);
  }

  for (size_t y = constants::boxY1; y <= constants::boxY2;
       y += constants::objRadius * 2) {
    game.addObject(constants::boxX1, y, constants::objRadius, false);
    game.addObject(constants::boxX2, y, constants::objRadius, false);
  }
#endif

  for (size_t x = constants::boxX1; x <= constants::boxX1 * 3;
       x += constants::objRadius * 2) {
    game.addObject(x, constants::boxY1, constants::objRadius, false);
  }

  for (size_t y = constants::boxY1; y <= constants::boxY1 * 3;
       y += constants::objRadius * 2) {
    game.addObject(constants::boxX1, y, constants::objRadius, false);
  }

  sf::Clock deltaClock;
  sf::Time dt;
  game.firstUnstatic = game.getCountOfObjects();
  unsigned short int counter = 0;

  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        window.close();
    }

    ++counter;
#ifdef RAINCASE
    if (counter % 1 == 0) {

      game.addObject(constants::boxX1 + constants::objRadius - 1,
                     constants::boxY1 + constants::objRadius,
                     constants::objRadius, true, utils::getRainbow(counter));
    }
#endif

    if (sf::Mouse::isButtonPressed(sf::Mouse::Right) && counter % 40 == 0) {
      sf::Vector2i position = sf::Mouse::getPosition(window);
      game.addSquare(position.x, position.y, utils::getRandomInt(1, 10) * 10,
                     utils::getRainbow(counter));
      // game.addCircle(position.x, position.y);
    }

    if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && counter % 5 == 0) {
      game.startMoving();
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
