#include <lualib.h>
#include <lauxlib.h>
#include "editor_datatype.h"
#include "termbox2.h"
#include "error.h"
#include "gap_buffer.h"
#include "editor.h"

Editor* get_editor(lua_State *lua);

void init_lua(Editor* editor)
{
  lua_State *lua = luaL_newstate();
  if (lua == nullptr)
  {
    fprintf(stderr, "Lua: cannot initialize");
    return;
  }

  editor->lua = lua;
  luaL_openlibs(lua);

  lua_pushlightuserdata(lua, editor);
  lua_setfield(lua, LUA_REGISTRYINDEX, "editor");
}

Status lua_load_config(lua_State *lua, const char* filename)
{
  if (luaL_dofile(lua, filename) != LUA_OK)
  {
    fprintf(stderr, "Lua error: %s\n", lua_tostring(lua, -1));
    lua_pop(lua, 1);
    return FAILURE;
  }
  return SUCCESS;
}

void lua_set_intfield(lua_State* lua, int value, const char* name)
{
  lua_pushinteger(lua, value);
  lua_setfield(lua, -2, name);
}

void register_constant(lua_State *lua)
{
  //directions
  lua_newtable(lua);

  lua_set_intfield(lua, DIR_RIGHT, "right");
  lua_set_intfield(lua, DIR_LEFT, "left");
  lua_set_intfield(lua, DIR_UP, "up");
  lua_set_intfield(lua, DIR_DOWN, "down");

  lua_setglobal(lua, "direction");

  //##keymap
  lua_newtable(lua);

  //arrow keys
  lua_set_intfield(lua, TB_KEY_ARROW_LEFT, "arrow_left");
  lua_set_intfield(lua, TB_KEY_ARROW_RIGHT, "arrow_right");
  lua_set_intfield(lua, TB_KEY_ARROW_UP, "arrow_up");
  lua_set_intfield(lua, TB_KEY_ARROW_DOWN, "arrow_down");

  //misc keys
  lua_set_intfield(lua, TB_KEY_BACKSPACE, "backspace");
  lua_set_intfield(lua, TB_KEY_BACKSPACE2, "backspace2");
  lua_set_intfield(lua, TB_KEY_DELETE, "delete");
  lua_set_intfield(lua, TB_KEY_ENTER, "enter");

  lua_set_intfield(lua, TB_KEY_CTRL_S, "ctrl_s");
  lua_set_intfield(lua, TB_KEY_CTRL_Q, "ctrl_q");

  lua_setglobal(lua, "key_press");
}

int lua_move_cursor(lua_State* lua)
{
  Editor* editor = get_editor(lua);

  int direction = luaL_checkinteger(lua, 1);
  int times = luaL_checkinteger(lua, 2);

  Direction dirs[] = {RIGHT, LEFT, UP, DOWN};
  if (direction < 0 || direction >= 4) return 0;

  move_gap(editor->buffer, (size_t)times, dirs[direction]);
  return 0;
}

int lua_quit_editor(lua_State* lua)
{
  Editor *editor = get_editor(lua);
  quit_editor(editor);
  return 0;
}

void register_primitives(lua_State *lua)
{
  lua_register(lua, "move_cursor", lua_move_cursor);
  lua_register(lua, "quit_editor", lua_quit_editor);
}

Editor* get_editor(lua_State *lua)
{
  lua_getfield(lua, LUA_REGISTRYINDEX, "editor");
  Editor *editor = lua_touserdata(lua, -1);
  if (editor == nullptr)
  {
    fprintf(stderr, "Sumting go rong.");
    return nullptr;
  }

  lua_pop(lua, 1);
  return editor;
}
