#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include "editor_datatype.h"
#include "termbox2.h"
#include "error.h"
#include "c_to_lua.h"

Editor* get_editor(lua_State *lua);
void register_constant(lua_State *lua);
void register_primitives(lua_State *lua);

typedef struct
{
  const char* name;
  lua_CFunction func_pointer;
} lua_name_function;

typedef struct
{
  const char* name;
  int value;
} lua_name_intconst;

const lua_name_function func_api[] = {
  {"move_cursor",          lua_move_cursor},
  {"move_cursor_to",       lua_move_cursor_to},

  {"quit_editor",          lua_quit_editor},

  {"insert_char",          lua_insert_char},

  {"set_mode",             lua_set_mode},
  {"save_mode",            lua_save_mode},
  {"get_mode",             lua_get_mode},
  {"get_saved_mode",       lua_get_saved_mode},

  {"restore_mode",         lua_restore_mode},
  {"is_minor_mode",        lua_is_minor_mode},

  {"insert_newlines",      lua_insert_newline},

  {"delete_after_cursor",  lua_delete_after_cursor},
  {"delete_before_cursor", lua_delete_before_cursor},

  {"call_mode_hook",       lua_call_mode_hook},

  {"set_anchor",           lua_set_anchor},
  {"clear_anchor",         lua_clear_anchor},

  {"delete_selected",      lua_delete_selected},

  {"load_config",          lua_reload_config},
  {"save_file",            lua_save_file},

  {"get_line_end_cursor", lua_get_line_end_cursor},
  {"get_line_num_cursor", lua_get_line_num_cursor},

  {"get_cursor_pos", lua_get_cursor_pos},
  {"cursor_forward_match", lua_cursor_forward_match},
  {"cursor_backward_match", lua_cursor_backward_match},

  {"char_at_cursor", lua_char_at_cursor},
  {"get_total_lines", lua_get_total_lines},
};

const lua_name_intconst intconst_api[] = {
  // directions
  {"right",   DIR_RIGHT},
  {"left",    DIR_LEFT},
  {"up",      DIR_UP},
  {"down",    DIR_DOWN},

  // arrow keys
  {"arrow_left",  TB_KEY_ARROW_LEFT},
  {"arrow_right", TB_KEY_ARROW_RIGHT},
  {"arrow_up",    TB_KEY_ARROW_UP},
  {"arrow_down",  TB_KEY_ARROW_DOWN},

  // misc keys
  {"backspace",   TB_KEY_BACKSPACE},
  {"backspace2",  TB_KEY_BACKSPACE2},
  {"delete",      TB_KEY_DELETE},
  {"enter",       TB_KEY_ENTER},
  {"esc",         TB_KEY_ESC},

  // control combinations
  {"ctrl_r", TB_KEY_CTRL_R},
  {"ctrl_q", TB_KEY_CTRL_Q},
  {"ctrl_s", TB_KEY_CTRL_S},
};

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

  register_primitives(lua);
  register_constant(lua);

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

void register_function(lua_State* lua, const char* func_name, lua_CFunction func_pointer)
{
  lua_pushcfunction(lua, func_pointer);
  lua_setfield(lua, -2, func_name);
}

void register_constant(lua_State *lua)
{
  //directions
  lua_newtable(lua);
  for (size_t i = 0; i < 4; i++)   // first 4 entries are directions
    lua_set_intfield(lua, intconst_api[i].value, intconst_api[i].name);
  lua_setglobal(lua, "direction");

  //##keymap
  lua_newtable(lua);

  for (size_t i = 4; i < sizeof(intconst_api)/sizeof(intconst_api[0]); i++)
    lua_set_intfield(lua, intconst_api[i].value, intconst_api[i].name);

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
  // create the 'pome' namespace table
  lua_newtable(lua);

  for (size_t i = 0; i < sizeof(func_api)/sizeof(func_api[0]); i++)
    register_function(lua, func_api[i].name, func_api[i].func_pointer);

  lua_setglobal(lua, "pome");   // all functions now live under `pome`
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
