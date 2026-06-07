#pragma once
#include "editor_datatype.h"

Position get_cursor_pos(gap_buffer* buffer);

struct tb_event;
bool process_key(struct tb_event *ev, Editor *editor);
void update_scroll(Editor* editor);
void quit_editor(Editor* editor);

void set_mode(Editor* editor, const char* mode_name);
void save_mode(Editor* editor, const char* mode_name);
void restore_mode(Editor* editor);

void call_mode_hook(Editor* editor, const char* mode_name, const char* hook_name);

void delete_selected(Editor* editor);
bool is_minor_mode(Editor* editor, const char* mode_name);
