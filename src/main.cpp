//! --------------------------------------------------------------------------
//! -------------------------- INCLUDES
//! --------------------------------------------------------------------------

#include <cstdlib>
#include <SDL.h>
#include <ctime>

#include "debug/assert.h"

#include "graphics/opengl.h"
#include "graphics/Texture.hpp"

#include "global.hpp"

//! --------------------------------------------------------------------------
//! -------------------------- CONSTANTS
//! --------------------------------------------------------------------------

// --------------------------------------------------------------------------
// EVENTS
// --------------------------------------------------------------------------

#define EVENT_QUIT 0b00000001


//! --------------------------------------------------------------------------
//! -------------------------- WORKSPACE
//! --------------------------------------------------------------------------

static SDL_Window *window;

static Texture texture;

static float prev_tick, this_tick, next_tick;

//! --------------------------------------------------------------------------
//! -------------------------- GAME LOOP
//! --------------------------------------------------------------------------

int draw()
{
  // Clear and reset
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
  glMatrixMode(GL_MODELVIEW);

  // Flip the buffers to update the screen
  SDL_GL_SwapWindow(window);

  // All good
  return EXIT_SUCCESS;
}

// Regulate the number of frames per second, pausing only if need be
int wait()
{
  // Get the current time-stamp
  prev_tick = this_tick;
  this_tick = SDL_GetTicks();

  // If it's not yet time for the next update, wait a while
	if (this_tick < next_tick )
		SDL_Delay(next_tick - this_tick);

  // Calculate when the next update should be
	next_tick = this_tick + (1000/MAX_FPS);

  // All good
	return EXIT_SUCCESS;
}

char treatEvents()
{
  // static to avoid reallocating it ever time we run the function
  static SDL_Event event;

  // write each event to our static variable
  while (SDL_PollEvent(&event))
  {
    switch (event.type)
    {
      // exit if the window is closed (ex: pressing the cross at the top)
      case SDL_QUIT:
        return EVENT_QUIT;
      break;

      // check for keypresses
      case SDL_KEYDOWN:
        switch (event.key.keysym.sym)
        {
          case SDLK_ESCAPE:
            return EVENT_QUIT;
          default:
            break;
        }
      break;

      default:
        // not all possible inputs are needed, so we DO want a default break
      break;
    }
  }

  // no event
  return 0;
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
	window = SDL_CreateWindow("Rubika", SDL_WINDOWPOS_UNDEFINED,
                           SDL_WINDOWPOS_UNDEFINED, WINDOW_DEFAULT_W,
                           WINDOW_DEFAULT_H, SDL_WINDOW_OPENGL|SDL_WINDOW_SHOWN);
  ASSERT_SDL(window, "Opening SDL2.0 application window");

  // Since the window size can be overriden, check what it is actually
  SDL_GetWindowSize(window, &global::viewport.x, &global::viewport.y);

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

  // --------------------------------------------------------------------------
  // LOAD AN IMAGE
  // --------------------------------------------------------------------------

  texture.load("assets/eye_of_draining.png");

  // --------------------------------------------------------------------------
  // START THE GAME LOOP
  // --------------------------------------------------------------------------

  bool stop = false;
  do
  {
    // Redraw everything, game objects included
    draw();

    // Regulate the number of frames per second, pausing if need be
    wait();

    // Treat input events, check for exit conditions.
    char flags = treatEvents();
    if(flags)
      stop = true;
  }
  while(!stop);

  // --------------------------------------------------------------------------
  // SHUT DOWN
  // --------------------------------------------------------------------------

  // Report a clean exit!
	return EXIT_SUCCESS;
}
