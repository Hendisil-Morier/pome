require("runtime")

local normal_config = {
    default  = function(ch) insert_char(ch) end,
    keymap = {
        [key_press.arrow_left]  = bind(move_cursor, direction.left, 1),
        [key_press.arrow_right] = bind(move_cursor, direction.right, 1),
        [key_press.arrow_up] = bind(move_cursor, direction.up, 1),
        [key_press.arrow_down] = bind(move_cursor, direction.down, 1),

        [key_press.delete] = delete_after_cursor,
        [key_press.backspace] = delete_before_cursor,
        [key_press.backspace2] = delete_before_cursor,

        [key_press.enter] = bind(insert_newline, 1),

        [key_press.ctrl_r] = bind(load_config, "init.lua"),

        [key_press.ctrl_q]      = quit_editor,
    }
}

define_mode("normal", normal_config)
set_mode("normal")
