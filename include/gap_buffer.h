#pragma once
#include "editor_datatype.h"
#include "error.h"

gap_buffer* init_gap_buffer(void);
void free_gb(gap_buffer* buffer);
Status expand_gap_buffer(gap_buffer* buffer, size_t new_cap);

Status append_gap_buffer(gap_buffer* buffer, char c);
Status insert_gap_buffer(gap_buffer* buffer, char c);

Status delete_before_gap_buffer(gap_buffer* buffer);
Status delete_after_gap_buffer(gap_buffer* buffer);

Status move_gap(gap_buffer* buffer, size_t times, Direction direction);
Status move_gap_to(gap_buffer* buffer, size_t abs_pos);

char gb_char_at(gap_buffer *buffer, size_t abs_pos);
