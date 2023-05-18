#include <ncurses.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <command_line.h>
#include <main.h>
#include <render.h>

struct Context ctx;

int world_init(struct World *world, struct Vec2i dimensions) {
  world->_dimensions.x = dimensions.x;
  world->_dimensions.y = dimensions.y;

  world->grid =
      (struct Square **)malloc(sizeof(struct Square *) * dimensions.y);
  if (!world->grid) {
    return FALSE;
  }

  for (int y = 0; y < dimensions.y; y++) {
    world->grid[y] =
        (struct Square *)malloc(sizeof(struct Square) * dimensions.x);
    if (!world->grid[y]) {
      return FALSE;
    }

    for (int x = 0; x < dimensions.x; x++) {
      world->grid[y][x].value = ctx.cell_dead;
    }
  }

  return TRUE;
}

struct Vec2i world_get_dimensions(struct World *world) {
  return world->_dimensions;
}

struct Square world_get_square(struct World *world, int x, int y) {
  struct Vec2i dimensions = world_get_dimensions(world);

  struct Square empty;
  empty.value = ctx.cell_dead;

  if (x < 0 || x >= dimensions.x) {
    return empty;
  }

  if (y < 0 || y >= dimensions.y) {
    return empty;
  }

  return world->grid[y][x];
}

void world_display(struct World *world, struct Renderer *renderer) {
  renderer->draw_function(&ctx, world);
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
        if (world_get_square(world, x + rx, y + ry).value == ctx.cell_alive) {
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

      if (world_get_square(world, x, y).value == ctx.cell_dead) {
        if (num_neighbors == 3) {
          world_copy[y][x].value = ctx.cell_alive;
        } else {
          world_copy[y][x].value = ctx.cell_dead;
        }
      } else if (world_get_square(world, x, y).value == ctx.cell_alive) {
        if (num_neighbors < 2) {
          world_copy[y][x].value = ctx.cell_dead;
        } else if (num_neighbors == 2 || num_neighbors == 3) {
          world_copy[y][x].value = ctx.cell_alive;
        } else if (num_neighbors > 3) {
          world_copy[y][x].value = ctx.cell_dead;
        } else {
          world_copy[y][x].value = ctx.cell_dead;
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

void world_random_seed(struct World *world, float percent) {
  struct Vec2i dimensions = world_get_dimensions(world);

  for (int y = 0; y < dimensions.y; y++) {
    for (int x = 0; x < dimensions.x; x++) {
      if (random() / (float)RAND_MAX > percent) {
        world_set_square(world, x, y, ctx.cell_dead);
      } else if (random() / (float)RAND_MAX > percent) {
        world_set_square(world, x, y, ctx.cell_alive);
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
  renderer->event_handler_function = &event_handler_ncurses;

  if (renderer->init_function != NULL) {
    if (renderer->init_function(&ctx, &world, world_dimensions) == FALSE) {
      fprintf(stderr, "Failed to initialize renderer.\n");
      exit(EXIT_FAILURE);
    }
  }

  while (true) {
    if (renderer->event_handler_function != NULL) {
      if (renderer->event_handler_function() == FALSE) {
        break;
      }
    }

    world_display(&world, renderer);
    world_simulate_step(&world);
    usleep(1000 * 100);
  }

  if (renderer->cleanup_function != NULL) {
    renderer->cleanup_function(&ctx, &world);
  }
}
