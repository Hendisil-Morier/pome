-- modes/visual.lua
local std = require("stdlib")
local base = require("modes.base_keymap")

local motion = std.motion
local register = std.register
local mode = std.mode

local function make_visual_config()
  local keymap = inherit(
    {
      [key.h] = motion.cursor_left,
      [key.l] = motion.cursor_right,
      [key.k] = motion.cursor_up,
      [key.j] = motion.cursor_down,

      [key.d] = function()
        pome.delete_selected()
        pome.clear_anchor()
        mode.safe_set_mode("normal")
      end,

      [key.v] = bind(mode.safe_set_mode, "normal"),
      
      [key.y] = function()
        register.yank()
        mode.safe_set_mode("normal")
        end,
    },
    base
  )

  return {
    on_enter = function(sel_mode)
        pome.set_anchor(sel_mode)
    end,
    on_exit  = pome.clear_anchor,
    keymap   = keymap,
  }
end

return make_visual_config