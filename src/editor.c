#include "editor_datatype.h"
#include "gap_buffer.h"

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
