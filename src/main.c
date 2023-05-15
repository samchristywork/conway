#include <ncurses.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <command_line.h>

WINDOW *grid_win;

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

struct Renderer {
  void (*init_function)(struct World *, struct Vec2i);
  void (*draw_function)(struct World *);
  void (*cleanup_function)(struct World *);
};

void *world_init_ncurses(struct World *world, struct Vec2i world_dimensions) {
  initscr();
  noecho();
  curs_set(FALSE);
  grid_win = newwin(world_dimensions.y, world_dimensions.x, 0, 0);

  return NULL;
}

void *world_cleanup_ncurses(struct World *world) {
  delwin(grid_win);
  endwin();

  return NULL;
}

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
  struct Vec2i dimensions = world_get_dimensions(world);

  for (int y = 0; y < dimensions.y; y++) {
    for (int x = 0; x < dimensions.x; x++) {
      printf("%c ", world_get_square(world, x, y).value);
    }
    printf("\n");
  }
  printf("\n");
}

void world_print_ncurses(struct World *world) {
  struct Vec2i dimensions = world_get_dimensions(world);

  for (int y = 0; y < dimensions.y; y++) {
    for (int x = 0; x < dimensions.x; x++) {
      mvwaddch(grid_win, y, x, world_get_square(world, x, y).value);
    }
  }
  wrefresh(grid_win);
}

void world_display(struct World *world, struct Renderer *renderer) {
  renderer->draw_function(world);
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
  struct Vec2i dimensions = world_get_dimensions(world);

  struct Square world_copy[dimensions.y][dimensions.x];

  for (int y = 0; y < dimensions.y; y++) {
    for (int x = 0; x < dimensions.x; x++) {
      int num_neighbors = world_get_num_neighbors(world, x, y);

      if (world_get_square(world, x, y).value == CELL_DEAD) {
        if (num_neighbors == 3) {
          world_copy[y][x].value = CELL_LIVE;
        } else {
          world_copy[y][x].value = CELL_DEAD;
        }
      } else if (world_get_square(world, x, y).value == CELL_LIVE) {
        if (num_neighbors < 2) {
          world_copy[y][x].value = CELL_DEAD;
        } else if (num_neighbors == 2 || num_neighbors == 3) {
          world_copy[y][x].value = CELL_LIVE;
        } else if (num_neighbors > 3) {
          world_copy[y][x].value = CELL_DEAD;
        } else {
          world_copy[y][x].value = CELL_DEAD;
        }
      }
    }
  }

  for (int y = 0; y < dimensions.y; y++) {
    for (int x = 0; x < dimensions.x; x++) {
      world_set_square(world, x, y, world_copy[y][x].value);
    }
  }
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
  struct Vec2i dimensions = world_get_dimensions(world);

  for (int y = 0; y < dimensions.y; y++) {
    for (int x = 0; x < dimensions.x; x++) {
      if (random() / (float)RAND_MAX > percent) {
        world_set_square(world, x, y, CELL_DEAD);
      } else if (random() / (float)RAND_MAX > percent) {
        world_set_square(world, x, y, CELL_LIVE);
      }
    }
  }
}

int main(int argc, char *argv[]) {
  add_arg('l', "loop", "Simulation should restart when complete.");
  add_arg('x', "width", "The width of the world.");
  add_arg('y', "height", "The height of the world.");

  parse_opts(argc, argv);

  bool loop = get_is_set('l');

  struct Vec2i world_dimensions = {-1, -1};
  if (get_is_set('x')) {
    world_dimensions.x = atoi(get_value('x'));
  }
  if (get_is_set('y')) {
    world_dimensions.y = atoi(get_value('y'));
  }

  if (world_dimensions.x <= 0) {
    world_dimensions.x = 30;
  }
  if (world_dimensions.y <= 0) {
    world_dimensions.y = 30;
  }

  struct World world;
  world_init(&world, world_dimensions);

  world_random_seed(&world, .5);

  struct Renderer *renderer = malloc(sizeof(struct Renderer));
  bzero(renderer, sizeof(struct Renderer));

  renderer->draw_function = &world_print_ncurses;
  renderer->init_function = &world_init_ncurses;
  renderer->cleanup_function = &world_cleanup_ncurses;

  if (renderer->init_function != NULL) {
    renderer->init_function(&world, world_dimensions);
  }

  while (true) {
    world_display(&world, renderer);
    world_simulate_step(&world);
    usleep(1000 * 100);
  }

  if (renderer->cleanup_function != NULL) {
    renderer->cleanup_function(&world);
  }
}
