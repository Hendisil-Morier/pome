#define TB_IMPL
#include "termbox2.h"
#include "editor_datatype.h"
#include "gap_buffer.h"
#include "editor.h"

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

    if (screen_y >= editor->row_offset && screen_y < editor->row_offset + screen_rows)
      tb_set_cell(screen_x, screen_y - editor->row_offset, c, TB_DEFAULT, TB_DEFAULT);

    screen_x++;
  }

  char status_buffer[128] = {};
  Position cur_pos = get_cursor_pos(editor->buffer);
  snprintf(status_buffer, sizeof(status_buffer),
    "row %zu : col %zu", cur_pos.y + 1, cur_pos.x + 1);

  for (size_t i = 0; status_buffer[i]; i++)
    tb_set_cell(i, tb_height() - 1, status_buffer[i], TB_BLACK, TB_WHITE);

  tb_set_cursor(cur_pos.x, cur_pos.y - editor->row_offset);
  tb_present();
}

bool process_key(struct tb_event *ev, Editor *editor)
{
  if (ev->ch)
    insert_gap_buffer(editor->buffer, (char)ev->ch);

  switch (ev->key)
  {
  case TB_KEY_CTRL_Q:
    return false;

  case TB_KEY_BACKSPACE:
  case TB_KEY_BACKSPACE2:
    delete_before_gap_buffer(editor->buffer);
    break;

  case TB_KEY_ENTER:
    insert_gap_buffer(editor->buffer, '\n');
    break;

  case TB_KEY_ARROW_LEFT:
    move_gap(editor->buffer, 1, LEFT);
    break;
  case TB_KEY_ARROW_RIGHT:
    move_gap(editor->buffer, 1,RIGHT);
    break;

  case TB_KEY_ARROW_UP:
    move_gap(editor->buffer, 1, UP);
    break;
  case TB_KEY_ARROW_DOWN:
    move_gap(editor->buffer, 1,DOWN);
    break;

  default: return true;
  }

  return true;
}


int main(void)
{
  struct tb_event ev;
  Editor editor = {};
  editor.buffer = init_gap_buffer();

  tb_init();

  editor.term_stat.width = tb_width();
  editor.term_stat.height = tb_height();

  bool goon = true;
  while(goon)
  {
    render_char(&editor);

    tb_poll_event(&ev);

    if(ev.type == TB_EVENT_KEY)
      goon = process_key(&ev, &editor);
  }

  tb_shutdown();
  free_gb(editor.buffer);
  return 0;
}
