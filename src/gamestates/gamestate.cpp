#include "gamestate.h"

static gamestate::t *current = nullptr;

namespace gamestate
{
  int switchTo(gamestate::t &next)
  {
    if(current)
    {
      current->leave(next);
      next.enter(*current);
    }
    else
      next.enter(next);
    current = &next;
  }

  int draw()
  {
    current->draw();
  }

  int update(float dt)
  {
    return current->update(dt);
  }

  int treatEvent(SDL_Event &event)
  {
    return current->treatEvent(event);
  }
}
