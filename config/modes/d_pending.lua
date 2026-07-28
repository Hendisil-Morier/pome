-- modes/d_pending.lua
local std = require("stdlib")
local base = require("modes.base_keymap")

local editing = std.editing

local function make_d_pending_config()
  local keymap = inherit(
    {
      [key.d] = editing.delete_line,
    },
    base
  )
  return {
    minor  = true,
    keymap = keymap,
  }
end

return make_d_pending_config