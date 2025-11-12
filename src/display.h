#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdbool.h>
#include <stdint.h>
#include "SDL2/SDL.h"

//////////////////////////////////////////////////
// Constants for maintaining a stable framerate //
//////////////////////////////////////////////////
#define FPS 60
#define FRAME_TARGET_TIME (1000 / FPS)

///////////////////////
// Window properties //
///////////////////////
extern int window_width;
extern int window_height;
extern SDL_Window* window;                  // the window itself
extern SDL_Renderer* renderer;              // the renderer object
extern uint32_t* color_buffer;              // a buffer to store the color value of every pixel on the screen
extern float* z_buffer;                  // a buffer to store the depth of each pixel on the screen
extern SDL_Texture* color_buffer_texture;   // an SDL texrure used to display the color buffer on the screen

//////////////////////
// Window functions //
//////////////////////
bool initialize_window(void);
void destroy_window(void);

/////////////////////////
// Rendering functions //
/////////////////////////
void render_color_buffer(void);
void clear_color_buffer(uint32_t color);
void clear_z_buffer(void);

///////////////////////
// Drawing functions //
///////////////////////
void draw_pixel(int x, int y, uint32_t color);
void draw_line(int x0, int y0, int x1, int y1, uint32_t color);
void draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color);
void draw_rectangle(int x, int y, int w, int h, uint32_t color);

#endif