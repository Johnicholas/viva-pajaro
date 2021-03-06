#include <assert.h>
#include <math.h>
#include <SDL/SDL.h>
#include <stdio.h>
#include <stdlib.h>

#define countof(X) (sizeof(X) / sizeof(X[0]))

int d(int max) {
  return rand()%max;
}

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 900
#define TOOL_ROW_START 215
#define TOOL_ROW_SPACING 135
#define TOOL_ROW_HEIGHT 850

struct Sprite {
  int key;
  int x;
  int y;
  int which;
  int color;
  // TODO: int layer;
  struct Sprite* next;
};

static struct Sprite* sprites;

void new_sprite(int key, int x, int y, int which, int layer) {
  printf("new sprite: %d %d %d %d %d\n", key, x, y, which, layer);
  struct Sprite* new_sprite = malloc(sizeof(struct Sprite));
  new_sprite->key = key;
  new_sprite->x = x;
  new_sprite->y = y;
  new_sprite->which = which;
  new_sprite->color = 0xffffff;
  // TODO: layer
  new_sprite->next = sprites;
  sprites = new_sprite;
}

void set_sprite_position(int key, int x, int y, int flipped) {
  struct Sprite* it;
  for (it = sprites; it; it = it->next) {
    if (it->key == key) {
      it->x = x;
      it->y = y;
      // TODO: flipped
      return;
    }
  }
}

void set_sprite_tint(int key, int color) {
  struct Sprite* it;
  for (it = sprites; it; it = it->next) {
    if (it->key == key) {
      it->color = color;
      return;
    }
  }
}

void set_sprite_rotation(int key, float rotation) {
  // TODO
}

void free_sprite(int key) {
  struct Sprite** it = &sprites;
  while (1) {
    if (*it == NULL) {
      return;
    } else if ((*it)->key == key) {
      struct Sprite* temp = (*it)->next;
      free(*it);
      *it = temp;
      return;
    }
    it = &((*it)->next);
  }
}

void set_count(int which, int new_value) {
  printf("You now have %d tier %d seeds.\n", new_value, which);
}

void print_debug(char* to_print) {
  printf("DEBUG: %s\n", to_print);
}

void set_muted(int new_muted) {
  if (new_muted) {
    printf("imagine we are now muted\n");
  } else {
    printf("imagine jungle sounds\n");
  }
}

// These three lines are part of the "Ceu invokes C" bindings.
#define ceu_out_emit_NEW_SPRITE(X) new_sprite(X->_1, X->_2, X->_3, X->_4, X->_5)
#define ceu_out_emit_SET_SPRITE_POSITION(X) set_sprite_position(X->_1, X->_2, X->_3, X->_4)
#define ceu_out_emit_SET_SPRITE_TINT(X) set_sprite_tint(X->_1, X->_2)
#define ceu_out_emit_SET_SPRITE_ROTATION(X) set_sprite_rotation(X->_1, X->_2)
#define ceu_out_emit_FREE_SPRITE(X) free_sprite(X->_1)
#define ceu_out_emit_SET_COUNT(X) set_count(X->_1, X->_2)
#define ceu_out_emit_PRINT_DEBUG(X) print_debug(X->_1)
#define ceu_out_emit_SET_MUTED(X) set_muted(X->_1)

#define ceu_out_assert(v) ceu_sys_assert(v)
void ceu_sys_assert (int v) {
  assert(v);
}

#define ceu_out_log(m,s) ceu_sys_log(m,s)
void ceu_sys_log (int mode, long s) {
  switch (mode) {
  case 0:
    printf("%s", (char*)s);
    break;
  case 1:
    printf("%lX", s);
    break;
  case 2:
    printf("%ld", s);
    break;
  }
}

typedef char* charptr;

// This is the C code generated by the Ceu compiler
#include "_ceu_app.c"

extern void ceu_app_init (tceu_app* app);

byte CEU_DATA[sizeof(CEU_Main)];
tceu_app app;

SDL_Surface *screen;

int corners_x[] = { 258, 512, 778, 1030, 111, 461, 828, 1175 };
int corners_y[] = { 485, 485, 485, 485, 800, 800, 800, 800 };

void draw_spot(int x, int y, int color) {
  static struct SDL_Rect rect;
  rect.x = x - 5;
  rect.y = y - 5;
  rect.w = 10;
  rect.h = 10;
  SDL_FillRect(screen, &rect, color);
}

void render() {
  struct Sprite* it;
  struct SDL_Rect rect;
  int i;

  rect.x = 0;
  rect.y = 0;
  rect.w = SCREEN_WIDTH;
  rect.h = SCREEN_HEIGHT;
  SDL_FillRect(screen, &rect, 0x999999);
  // Draw a trapezoid grid thingy?
  for (i = 0; i < countof(corners_x); i++ ) {
    draw_spot(corners_x[i], corners_y[i], 0x000000);
  }
  for (i = 0; i < 6; i++) {
    draw_spot(TOOL_ROW_START + TOOL_ROW_SPACING * (i + 0.5), TOOL_ROW_HEIGHT, 0x000000);
  }
  // Draw the sprites
  for (it = sprites; it; it = it->next) {
    draw_spot(it->x, it->y, it->color);
  }
}


void mainloop() {
  // Render stuff
  render();
  
  SDL_Flip(screen);  
  
  // Poll for events, and handle the ones we care about.
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
    case SDL_QUIT: exit(0); break;
    case SDL_MOUSEBUTTONDOWN:
      {
        tceu__int__int payload = { event.button.x, event.button.y };
        ceu_sys_go(&app, CEU_IN_MOUSE_DOWN, &payload);
      }
      break;
    case SDL_MOUSEBUTTONUP:
      {
        tceu__int__int payload = { event.button.x, event.button.y };
        ceu_sys_go(&app, CEU_IN_MOUSE_UP, &payload);
      }
      break;
    case SDL_MOUSEMOTION: 
      {
        tceu__int__int payload = { event.motion.x, event.motion.y };
        ceu_sys_go(&app, CEU_IN_MOUSE_MOVE, &payload);
      }
      break;
    }
  }

  ceu_sys_go(&app, CEU_IN_UPDATE, NULL);

  SDL_Delay( 1000 / 60 );
}

// Entry point
int main(int argc, char *argv[]) {
  app.data = (tceu_org*) &CEU_DATA;
  app.init = &ceu_app_init;
  app.init(&app);

  SDL_Init(SDL_INIT_VIDEO);
  atexit(SDL_Quit);
  screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 32, SDL_SWSURFACE);  
  if (screen == NULL) {
    exit(1);
  }
  while (1) {
    mainloop();
  }
  return 0;
}

