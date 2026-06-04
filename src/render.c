#define TB_IMPL
#include "termbox2.h"
#include "editor_datatype.h"
#include "gap_buffer.h"
#include "editor.h"
#include "error.h"

Status save_file(Editor* editor);

void render_status (Editor* editor)
{
  char status_buffer[128] = {};
  Position cur_pos = get_cursor_pos(editor->buffer);
  snprintf(status_buffer, sizeof(status_buffer),
    "row %zu : col %zu", cur_pos.y + 1, cur_pos.x + 1);

  for (size_t i = 0; status_buffer[i]; i++)
    tb_set_cell(i, tb_height() - 1, status_buffer[i], TB_BLACK, TB_WHITE);
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
  case TB_KEY_DELETE:
    delete_after_gap_buffer(editor->buffer);
    break;

  case TB_KEY_ENTER:
    insert_gap_buffer(editor->buffer, '\n');
    break;

  case TB_KEY_CTRL_S:
    save_file(editor);
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

Status load_file(Editor* editor)
{
  if (editor == nullptr)
    return FAILURE;

  const char* filename = editor->filename;
  if (filename == nullptr)
    return FAILURE;

  FILE *file = fopen(filename, "r");
  if (file == nullptr)
  {
    perror("Unable to open file.");
    return FAILURE;
  }

  int c;
  while((c = fgetc(file)) != EOF)
    insert_gap_buffer(editor->buffer, c);

  if (ferror(file))
  {
    perror("I/O error when reading.");
    goto clean_up;
  }

  fclose(file);
  return SUCCESS;

clean_up:
  fclose(file);
  return FAILURE;
}

Status save_file(Editor* editor)
{
  if (editor == nullptr)
    return FAILURE;

  const char* filename = editor->filename;
  if (filename == nullptr)
    return FAILURE;

  FILE *file = fopen(filename, "w");
  if (file == nullptr)
  {
    perror("Unable to open file.");
    return FAILURE;
  }

  gap_buffer* buffer = editor->buffer;
  size_t logic_text_len = buffer->gap_start + (buffer->capacity - buffer->gap_end);

  for (size_t i = 0; i < logic_text_len; i++)
  {
    char c = gb_char_at(buffer, i);
    fputc(c, file);
  }

  if (ferror(file))
  {
    perror("I/O error when writing.");
    goto clean_up;
  }

  fclose(file);
  return SUCCESS;

clean_up:
  fclose(file);
  return FAILURE;
}

int main(int argc, char* argv[])
{
  struct tb_event ev;
  Editor editor = {};
  editor.buffer = init_gap_buffer();

  tb_init();

  editor.term_stat.width = tb_width();
  editor.term_stat.height = tb_height();

  editor.filename = argv[1];

  load_file(&editor);
  move_gap_to(editor.buffer, 0);

  bool goon = true;
  while(goon)
  {
    render_char(&editor);

    tb_poll_event(&ev);

    if(ev.type == TB_EVENT_KEY)
      goon = process_key(&ev, &editor);

    if (ev.type == TB_EVENT_RESIZE)
    {
      editor.term_stat.width = tb_width();
      editor.term_stat.height = tb_height();
    }

    update_scroll(&editor);
  }

  tb_shutdown();
  free_gb(editor.buffer);
  return 0;
}
