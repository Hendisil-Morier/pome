require("runtime")
require("pome_stdlib")

local normal_mode = "normal"
local g_pending_mode = "g_pending"
local insert_mode = "insert"
local visual_mode = "visual"

local base_keymap = {
  [key_press.arrow_left]  = cursor_left,
  [key_press.arrow_right] = cursor_right,
  [key_press.arrow_up]    = cursor_up,
  [key_press.arrow_down]  = cursor_down,

  [key_press.esc]         = bind(safe_set_mode, normal_mode),
  [key_press.ctrl_r]      = bind(load_config, "init.lua"),
  [key_press.ctrl_q]      = quit_editor,
}

local normal_keymap = inherit (
  {
    [key_press.h]  = cursor_left,
    [key_press.l] = cursor_right,
    [key_press.k]    = cursor_up,
    [key_press.j]  = cursor_down,

    -- press g to enter g_pending minor mode
    [key_press.g]    = bind(enter_minor_mode, g_pending_mode),
    [key_press.i]    = bind(safe_set_mode, insert_mode),
    [key_press.v]    = bind(safe_set_mode, visual_mode),
  },
  base_keymap
)

local visual_keymap = inherit (
  {
    [key_press.h]  = cursor_left,
    [key_press.l] = cursor_right,
    [key_press.k]    = cursor_up,
    [key_press.j]  = cursor_down,

    [key_press.d] = function()
      delete_selected();
      clear_anchor();
      safe_set_mode(normal_mode);
    end,

    [key_press.v] = bind(safe_set_mode, normal_mode),
  },
  base_keymap
)

local normal_config = {
  -- default = function(ch) insert_char(ch) end,
  keymap = normal_keymap,
}

local insert_keymap = inherit(
  {
    [key_press.backspace]   = delete_before_cursor,
    [key_press.backspace2]  = delete_before_cursor,
    [key_press.delete]      = delete_after_cursor,

    [key_press.enter]       = insert_newline,
  },
  base_keymap
)

local visual_config =
  {
    on_enter = set_anchor,
    on_exit = clear_anchor,
    keymap = visual_keymap,
  }

local insert_config =
  {
    default = function(ch) insert_char(ch) end,

    keymap = insert_keymap,
  }

local g_pending_keymap = inherit(
  {
    [key_press.g] = bind(move_cursor_to, 1, 0),
  },
  base_keymap
)

local g_pending_config = {
  minor = true,
  keymap = g_pending_keymap,
}

define_mode(normal_mode, normal_config)
define_mode(g_pending_mode, g_pending_config)
define_mode(insert_mode, insert_config)
define_mode(visual_mode, visual_config)

--set default mode
safe_set_mode(normal_mode)
