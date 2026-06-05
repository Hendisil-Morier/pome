#pragma once
#include <stddef.h>
#include <lua.h>

typedef struct
{
  enum
  {
    DIR_RIGHT,
    DIR_LEFT,
    DIR_UP,
    DIR_DOWN,
  } direction;
} Direction;

static constexpr Direction RIGHT = {.direction = DIR_RIGHT};
static constexpr Direction LEFT = {.direction = DIR_LEFT};
static constexpr Direction UP = {.direction = DIR_UP};
static constexpr Direction DOWN = {.direction = DIR_DOWN};

typedef struct gap_buffer gap_buffer;
struct gap_buffer
{
  size_t capacity;
  size_t gap_start;
  size_t gap_end;
  char *buf;
};

typedef struct
{
  size_t x, y;
} Position;

typedef struct
{
  size_t width, height;
} terminal_state;

typedef struct
{
  Position pos;
  size_t anchor;
  bool selecting;
} cursor_state;

typedef struct
{
  const char* current_mode;
  const char* pre_mode;
} mode_info;

typedef struct
{
  const char* filename;
  lua_State *lua;

  gap_buffer* buffer;
  terminal_state term_stat;

  mode_info modeinfo;
  cursor_state cursor;

  size_t row_offset;
  bool running; //gooning
} Editor;
