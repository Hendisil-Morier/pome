#pragma once

#include <lualib.h>
#include <lauxlib.h>
#include "editor_datatype.h"
#include "error.h"

Editor* get_editor(lua_State *lua);
void init_lua(Editor* editor);

Status lua_load_config(lua_State *lua, const char* filename);
void lua_set_intfield(lua_State* lua, int value, const char* filename);

void register_constant(lua_State *lua);
void register_primitives(lua_State *lua);

