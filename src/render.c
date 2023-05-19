#include <ncurses.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <main.h>
#include <render.h>

int world_init_ncurses(struct Context *ctx, struct World *world,
                       struct Vec2i world_dimensions) {
  initscr();
  cbreak();
  curs_set(FALSE);
  nodelay(stdscr, TRUE);
  noecho();
  scrollok(stdscr, TRUE);

  ctx->grid_win = newwin(world_dimensions.y + 3, world_dimensions.x + 2, 0, 0);
  if (ctx->grid_win == NULL) {
    return FALSE;
  }

  return TRUE;
}

void world_cleanup_ncurses(struct Context *ctx, struct World *world) {
  delwin(ctx->grid_win);
  endwin();
}

int event_handler_ncurses() {
  int ch = getch();

  if (ch != ERR) {
    return FALSE;
  }

  return TRUE;
}

void world_print_ncurses(struct Context *ctx, struct World *world) {

  struct Vec2i dimensions = world_get_dimensions(world);

  mvwhline(ctx->grid_win, 0, 0, 0, dimensions.x + 1);
  mvwhline(ctx->grid_win, dimensions.y + 1, 0, 0, dimensions.x + 1);
  mvwvline(ctx->grid_win, 0, 0, 0, dimensions.y + 1);
  mvwvline(ctx->grid_win, 0, dimensions.x + 1, 0, dimensions.y + 1);
  mvwaddch(ctx->grid_win, 0, 0, ACS_ULCORNER);
  mvwaddch(ctx->grid_win, dimensions.y + 1, 0, ACS_LLCORNER);
  mvwaddch(ctx->grid_win, 0, dimensions.x + 1, ACS_URCORNER);
  mvwaddch(ctx->grid_win, dimensions.y + 1, dimensions.x + 1, ACS_LRCORNER);

  static int frame = 0;
  char buf[100];
  sprintf(buf, "Frame: %d", frame);
  mvwaddstr(ctx->grid_win, dimensions.y + 2, 1, buf);
  frame++;

  for (int y = 0; y < dimensions.y; y++) {
    for (int x = 0; x < dimensions.x; x++) {
      mvwaddch(ctx->grid_win, y + 1, x + 1,
               world_get_square(world, x, y).value);
    }
  }

  wrefresh(ctx->grid_win);
}

void world_print_term(struct Context *ctx, struct World *world) {
  struct Vec2i dimensions = world_get_dimensions(world);

  for (int y = 0; y < dimensions.y; y++) {
    for (int x = 0; x < dimensions.x; x++) {
      printf("%c ", world_get_square(world, x, y).value);
    }
    printf("\n");
  }
  printf("\n");
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
