#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include "gap_buffer.h"
#include "lua_vm.h"
#include "editor.h"
#include "file_handling.h"

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

int lua_is_minor_mode(lua_State* lua)
{
  if (lua == nullptr) return 0;
  Editor* editor = get_editor(lua);
  if (editor == nullptr) return 0;

  const char* mode_name = luaL_checkstring(lua, 1);
  bool result = is_minor_mode(editor, mode_name);
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

int lua_call_mode_hook(lua_State* lua)
{
  if (lua == nullptr) return 0;
  Editor* editor = get_editor(lua);
  if (editor == nullptr) return 0;

  const char* mode_name = luaL_checkstring(lua, 1);
  const char* hook_name = luaL_checkstring(lua, 2);

  call_mode_hook(editor, mode_name, hook_name);
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

int lua_set_anchor(lua_State* lua)
{
  if (lua == nullptr) return 0;
  Editor* editor = get_editor(lua);
  if (editor == nullptr) return 0;

  Position cur_pos = get_cursor_pos(editor->buffer);
  size_t achr_x = luaL_optinteger(lua, 1, cur_pos.x);
  size_t achr_y = luaL_optinteger(lua, 2, cur_pos.y);

  editor->cursor.anchor = repos_to_abspos(editor->buffer,
                                    (Position){.x = achr_x, .y = achr_y});
  editor->cursor.selecting = true;

  /* fprintf(stderr, "anchor set to %zu\n", editor->cursor.anchor); */
  /* fprintf(stderr, "cur_pos x=%zu y=%zu\n", cur_pos.x, cur_pos.y); */

  return 0;
}

int lua_clear_anchor(lua_State* lua)
{
  if (lua == nullptr) return 0;
  Editor* editor = get_editor(lua);
  if (editor == nullptr) return 0;

  editor->cursor.selecting = false;
  return 0;
}

int lua_delete_selected(lua_State* lua)
{
  if (lua == nullptr) return 0;
  Editor* editor = get_editor(lua);
  if (editor == nullptr) return 0;

  delete_selected(editor);
  return 0;
}

int lua_save_file(lua_State* lua)
{
  if (lua == nullptr) return 0;
  Editor* editor = get_editor(lua);
  if (editor == nullptr) return 0;

  save_file(editor);
  return 0;
}

int lua_get_line_end_cursor(lua_State* lua)
{
  if (lua == nullptr) return 0;
  Editor* editor = get_editor(lua);
  if (editor == nullptr) return 0;

  Position cur_pos = get_cursor_pos(editor->buffer);
  size_t line_end = get_line_length(editor->buffer, cur_pos.y);

  lua_pushinteger(lua, line_end);

  return 1;
}

int lua_get_line_num_cursor(lua_State* lua)
{
  if (lua == nullptr) return 0;
  Editor* editor = get_editor(lua);
  if (editor == nullptr) return 0;

  Position cur_pos = get_cursor_pos(editor->buffer);

  lua_pushinteger(lua, cur_pos.y);

  return 1;
}

int lua_get_cursor_pos(lua_State* lua)
{
  if (lua == nullptr) return 0;
  Editor* editor = get_editor(lua);
  if (editor == nullptr) return 0;

  Position p = get_cursor_pos(editor->buffer);
  lua_pushinteger(lua, p.x);   // first return value
  lua_pushinteger(lua, p.y);   // second return value
  return 2;                    // "I am returning 2 values"
}

int lua_cursor_forward_match(lua_State* lua)
{
  if (lua == nullptr) return 0;
  Editor* editor = get_editor(lua);
  if (editor == nullptr) return 0;

  char matcher = (char) luaL_checkinteger(lua, 1);
  size_t result;
  Position pos;

  bool found = forward_match(editor->buffer,editor->buffer->gap_start, matcher, &result);

  if (found)
  {
    pos = abspos_to_repos(editor->buffer, result);

    lua_pushinteger(lua, pos.x);
    lua_pushinteger(lua, pos.y);
    return 2;
  }

  lua_pushnil(lua);
  return 1;
}

int lua_cursor_backward_match(lua_State* lua)
{
  if (lua == nullptr) return 0;
  Editor* editor = get_editor(lua);
  if (editor == nullptr) return 0;

  char matcher = (char) luaL_checkinteger(lua, 1);
  size_t result;
  Position pos;

  bool found = backward_match(editor->buffer,editor->buffer->gap_start, matcher, &result);

  if (found)
  {
    pos = abspos_to_repos(editor->buffer, result);

    lua_pushinteger(lua, pos.x);
    lua_pushinteger(lua, pos.y);
    return 2;
  }

  lua_pushnil(lua);
  return 1;
}

int lua_char_at_cursor(lua_State* lua)
{
  if (lua == nullptr) return 0;
  Editor* editor = get_editor(lua);
  if (editor == nullptr) return 0;

  char c = gb_char_at(editor->buffer, editor->buffer->gap_start);
  char result[2] = {c, '\0'};

  lua_pushstring(lua, result);
  return 1;
}

int lua_get_total_lines(lua_State* lua)
{
  if (lua == nullptr) return 0;
  Editor* editor = get_editor(lua);
  if (editor == nullptr) return 0;

  size_t total_line = get_total_lines(editor->buffer);

  lua_pushinteger(lua, total_line);
  return 1;
}
