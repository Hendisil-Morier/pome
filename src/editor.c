#include "editor_datatype.h"
#include "gap_buffer.h"
#include "termbox2.h"
#include "file_handling.h"

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

void process_key(struct tb_event *ev, Editor* editor)
{
  if (ev == nullptr || editor == nullptr)
    return;

  lua_State *lua = editor->lua;

  if (ev->ch == true)
  {
    insert_gap_buffer(editor->buffer, (char)ev->ch);
    return;
  }

  lua_getglobal(lua, "keymap");
  lua_pushinteger(lua, ev->key);
  lua_gettable(lua, -2);

  if (lua_isfunction(lua, -1))
  {
    if (lua_pcall(lua, 0,0,0) != LUA_OK)
    {
      fprintf(stderr, "Lua error: %s\n", lua_tostring(lua, -1));
      lua_pop(lua, 1);
    }
  }
  else
      lua_pop(lua, 1);

  lua_pop(lua, 1);
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
