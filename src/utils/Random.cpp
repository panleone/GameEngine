#include "Random.h"

#include <random>
#include <chrono>
#include <optional>

float rng::randomFloat(float min, float max) {
  std::uniform_real_distribution<float> unif(min, max);
  // Create the random seed once
  static std::optional<std::default_random_engine> re{std::nullopt};
  if (!re) {
    re = std::default_random_engine{};
    re->seed(std::chrono::system_clock::now().time_since_epoch().count());
  }
  return unif(*re);
}
float rng::randomAngle() { return rng::randomFloat(0, 2 * M_PI); }