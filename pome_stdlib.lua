function safe_set_mode(name)
  if (name ~= pome.get_mode())
  then pome.set_mode(name)
  end
end

function enter_minor_mode(name)
  local cur_mode = pome.get_mode()
  if not pome.is_minor_mode(cur_mode) then
    pome.save_mode(cur_mode)
  end
  safe_set_mode(name)
end

function cursor_left()  pome.move_cursor(direction.left, 1)  end
function cursor_right() pome.move_cursor(direction.right, 1) end
function cursor_up()    pome.move_cursor(direction.up, 1)    end
function cursor_down()  pome.move_cursor(direction.down, 1)  end

function insert_newline() pome.insert_newlines(1) end

function cursor_line_start()
  pome.move_cursor_to(0, pome.get_line_num_cursor())
end

function delete_line()
  cursor_line_start();

  local y = pome.get_line_num_cursor();

  pome.set_anchor(0, y + 1);
  pome.delete_selected();
  pome.clear_anchor();
end
