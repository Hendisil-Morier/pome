#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include "editor_datatype.h"
#include "termbox2.h"
#include "error.h"
#include "c_to_lua.h"

Editor* get_editor(lua_State *lua);

void init_lua(Editor* editor)
{
  lua_State *lua = luaL_newstate();
  if (lua == nullptr)
  {
    fprintf(stderr, "Lua: cannot initialize.\r\n");
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
    fprintf(stderr, "Lua load config error: %s\r\n", lua_tostring(lua, -1));
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

  lua_set_intfield(lua, TB_KEY_ESC, "esc");

  lua_set_intfield(lua, TB_KEY_CTRL_R, "ctrl_r");
  lua_set_intfield(lua, TB_KEY_CTRL_Q, "ctrl_q");

  //printable character
  //lowercase
  for (char c = 'a'; c <= 'z'; c++)
  {
    char name[2] = {c, '\0'};
    lua_set_intfield(lua, c, name);
  }

  // uppercase
  for (char c = 'A'; c <= 'Z'; c++)
  {
    char name[2] = {c, '\0'};
    lua_set_intfield(lua, c, name);
  }

  lua_setglobal(lua, "key_press");
}

void register_primitives(lua_State *lua)
{
  lua_register(lua, "move_cursor", lua_move_cursor);
  lua_register(lua, "move_cursor_to", lua_move_cursor_to);
  lua_register(lua, "quit_editor", lua_quit_editor);

  lua_register(lua, "insert_char", lua_insert_char);

  lua_register(lua, "set_mode", lua_set_mode);
  lua_register(lua, "save_mode", lua_save_mode);

  lua_register(lua, "get_mode", lua_get_mode);
  lua_register(lua, "get_saved_mode", lua_get_saved_mode);
  lua_register(lua, "restore_mode", lua_restore_mode);

  lua_register(lua, "is_minor_mode", lua_is_minor_mode);

  lua_register(lua, "insert_newlines", lua_insert_newline);

  lua_register(lua, "delete_after_cursor", lua_delete_after_cursor);
  lua_register(lua, "delete_before_cursor", lua_delete_before_cursor);

  lua_register(lua, "call_mode_hook", lua_call_mode_hook);

  lua_register(lua, "set_anchor", lua_set_anchor);
  lua_register(lua, "clear_anchor", lua_clear_anchor);

  lua_register(lua, "delete_selected", lua_delete_selected);

  lua_register(lua, "load_config", lua_reload_config);
}

Editor* get_editor(lua_State *lua)
{
  lua_getfield(lua, LUA_REGISTRYINDEX, "editor");
  Editor *editor = lua_touserdata(lua, -1);
  if (editor == nullptr)
  {
    fprintf(stderr, "Sumting go rong.\r\n");
    return nullptr;
  }

  lua_pop(lua, 1);
  return editor;
}
