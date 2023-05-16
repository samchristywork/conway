#ifndef MAIN_H
#define MAIN_H

#include <curses.h>

enum {
  CELL_LIVE = 'o',
  CELL_DEAD = '.',
};

struct Vec2i {
  int x;
  int y;
};

struct Square {
  int value;
};

struct World {
  struct Vec2i _dimensions;
  struct Square **grid;
};

struct Context {
  WINDOW *grid_win;
};

struct Renderer {
  int (*init_function)(struct Context *, struct World *, struct Vec2i);
  void (*draw_function)(struct Context *, struct World *);
  void (*cleanup_function)(struct Context *, struct World *);
  int (*event_handler_function)();
};

struct Vec2i world_get_dimensions(struct World *world);

struct Square world_get_square(struct World *world, int x, int y);

int world_get_num_neighbors(struct World *world, int x, int y);

#endif