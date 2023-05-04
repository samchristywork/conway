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
  world->_dimensions.x = dimensions.x;
  world->_dimensions.y = dimensions.y;

  world->grid =
      (struct Square **)malloc(sizeof(struct Square *) * dimensions.y);
  if (!world->grid) {
    return false;
  }

  for (int y = 0; y < dimensions.y; y++) {
    world->grid[y] =
        (struct Square *)malloc(sizeof(struct Square) * dimensions.x);
    if (!world->grid[y]) {
      return false;
    }

    for (int x = 0; x < dimensions.x; x++) {
      world->grid[y][x].value = CELL_DEAD;
    }
  }

  return true;
}

struct Vec2i world_get_dimensions(struct World *world) {
  return world->_dimensions;
}

struct Square world_get_square(struct World *world, int x, int y) {
  struct Vec2i dimensions = world_get_dimensions(world);

  struct Square empty;
  empty.value = CELL_DEAD;

  if (x < 0 || x >= dimensions.x) {
    return empty;
  }

  if (y < 0 || y >= dimensions.y) {
    return empty;
  }

  return world->grid[y][x];
}

void world_print(struct World *world) {
}

bool world_set_square(struct World *world, int x, int y, int value) {
  struct Vec2i dimensions = world_get_dimensions(world);

  if (x < 0 || x >= dimensions.x) {
    return false;
  }

  if (y < 0 || y >= dimensions.y) {
    return false;
  }

  world->grid[y][x].value = value;
  return true;
}

int world_get_num_neighbors(struct World *world, int x, int y) {
  int sum = 0;
  for (int ry = -1; ry <= 1; ry++) {
    for (int rx = -1; rx <= 1; rx++) {
      if (rx || ry) {
        if (world_get_square(world, x + rx, y + ry).value == CELL_LIVE) {
          sum++;
        }
      }
    }
  }
  return sum;
}

void world_simulate_step(struct World *world) {
}

void world_print_num_neighbors(struct World *world) {
  struct Vec2i dimensions = world_get_dimensions(world);

  for (int y = 0; y < dimensions.y; y++) {
    for (int x = 0; x < dimensions.x; x++) {
      printf("%d", world_get_num_neighbors(world, x, y));
    }
    printf("\n");
  }
  printf("\n");
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
