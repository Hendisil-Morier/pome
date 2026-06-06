#pragma once

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

int lua_move_cursor(lua_State* lua);
int lua_move_cursor_to(lua_State* lua);

int lua_quit_editor(lua_State* lua);

int lua_insert_char(lua_State* lua);
int lua_insert_newline(lua_State *lua);

bool is_minor_mode(lua_State* lua, const char* mode_name);
int lua_set_mode(lua_State* lua);
int lua_save_mode(lua_State* lua);

int lua_get_mode(lua_State* lua);
int lua_get_saved_mode(lua_State* lua);

int lua_restore_mode(lua_State* lua);
int lua_delete_after_cursor(lua_State* lua);

int lua_delete_before_cursor(lua_State* lua);
int lua_reload_config(lua_State *lua);

int lua_is_minor_mode(lua_State* lua);

int lua_call_mode_hook(lua_State* lua);

int lua_clear_anchor(lua_State* lua);
int lua_set_anchor(lua_State* lua);

int lua_delete_selected(lua_State* lua);
int lua_save_file(lua_State* lua);

int lua_get_line_end_cursor(lua_State* lua);
int lua_get_line_num_cursor(lua_State* lua);
int lua_get_cursor_pos(lua_State* lua);

int lua_cursor_forward_match(lua_State* lua);
int lua_cursor_backward_match(lua_State* lua);
