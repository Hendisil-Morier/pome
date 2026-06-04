function bind(f, ...)
    local args = {...}
    return function() f(table.unpack(args)) end
end

keymap = {
    [key_press.arrow_left]  = bind(move_cursor, direction.left, 1),
    [key_press.arrow_right] = bind(move_cursor, direction.right, 1),
    [key_press.arrow_up]    = bind(move_cursor, direction.up, 1),
    [key_press.arrow_down]  = bind(move_cursor, direction.down, 1),

    [key_press.enter]       = bind(insert_char,),

    [key_press.ctrl_q]      = quit_editor,
}
