#pragma once
#include "editor_datatype.h"
#include "error.h"

size_t min(size_t a, size_t b);
size_t max(size_t a, size_t b);

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

size_t get_line_start(gap_buffer *buffer, size_t target_line);
size_t get_line_length(gap_buffer* buffer, size_t target_line);
size_t get_logic_text_len(gap_buffer* buffer);
size_t get_total_lines(gap_buffer* buffer);


Position abspos_to_repos(gap_buffer* buffer, size_t abs_pos);
size_t repos_to_abspos(gap_buffer* buffer, Position repos);

bool forward_match(gap_buffer* buffer, size_t from, char matcher, size_t* result);
bool backward_match(gap_buffer* buffer, size_t from, char matcher, size_t* result);
