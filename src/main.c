#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <command_line.h>

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

bool world_init(struct World *world, struct Vec2i dimensions) {
}

struct Vec2i world_get_dimensions(struct World *world) {
}

struct Square world_get_square(struct World *world, int x, int y) {
}

void world_print(struct World *world) {
}

bool world_set_square(struct World *world, int x, int y, int value) {
}

int world_get_num_neighbors(struct World *world, int x, int y) {
}

void world_simulate_step(struct World *world) {
}

void world_print_num_neighbors(struct World *world) {
}

void world_random_seed(struct World *world, float percent) {
}

int main(int argc, char *argv[]) {
  struct Vec2i world_dimensions = {-1, -1};

  if (world_dimensions.x <= 0) {
    world_dimensions.x = 30;
  }
  if (world_dimensions.y <= 0) {
    world_dimensions.y = 30;
  }

  struct World world;
  world_init(&world, world_dimensions);

  world_random_seed(&world, .5);

  while (true) {
    world_print(&world);
    world_simulate_step(&world);
    usleep(1000 * 100);
  }
}
