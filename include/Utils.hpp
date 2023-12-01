#pragma once
#include <SFML/Graphics/Color.hpp>
#include <math.h>
#include <numbers>

namespace utils {
static sf::Color getRainbow(float t) {
  const float r = sin(t);
  const float g = sin(t + 0.33f * 2.0f * std::numbers::pi);
  const float b = sin(t + 0.66f * 2.0f * std::numbers::pi);
  return sf::Color(255 * r * r, 255 * g * g, 0);
}
} // namespace utils
