#include "ingame.h"

#include "title.h"

#include "../math/wjd_math.h"
#include "../math/Rect.hpp"
#include "../graphics/Texture.hpp"
#include "../global.hpp"

#include "../debug/assert.h"

#define GRID_W 300
#define GRID_H 300
#define TILE_LAVA 1
#define TILE_ICE 2

static Texture atlas;
static int tiles[GRID_W][GRID_H];
static fRect sprite(0, 0, 4, 4),
  lava(0, 0, 32, 32),
  ice(32, 0, 32, 32);

namespace gamestate
{

static gamestate::t state;

static bool initialised = false;

static int init()
{
  state.update = [](float dt)
  {


    // All okay
    return 0;
  };

  state.draw = []()
  {

    // Draw the grid
    for(int x = 0; x < GRID_W; x++)
    for(int y = 0; y < GRID_H; y++)
    {
      sprite.x = sprite.w*x;
      sprite.y = sprite.h*y;
      switch(tiles[x][y])
      {
        case TILE_ICE:
          atlas.draw(&ice, &sprite);
          break;

        case TILE_LAVA:
          atlas.draw(&lava, &sprite);
          break;
      }
    }

    // All okay
    return 0;
  };

  state.treatEvent = [](SDL_Event &event)
  {
    switch (event.type)
    {
      // Exit if the window is closed
      // (ex: pressing the cross at the top)
      case SDL_QUIT:
        return EVENT_QUIT;
      break;

      // Check for key-presses
      case SDL_KEYDOWN:
        switch (event.key.keysym.sym)
        {
          case SDLK_RETURN:
          break;

          case SDLK_ESCAPE:
            return gamestate::switchTo(title::get());
          default:
            break;
        }
      break;

      default:
        // not all possible inputs are needed, so we DO want a default break
      break;
    }


    // All okay
    return 0;
  };

  state.leave = [](gamestate::t &next)
  {
    log("Leaving ingame");

    // Unload all the assets we used
    atlas.unload();

    // All okay
    return 0;
  };

  state.enter = [](gamestate::t &previous)
  {
    log("Entering ingame");

    // Reset state
    for(int x = 0; x < GRID_W; x++)
    for(int y = 0; y < GRID_H; y++)
    {
      tiles[x][y] = (rand()%2 ? TILE_ICE : TILE_LAVA);
    }

    // Load all the assets we need
    ASSERT(atlas.load("assets/atlas.png") == EXIT_SUCCESS, "Opening atlas texture");

    // All okay
    return 0;
  };

  // Init success
  return 0;
}

namespace ingame
{

  gamestate::t &get()
  {
    if(!initialised)
    {
      init();
      initialised = true;
    }

    return state;
  }

} // ingame

} // gamestate
