#include "editor_datatype.h"
#include "gap_buffer.h"
#include "termbox2.h"
#include "lua_vm.h"

Position get_cursor_pos(gap_buffer *buffer)
{
  Position cur_pos = {};
  if (buffer == nullptr) return cur_pos;

  constexpr char newline_char = '\n';
  char c = 0;
  size_t x = 0;
  size_t y = 0;

  for (size_t i = 0; i < buffer->gap_start; i++)
  {
    c = gb_char_at(buffer, i);

    if (c == newline_char) {y++; x = 0;}
    else x++;
  }

  cur_pos.x = x;
  cur_pos.y = y;

  return cur_pos;
}

void set_mode(Editor* editor, const char* mode_name)
{
  if (editor == nullptr) return;

  free((void*)editor->modeinfo.current_mode);

  editor->modeinfo.current_mode = strdup(mode_name);
}

void save_mode(Editor* editor, const char* mode_name)
{
  if (editor == nullptr) return;

  free((void*) editor->modeinfo.pre_mode);

  editor->modeinfo.pre_mode = strdup(mode_name);
}

void restore_mode(Editor* editor)
{
  if (editor == nullptr) return;

  free((void*)editor->modeinfo.current_mode);

  editor->modeinfo.current_mode = editor->modeinfo.pre_mode;
  editor->modeinfo.pre_mode = nullptr;
}

bool call_keymap(struct tb_event* ev, Editor* editor)
{
  if (ev == nullptr || editor == nullptr)
    return false;

  lua_State *lua = editor->lua;

  lua_getglobal(lua, "modes");
  lua_getfield(lua, -1, editor->modeinfo.current_mode);
  lua_getfield(lua, -1, "keymap");

  uint32_t key = ev->key ? ev->key : ev->ch;
  lua_pushinteger(lua, key);

  lua_gettable(lua, -2);

  if (lua_isfunction(lua, -1) == false)
  {
    lua_pop(lua, 4); //pop nil
    return false;
  }

  if (lua_pcall(lua, 0, 0, 0) != LUA_OK)
  {
    fprintf(stderr, "Lua keymap error: %s\r\n",
            lua_tostring(lua, -1));

    lua_pop(lua, 1);
    return false;
  }

  lua_pop(lua, 3);

  return true;
}

void call_default(struct tb_event* ev, Editor* editor)
{
  if (ev == nullptr || editor == nullptr)
    return;

  lua_State *L = editor->lua;

  lua_getglobal(L, "modes");
  lua_getfield(L, -1, editor->modeinfo.current_mode);
  lua_getfield(L, -1, "default");

  if (ev->ch == 0)
  {
    lua_pop(L, 3);
    return;
  }

  if (!lua_isfunction(L, -1))
  {
    lua_pop(L, 3);   // pop default, mode_table, modes
    return;
  }

  lua_pushinteger(L, ev->ch);

  if (lua_pcall(L, 1, 0, 0) != LUA_OK)
  {
    fprintf(stderr, "Lua efault error: %s\r\n", lua_tostring(L, -1));
    lua_pop(L, 1);
    lua_pop(L, 2);

    return;
  }

  lua_pop(L, 2);  // pop mode_table and modes
}

void process_key(struct tb_event *ev, Editor* editor)
{
  if (ev == nullptr || editor == nullptr)
    return;

  const char* mode_before = editor->modeinfo.current_mode;

  bool mode_changed = (mode_before == editor->modeinfo.current_mode);
  bool current_is_minor = is_minor_mode(editor->lua, editor->modeinfo.current_mode);
  bool saved_is_major = editor->modeinfo.pre_mode != nullptr
    &&!is_minor_mode(editor->lua, editor->modeinfo.pre_mode);

  bool defau = !call_keymap(ev,editor);

  if (defau == true)
    call_default(ev, editor);

  if (mode_changed && current_is_minor && saved_is_major)
    restore_mode(editor);
}

void quit_editor(Editor* editor)
{
  if (editor == nullptr) return;

  editor->running = false;
}

void update_scroll(Editor* editor)
{
 if (editor == nullptr) return;
 gap_buffer* buffer = editor->buffer;

 Position cur_pos = get_cursor_pos(buffer);
 size_t screen_row = tb_height() - 1;

 if (cur_pos.y < editor->row_offset)
   editor->row_offset = cur_pos.y;

 if (cur_pos.y >= editor->row_offset + screen_row)
   editor->row_offset = cur_pos.y - screen_row + 1;
}
