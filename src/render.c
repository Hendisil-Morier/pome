#define TB_IMPL

#include "termbox2.h"
#include "editor_datatype.h"
#include "editor.h"
#include "gap_buffer.h"

constexpr int TAB_width = 4;

void render_status_bar (Editor* editor)
{
  char status_buffer[128] = {};
  Position cur_pos = get_cursor_pos(editor->buffer);
  snprintf(status_buffer, sizeof(status_buffer),
    "%s | %zu : %zu", editor->modeinfo.current_mode,
           cur_pos.y + 1, cur_pos.x + 1);

  for (size_t i = 0; status_buffer[i]; i++)
    tb_set_cell(i, tb_height() - 1, status_buffer[i], TB_BLACK, TB_WHITE);

  tb_set_cursor(cur_pos.x, cur_pos.y - editor->row_offset);
}

void render_char (Editor *editor)
{
  if (editor == nullptr) return;
  if (editor->buffer == nullptr) return;
  tb_clear();

  gap_buffer* buffer = editor->buffer;
  const size_t logic_text_len = buffer->gap_start + (buffer->capacity - buffer->gap_end);

  size_t screen_rows = tb_height() - 1;
  size_t screen_x = 0;
  size_t screen_y = 0;
  char c = 0;
  for(size_t i = 0; i < logic_text_len; i++)
  {
    c = gb_char_at(buffer, i);
    if (c == '\n')
    {
      screen_y++; screen_x = 0;
      continue;
    }

    if (c == '\t')
    {
      // advance to the next multiple of 8 (standard tab width)
      size_t next_tab = (screen_x / TAB_width + 1) * TAB_width;
      // fill with spaces up to the next tab stop
      while (screen_x < next_tab)
      {
        if (screen_y >= editor->row_offset && screen_y < editor->row_offset + screen_rows)
          tb_set_cell(screen_x, screen_y - editor->row_offset, ' ', TB_DEFAULT, TB_DEFAULT);
        screen_x++;
      }
      continue;
    }

    if (screen_y >= editor->row_offset && screen_y < editor->row_offset + screen_rows)
      tb_set_cell(screen_x, screen_y - editor->row_offset, c, TB_DEFAULT, TB_DEFAULT);

    screen_x++;
  }

  render_status_bar(editor);

  tb_present();
}
