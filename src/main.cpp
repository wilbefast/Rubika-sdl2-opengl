//! --------------------------------------------------------------------------
//! -------------------------- INCLUDES
//! --------------------------------------------------------------------------

#include <cstdlib>
#include <ctime>
#include <SDL.h>

#include "debug/assert.h"

#include "graphics/opengl.h"
#include "graphics/Texture.hpp"

#include "math/wjd_math.h"

#include "gamestates/gamestate.h"
#include "gamestates/title.h"

#include "global.hpp"

#include "babysitter.h"

//! --------------------------------------------------------------------------
//! -------------------------- NAMESPACES
//! --------------------------------------------------------------------------

using namespace std;

//! --------------------------------------------------------------------------
//! -------------------------- WORKSPACE
//! --------------------------------------------------------------------------

static SDL_Window *window = nullptr;

//! --------------------------------------------------------------------------
//! -------------------------- GAME LOOP
//! --------------------------------------------------------------------------

int update(float dt)
{
  // Cap delta-time
  if(dt > MAX_DT)
    dt = MAX_DT;

  // Update, accumulate event flags
  int flags = gamestate::update(dt);

  // Babysitter
  flags |= babysitter::update(dt);

  // Treat input events
  static SDL_Event event;
  while (SDL_PollEvent(&event))
    flags |= gamestate::treatEvent(event);


  // Returns flags
  return flags;
}

int draw()
{
  // Clear and reset
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
  glMatrixMode(GL_MODELVIEW);

  // Draw the current state
  gamestate::draw();

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
  global::viewport.x = WINDOW_DEFAULT_W;
  global::viewport.y = WINDOW_DEFAULT_H;

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
  glViewport(0, 0, global::viewport.x, global::viewport.y);

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
  // SET UP GAMESTATES
  // --------------------------------------------------------------------------

  gamestate::switchTo(gamestate::title::get());

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
