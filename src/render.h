#ifndef RENDER_H
#define RENDER_H

#include <main.h>

int world_init_ncurses(struct Context *ctx, struct World *world, struct Vec2i world_dimensions);

void world_cleanup_ncurses(struct Context *ctx, struct World *world);

int event_handler_ncurses();

void world_print_ncurses(struct Context *ctx, struct World *world);

void world_print(struct World *world);

void world_print_num_neighbors(struct World *world);

#endif
