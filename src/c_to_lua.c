#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include "gap_buffer.h"
#include "lua_vm.h"
#include "editor.h"

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

int lua_move_cursor_to(lua_State* lua)
{
  if (lua == nullptr) return 0;
  Editor *editor = get_editor(lua);
  if (editor == nullptr) return 0;

  int x = luaL_checkinteger(lua, 1);
  int y = luaL_checkinteger(lua, 2);

  if (x < 0 || y < 0) return 0;

  size_t line_start = get_line_start(editor->buffer, y);
  size_t abs_pos = line_start + x;

  move_gap_to(editor->buffer, abs_pos);
  return 0;
}

int lua_quit_editor(lua_State* lua)
{
  if (lua == nullptr) return 0;
  Editor *editor = get_editor(lua);
  if (editor == nullptr) return 0;

  quit_editor(editor);
  return 0;
}

int lua_insert_char(lua_State* lua)
{
  if (lua == nullptr) return 0;
  Editor* editor = get_editor(lua);
  if (editor == nullptr) return 0;

  int ch = luaL_checkinteger (lua, 1);
  //ascii
  if (ch < 0 || ch > 127) return 0;

  insert_gap_buffer(editor->buffer, (char)ch);

  return 0;
}

int lua_insert_newline(lua_State *lua)
{
  if (lua == nullptr) return 0;
  Editor* editor = get_editor(lua);
  if (editor == nullptr) return 0;

  int times = luaL_checkinteger(lua, 1);
  if (times < 0) return 0;

  for (size_t i = 0; i < (size_t)times; i++)
    insert_gap_buffer(editor->buffer, '\n');

  return 0;
}

bool is_minor_mode(lua_State* lua, const char* mode_name)
{
  if (lua == nullptr) return false;

  lua_getglobal(lua, "modes");
  lua_getfield(lua, -1, mode_name);
  lua_getfield(lua, -1, "minor");

  bool result = lua_toboolean(lua, -1);

  lua_pop(lua, 3);
  return result;
}

int lua_is_minor_mode(lua_State* lua)
{
  if (lua == nullptr) return 0;
  Editor* editor = get_editor(lua);
  if (editor == nullptr) return 0;

  const char* mode_name = luaL_checkstring(lua, 1);
  bool result = is_minor_mode(lua, mode_name);
  lua_pushboolean(lua, result);

  return 1;
}

int lua_set_mode(lua_State* lua)
{
  if (lua == nullptr) return 0;
  Editor* editor = get_editor(lua);
  if (editor == nullptr) return 0;

  const char* target_modename = luaL_checkstring(lua, 1);

  set_mode(editor, target_modename);

  return 0;
}

int lua_get_mode(lua_State* lua)
{
  if (lua == nullptr) return 0;
  Editor* editor = get_editor(lua);
  if (editor == nullptr) return 0;

  lua_pushstring(lua, editor->modeinfo.current_mode);
  return 1;
}

int lua_save_mode(lua_State* lua)
{
  if (lua == nullptr) return 0;
  Editor* editor = get_editor(lua);
  if (editor == nullptr) return 0;

  const char* mode_name = luaL_checkstring(lua, 1);

  save_mode(editor, mode_name);

  return 0;
}

int lua_get_saved_mode(lua_State* lua)
{
  if (lua == nullptr) return 0;
  Editor* editor = get_editor(lua);
  if (editor == nullptr) return 0;

  lua_pushstring(lua, editor->modeinfo.pre_mode);
  return 1;
}

int lua_restore_mode(lua_State* lua)
{
  if (lua == nullptr) return 0;
  Editor* editor = get_editor(lua);
  if (editor == nullptr) return 0;

  restore_mode(editor);
  return 0;
}

int lua_delete_after_cursor(lua_State* lua)
{
  if (lua == nullptr) return 0;
  Editor* editor = get_editor(lua);
  if (editor == nullptr) return 0;

  delete_after_gap_buffer(editor->buffer);
  return 0;
}

int lua_delete_before_cursor(lua_State* lua)
{
  if (lua == nullptr) return 0;
  Editor* editor = get_editor(lua);
  if (editor == nullptr) return 0;

  delete_before_gap_buffer(editor->buffer);
  return 0;
}

int lua_reload_config(lua_State *lua)
{
  if (lua == nullptr) return 0;
  Editor* editor = get_editor(lua);
  if (editor == nullptr) return 0;

  const char *filename = luaL_optstring(lua, 1, "init.lua");
  Status result = lua_load_config(lua, filename);

  lua_pushboolean(lua, SUCCEEDED(result));

  return 1;
}
