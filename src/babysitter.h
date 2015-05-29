#pragma once

#include <functional>

namespace babysitter
{
  int update(float dt);

  int sit(std::function<float(float progress, float dt)> baby_step);

  int clear();
}
