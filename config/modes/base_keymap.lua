local std = require("stdlib")

local motion = std.motion

return {
  [key.left]       = motion.cursor_left,
  [key.right]      = motion.cursor_right,
  [key.up]         = motion.cursor_up,
  [key.down]       = motion.cursor_down,

  [key.esc]        = bind(std.mode.safe_set_mode, "normal"),
  [key.ctrl.r]  = function() dofile(pome.get_config_dir() .. "/init.lua") end,
  [key.ctrl.q]  = pome.quit_editor,
  [key.ctrl.s]  = pome.save_file,
}