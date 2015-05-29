//! --------------------------------------------------------------------------
//! -------------------------- INCLUDES
//! --------------------------------------------------------------------------

#include <cstdlib>
#include <SDL.h>
#include <ctime>

#include "debug/assert.h"

#include "graphics/opengl.h"
#include "graphics/Texture.hpp"

#include "math/wjd_math.h"

#include "global.hpp"

//! --------------------------------------------------------------------------
//! -------------------------- CONSTANTS
//! --------------------------------------------------------------------------

// --------------------------------------------------------------------------
// GRID
// --------------------------------------------------------------------------

#define GRID_W 50
#define GRID_H 50
#define TILE_LAVA 1
#define TILE_ICE 2

// --------------------------------------------------------------------------
// EVENTS
// --------------------------------------------------------------------------

#define EVENT_QUIT 0b00000001

//! --------------------------------------------------------------------------
//! -------------------------- WORKSPACE
//! --------------------------------------------------------------------------

static SDL_Window *window;

static Texture atlas, spaceship;

static int tiles[GRID_W][GRID_H];

static fRect lava(0, 0, 32, 32),
  ice(32, 0, 32, 32),
  sprite(0, 0, 128, 128);

static float spaceship_x = 0.0f;
static float spaceship_y = 0.0f;
static float spaceship_dx = 0.0f;
static float spaceship_dy = -1.0f;

static float c = cos(PI*0.01);
static float s = sin(PI*0.01);

static float mouse_x = 0.0f, mouse_y = 0.0f;

//! --------------------------------------------------------------------------
//! -------------------------- GAME LOOP
//! --------------------------------------------------------------------------

int treatEvents()
{
  // Static to avoid reallocating it ever time we run the function
  static SDL_Event event;

  // Write each APP_NAMEevent to our static variable
  while (SDL_PollEvent(&event))
  {
    switch (event.type)
    {
      // Exit if the window is closed (ex: pressing the cross at the top)
      case SDL_QUIT:
        return EVENT_QUIT;
      break;

      // Check for key-presses
      case SDL_KEYDOWN:
        switch (event.key.keysym.sym)
        {
          case SDLK_ESCAPE:
            return EVENT_QUIT;
          default:
            break;
        }
      break;

      case SDL_MOUSEMOTION:
        mouse_x = event.motion.x;
        mouse_y = event.motion.y;
      break;

      default:
        // not all possible inputs are needed, so we DO want a default break
      break;
    }
  }

  // No event
  return 0;
}

int update(float dt)
{
  // Cap delta-time
  if(dt > MAX_DT)
    dt = MAX_DT;

  // Turn the ship
  float vx = mouse_x - spaceship_x;
  float vy = mouse_y - spaceship_y;

  float norm_v = sqrt(vx*vx + vy*vy);
  float nvx = vx/norm_v;
  float nvy = vy/norm_v;

  // Do we need to turn
  float dot = spaceship_dx*nvx + spaceship_dy*nvy;
  if(dot < 0.99)
  {
    // Which direction shall we turn in?
    float det = spaceship_dx*vy - spaceship_dy*vx;
    float ss = det < 0 ? -s : s;

    spaceship_dx = spaceship_dx*c - spaceship_dy*ss;
    spaceship_dy = spaceship_dx*ss + spaceship_dy*c;

    // Normally we shouldn't need to do this!!!
    float norm = sqrt(spaceship_dx*spaceship_dx + spaceship_dy*spaceship_dy);
    spaceship_dx /= norm;
    spaceship_dy /= norm;
  }

  // Move the spaceship to the its model position
  sprite.x = spaceship_x - sprite.w*0.5f;
  sprite.y = spaceship_y - sprite.h*0.5f;

  // Treat input events
  return treatEvents();
}

int draw()
{
  // Clear and reset
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
  glMatrixMode(GL_MODELVIEW);

  // Draw the grid
/*
  for(int x = 0; x < GRID_W; x++)
  for(int y = 0; y < GRID_H; y++)
  {
    static fRect tile(0, 0, 32, 32);

    tile.x = tile.w*x;
    tile.y = tile.h*y;
    switch(tiles[x][y])
    {
      case TILE_ICE:
        atlas.draw(&ice, &tile);
        break;

      case TILE_LAVA:
        atlas.draw(&lava, &tile);
        break;
    }
  }
*/
  // Draw the sprite
  spaceship.draw(nullptr, &sprite);

  // Debug
  float x = spaceship_x + spaceship_dx*128;
  float y = spaceship_y + spaceship_dy*128;
  fRect little(0, 0, 16, 16);
  for(float t = 0.0f; t < 1.0f; t += 0.1f)
  {
    // Draw the spaceship's direction
    little.x = (1 - t)*spaceship_x + t*x - 8;
    little.y = (1 - t)*spaceship_y + t*y - 8;
    spaceship.draw(nullptr, &little);

    // Draw the mouse direction
    little.x = (1 - t)*spaceship_x + t*mouse_x - 8;
    little.y = (1 - t)*spaceship_y + t*mouse_y - 8;
    spaceship.draw(nullptr, &little);
  }

  // Flip the buffers to update the screen
  SDL_GL_SwapWindow(window);

  // All good
  return EXIT_SUCCESS;
}

//! --------------------------------------------------------------------------
//! -------------------------- STARTUP
//! --------------------------------------------------------------------------

// Main must have exactly this signature or SDL2 will be sad
int main(int argc, char *argv[])
{
  // Initialise random numbers
  srand(time(NULL));

  // --------------------------------------------------------------------------
  // START SDL
  // --------------------------------------------------------------------------

  // Set up SDL (create window and context for OpenGL)
	window = SDL_CreateWindow(APP_NAME, SDL_WINDOWPOS_UNDEFINED,
                           SDL_WINDOWPOS_UNDEFINED, WINDOW_DEFAULT_W,
                           WINDOW_DEFAULT_H, SDL_WINDOW_OPENGL|SDL_WINDOW_SHOWN);
  ASSERT_SDL(window, "Opening SDL2.0 application window");

  // Since the window size can be overriden, check what it is actually
  SDL_GetWindowSize(window, &global::viewport.x, &global::viewport.y);
  global::scale.x = global::scale.y = 1.0f;

  // Create the OpenGL context for the window we just opened
  auto context = SDL_GL_CreateContext(window);
  SDL_GL_MakeCurrent(window, context);

  // Configure SDL/OpenGL interface
  ASSERT_SDL(SDL_GL_SetSwapInterval(1) != -1, "Activating SDL V-sync");
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, GL_V_MAJOR);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, GL_V_MINOR);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

  // --------------------------------------------------------------------------
  // START OPENGL
  // --------------------------------------------------------------------------

  {

  // Define viewport
  glViewport(0, 0, WINDOW_DEFAULT_W, WINDOW_DEFAULT_H);

  // Black background by default
  glClearColor(0, 0, 0, 255);

  // Texturing
  glEnable(GL_TEXTURE_2D);

  // Blending and anti-aliasing
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

  // Disable depth-testing
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);

	// Disable lighting
	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);
	glDisable(GL_LIGHT1);

  // Set up viewport
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, WINDOW_DEFAULT_W, WINDOW_DEFAULT_H, 0, -1, 1);

  // Clean the slate
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  } // start opengl

  // --------------------------------------------------------------------------
  // LOAD THE IMAGES
  // --------------------------------------------------------------------------

  ASSERT(atlas.load("assets/atlas.png") == EXIT_SUCCESS, "Opening atlas texture");
  ASSERT(spaceship.load("assets/medspeedster.png") == EXIT_SUCCESS, "Opening spaceship texture");

  // --------------------------------------------------------------------------
  // INITIALISE THE GRID
  // --------------------------------------------------------------------------

  for(int x = 0; x < GRID_W; x++)
  for(int y = 0; y < GRID_H; y++)
  {
    tiles[x][y] = (rand()%2 ? TILE_ICE : TILE_LAVA);
  }

  // --------------------------------------------------------------------------
  // INITIALISE THE GAME OBJECTS
  // --------------------------------------------------------------------------

  spaceship_x = global::viewport.x*0.5f;
  spaceship_y = global::viewport.y*0.5f;

  // --------------------------------------------------------------------------
  // START THE GAME LOOP
  // --------------------------------------------------------------------------

  {

  float prev_tick, this_tick = SDL_GetTicks();
  bool stop = false;
  do
  {
    // Get the current time-stamp
    prev_tick = this_tick;
    this_tick = SDL_GetTicks();

    // Update, check for exit events
    stop = (update((this_tick - prev_tick)/1000.0f) & EVENT_QUIT);

    // Redraw everything, game objects included
    draw();
  }
  while(!stop);

  } // game loop

  // --------------------------------------------------------------------------
  // SHUT DOWN
  // --------------------------------------------------------------------------

  // Destroy context
  SDL_GL_MakeCurrent(NULL, NULL);
  SDL_GL_DeleteContext(context);
  SDL_DestroyWindow(window);

  // Shut down SDL
	SDL_Quit();

  // Report a clean exit!
	return EXIT_SUCCESS;
}
