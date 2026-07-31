-- modes/insert.lua
local std = require("stdlib")
local base = require("modes.base_keymap")

local editing = std.editing

local function make_insert_config()
  local keymap = inherit(
    {
      [key.backspace]  = editing.delete_before_cursor,
      [key.delete]     = editing.delete_after_cursor,
      [key.enter]      = editing.insert_newline,
    },
    base
  )

  return {
    default = function(ch) editing.insert_char(ch) end,
    keymap  = keymap,
    on_enter = function() pome.set_cursor_shape("bar_blink") end,
  }
end

return make_insert_config