function safe_set_mode(name)
  if (name ~= get_mode())
  then set_mode(name)
  end
end

function enter_minor_mode(name)
  local cur_mode = get_mode()
  if not is_minor_mode(cur_mode) then
    save_mode(cur_mode)
  end
  safe_set_mode(name)
end

function cursor_left()  move_cursor(direction.left, 1)  end
function cursor_right() move_cursor(direction.right, 1) end
function cursor_up()    move_cursor(direction.up, 1)    end
function cursor_down()  move_cursor(direction.down, 1)  end

function insert_newline() insert_newlines(1) end
