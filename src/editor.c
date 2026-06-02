#define TB_IMPL
#include "termbox2.h"
#include "error.h"

typedef struct
{
  size_t x, y;
} Position;

typedef struct
{
  size_t width, height;
} terminal_state;

typedef struct
{
  Position pos;
} cursor_state;

typedef struct
{
  size_t capacity;
  size_t gap_start;
  size_t gap_end;
  char *buf;
} gap_buffer;

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
  free(buffer);
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

typedef struct
{
  enum
  {
    DIR_RIGHT,
    DIR_LEFT,
  } direction;
} Direction;

#define RIGHT ((Direction){DIR_RIGHT})
#define LEFT ((Direction){DIR_LEFT})

Status move_gap(gap_buffer* buffer, size_t distance, Direction dir)
{
  if (buffer == nullptr) return FAILURE;
  if (distance == 0) return SUCCESS;

  size_t safe_distance = (dir.direction == DIR_RIGHT)?
        (buffer->capacity - buffer->gap_end) : buffer->gap_start;

  if (distance > safe_distance) distance = safe_distance;
  if (distance > buffer->gap_end - buffer->gap_start)
    expand_gap_buffer(buffer, buffer->capacity + distance);
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
  }

  return SUCCESS;
}
