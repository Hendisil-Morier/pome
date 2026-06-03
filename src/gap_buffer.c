#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "error.h"
#include "editor_datatype.h"
#include "editor.h"

constexpr size_t GAP_BUFFER_SIZE = 32;

gap_buffer* init_gap_buffer(void)
{
  gap_buffer* tmp = malloc(sizeof(gap_buffer));
  if (tmp == nullptr) return nullptr;

  tmp->buf = calloc(GAP_BUFFER_SIZE, 1);
  if (tmp->buf == nullptr) {free(tmp); return nullptr;}

  tmp->gap_start = 0;
  tmp->gap_end = GAP_BUFFER_SIZE;
  tmp->capacity = GAP_BUFFER_SIZE;

  return tmp;
}

void free_gb(gap_buffer* buffer)
{
  free(buffer->buf);
}

Status expand_gap_buffer(gap_buffer* buffer, size_t new_cap)
{
  if (buffer == nullptr) return FAILURE;

  size_t old_cap = buffer->capacity;
  size_t tail_text_len = old_cap - buffer->gap_end;
  size_t new_gap_end = new_cap - tail_text_len;
  size_t old_gap_end = buffer->gap_end;

  char* new_buffer = realloc(buffer->buf, new_cap);
  if (new_buffer == nullptr) return FAILURE;

  buffer->capacity = new_cap;
  buffer->gap_end = new_gap_end;
  buffer->buf = new_buffer;
  memmove(new_buffer + new_gap_end, new_buffer + old_gap_end, tail_text_len);

  return SUCCESS;
}

Status append_gap_buffer(gap_buffer* buffer, char c)
{
  if (buffer == nullptr) return FAILURE;

  if (buffer->gap_start >= buffer->gap_end)
  {
    Status st = expand_gap_buffer(buffer, buffer->capacity * 2);
    if (FAILED(st)) return FAILURE;
  }

  buffer->buf[buffer->gap_end - 1] = c;
  buffer->gap_end--;

  return SUCCESS;
}

Status insert_gap_buffer(gap_buffer* buffer, char c)
{
  if (buffer == nullptr) return FAILURE;

  if (buffer->gap_start >= buffer->gap_end)
  {
    Status st = expand_gap_buffer(buffer, buffer->capacity * 2);
    if (FAILED(st)) return FAILURE;
  }

  buffer->buf[buffer->gap_start] = c;
  buffer->gap_start++;

  return SUCCESS;
}

Status delete_before_gap_buffer(gap_buffer* buffer)
{
  if (buffer == nullptr) return FAILURE;

  if (buffer->gap_start == 0) return FAILURE;
  buffer->gap_start--;

  return SUCCESS;
}

Status delete_after_gap_buffer(gap_buffer* buffer)
{
  if (buffer == nullptr) return FAILURE;

  if (buffer->gap_end == buffer->capacity) return FAILURE;
  buffer->gap_end++;

  return SUCCESS;
}


Status move_gap_horizontal(gap_buffer* buffer, size_t distance, Direction dir)
{
  if (buffer == nullptr) return FAILURE;
  if (distance == 0) return SUCCESS;

  size_t safe_distance = (dir.direction == DIR_RIGHT)?
        (buffer->capacity - buffer->gap_end) : buffer->gap_start;

  if (distance > safe_distance) distance = safe_distance;

  char* buf = buffer->buf;

  char* left_dest_address = buf + (buffer->gap_end - distance);
  char* left_src_address = buf + (buffer->gap_start - distance);

  switch(dir.direction)
  {
    case DIR_LEFT:
      memmove(left_dest_address, left_src_address,distance);
      buffer->gap_start -= distance;
      buffer->gap_end -= distance;
      break;
    case DIR_RIGHT:
      memmove( buf + buffer->gap_start, buf + buffer->gap_end, distance);
      buffer->gap_start += distance;
      buffer->gap_end += distance;
      break;
    default: return FAILURE;
  }

  return SUCCESS;
}

Status move_gap_to(gap_buffer* buffer, size_t abs_pos)
{
  if (buffer == nullptr) return FAILURE;

  const size_t safe_pos = buffer->gap_start + (buffer->capacity - buffer->gap_end);
  if (abs_pos > safe_pos) abs_pos = safe_pos;

  Status st = FAILURE;
  if (abs_pos > buffer->gap_start)
    st = move_gap_horizontal(buffer, abs_pos - buffer->gap_start, RIGHT);

  else if (abs_pos < buffer->gap_start)
    st = move_gap_horizontal(buffer, buffer->gap_start - abs_pos, LEFT);

  else
    st = SUCCESS;

  return st;
}

char gb_char_at(gap_buffer *buffer, size_t abs_pos)
{
  if (buffer == nullptr) return 0;

  const size_t safe_pos = buffer->gap_start + (buffer->capacity - buffer->gap_end);
  if (abs_pos > safe_pos)
    return 0;

  if (abs_pos >= buffer->gap_start)
    abs_pos += buffer->gap_end - buffer->gap_start;

  return buffer->buf[abs_pos];
}

size_t get_line_start(gap_buffer *buffer, size_t target_line)
{
  size_t logic_text_len = buffer->gap_start + (buffer->capacity - buffer->gap_end);
  size_t lines = 0;
  for (size_t i = 0; i < logic_text_len; i++)
  {
    if (lines == target_line) return i;
    if (gb_char_at(buffer, i) == '\n') lines++;
  }
  return logic_text_len;
}

size_t get_line_length(gap_buffer* buffer, size_t target_line)
{
  size_t logic_text_len = buffer->gap_start + (buffer->capacity - buffer->gap_end);
  size_t line_start = get_line_start(buffer, target_line);
  size_t i = 0;

  for (;line_start + i < logic_text_len; i++)
    if (gb_char_at(buffer, line_start + i) == '\n') break;

  return i;
}

size_t get_total_lines(gap_buffer* buffer)
{
  if (buffer == nullptr) return 0;

  size_t logic_text_len = buffer->gap_start + (buffer->capacity - buffer->gap_end);
  size_t lines = 1;
  for (size_t i = 0; i < logic_text_len; i++)
  {
    char c = gb_char_at(buffer, i);
    if (c == '\n') lines++;
  }

  return lines;
}

size_t max(size_t a, size_t b)
{
  return (a < b)? a : b;
}

Status move_gap_vertical(gap_buffer *buffer, size_t target_line)
{
  if (buffer == nullptr) return FAILURE;
  /* size_t logic_text_len = buffer->gap_start + (buffer->capacity - buffer->gap_end); */

  Position cur_pos = get_cursor_pos(buffer);
  size_t line_length = get_line_length(buffer, target_line);
  size_t line_start = get_line_start(buffer, target_line);
  size_t target_x = max(cur_pos.x, line_length);

  Status st = FAILURE;
  st = move_gap_to(buffer, line_start + target_x);

  return st;
}

Status move_gap(gap_buffer* buffer, size_t times, Direction direction)
{
  if (buffer == nullptr) return FAILURE;
  if (times == 0) return SUCCESS;

  Position cur_pos = get_cursor_pos(buffer);

  switch (direction.direction)
  {
    case DIR_LEFT:
      return move_gap_horizontal(buffer, times, LEFT);
    case DIR_RIGHT:
      return move_gap_horizontal(buffer, times,RIGHT);
    case DIR_UP:
      if (cur_pos.y < times) times = cur_pos.y;
      return move_gap_vertical(buffer,cur_pos.y - times);
    case DIR_DOWN:
      size_t total = get_total_lines(buffer);
      size_t target = cur_pos.y + times;
      if (target > total) target = total;
      return move_gap_vertical(buffer,target);
    default:
      return FAILURE;
  }
}
