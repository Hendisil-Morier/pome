-- modes/normal.lua
local std = require("stdlib")
local base = require("modes.base_keymap")

local register = std.register
local mode = std.mode
local motion = std.motion
local editing = std.editing

local function make_normal_config()
local keymap = inherit(
    {
        [key.h] = motion.cursor_left,
        [key.l] = motion.cursor_right,
        [key.k] = motion.cursor_up,
        [key.j] = motion.cursor_down,
    
        [key.d] = bind(mode.enter_minor_mode, "d_pending"),
        [key.g] = bind(mode.enter_minor_mode, "g_pending"),
        [key.G] = motion.goto_lastline,
        [key.i] = bind(mode.safe_set_mode, "insert"),
        [key.v] = bind(mode.safe_set_mode, "visual"),
        [key.w] = motion.word_forward,
        [key.b] = motion.word_backward,
        
        [key.o] = bind(editing.openline, false),
        [key.O] = bind(editing.openline, true),
        
        [":"] = bind(mode.safe_set_mode, "command"),
        
        [key.u] = pome.undo,
        [key.U] = pome.redo,
    
        [key.p] = register.paste,
    },

    base
);

  local sequences =
    {
      ["space e r"] = bind(dofile, "config/init.lua"),
      ["space q q"] = pome.quit_editor,
    }
    
  return {
    keymap = keymap,
    sequences = sequences,
    on_enter = function() pome.set_cursor_shape("block") end,
  }
end

-- return the entire function for hot reload
return make_normal_config
