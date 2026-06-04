#pragma once
#include "editor_datatype.h"

Position get_cursor_pos(gap_buffer* buffer);

bool process_key(struct tb_event *ev, Editor *editor);
void update_scroll(Editor* editor);
void quit_editor(Editor* editor);

void set_mode(Editor* editor, const char* mode_name, bool save);
