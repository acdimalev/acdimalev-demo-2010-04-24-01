#include "SDL.h"
#include <cairo.h>

#include <stdlib.h>

int fps = 30;
int width  = 320;
int height = 240;
float aspect = 320/240.0;

int rps = 4;

int main(int argc, char **argv) {
  SDL_Surface *sdl_surface;
  cairo_t *cr;

  Uint32 next_frame, next_rand;

  int running;

  float drop[16], drop_vel[16];

  /* Initialize SDL */
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
  SDL_ShowCursor(0);
  SDL_SetVideoMode(width, height, 32, 0);
  sdl_surface = SDL_GetVideoSurface();

  { /* Initialize Cairo Canvas */
    cairo_surface_t *surface;
    surface = cairo_image_surface_create_for_data(
      sdl_surface->pixels,
      CAIRO_FORMAT_RGB24,
      sdl_surface->w,
      sdl_surface->h,
      sdl_surface->pitch
      );
    cr = cairo_create(surface);
    // Reduce surface refcount.
    // Surface will be freed when canvas is destroyed.
    cairo_surface_destroy(surface);
  }

  // Cartesian
  cairo_translate(cr, width/2.0, height/2.0);
  cairo_scale(cr, 1, -1);

  // scale -- 4:3 :: 16:12
  cairo_scale(cr, height / 12, height / 12);

  /* Initialize Delay */
  next_frame = 1024.0 / fps;

  /* Game Logic */
  running = 1;

  {
    int i;

    for (i = 0; i < 16; i = i + 1) {
      drop[i] = -6 - 3;
      drop_vel[i] = -rand() % 3 - 1;
    }

    next_rand = 1024.0 / rps;
  }

  while (running) {

    /* Render Frame */
    cairo_set_operator(cr, CAIRO_OPERATOR_CLEAR);
    cairo_paint(cr);
    cairo_set_operator(cr, CAIRO_OPERATOR_OVER);

    {
      int i;
      for (i = 0; i < 16; i = i + 1) {
        cairo_move_to(cr, i - 8, drop[i] + 3);
        cairo_line_to(cr, i - 7, drop[i] + 3);
        cairo_line_to(cr, i - 7, drop[i]);
        cairo_line_to(cr, i - 8, drop[i]);
        cairo_close_path(cr);
      }
      cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
      cairo_fill(cr);
    }

    /* Update Display */
    SDL_UnlockSurface(sdl_surface);
    SDL_Flip(sdl_surface);
    SDL_LockSurface(sdl_surface);

    { /* Delay */
      Uint32 now;
      now = SDL_GetTicks();
      if (now < next_frame) {
        SDL_Delay(next_frame - now);
      }
      next_frame = next_frame + 1024.0 / fps;
    }

    { /* Game Logic */
      Uint8 *keystate;
      int i;
      Uint32 now;

      SDL_PumpEvents();
      keystate = SDL_GetKeyState(NULL);
      if (keystate[SDLK_q]) {
        running = 0;
      }

      for (i = 0; i < 16; i = i + 1) {
        if (drop[i] > -6 - 3) {
          drop[i] = drop[i] + drop_vel[i] / fps;
        }
      }

      now = SDL_GetTicks();
      if (now > next_rand) {
        for (i = 0; i < 16; i = i + 1) {
          if (drop[i] > -6 - 3) {
            drop_vel[i] = -rand() % 3 - 1;
          } else {
            if (rand() % 32 == 0) {
              drop[i] = 6;
            }
          }
        }
        next_rand = next_rand + 1024.0 / rps;
      }
    }

  }
  SDL_UnlockSurface(sdl_surface);

  cairo_destroy(cr);
  SDL_Quit();

  return 0;
}
