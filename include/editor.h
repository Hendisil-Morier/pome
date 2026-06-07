#pragma once
#include "editor_datatype.h"
struct tb_event;

/* GENERATE BELOW THIS LINE */
bool is_minor_mode(Editor* editor, const char* mode_name);
Position get_cursor_pos(gap_buffer *buffer);
void call_mode_hook(Editor* editor, const char* mode_name, const char* hook_name);
void set_mode(Editor* editor, const char* mode_name);
void save_mode(Editor* editor, const char* mode_name);
void restore_mode(Editor* editor);
bool call_keymap(struct tb_event* ev, Editor* editor);
void call_default(struct tb_event* ev, Editor* editor);
void process_key(struct tb_event *ev, Editor* editor);
void quit_editor(Editor* editor);
void update_scroll(Editor* editor);
void delete_selected(Editor* editor);
