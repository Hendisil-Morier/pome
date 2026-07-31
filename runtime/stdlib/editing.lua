-- stdlib/editing.lua
-- Basic editing operations that combine cursor movement with Pome API calls
local motion = require("stdlib.motion")
local mode = require("stdlib.mode")

-- We'll use motion.cursor_line_start, but we can also use other helpers if needed
-- (like motion.move_cursor_to, motion.pos_next, etc.)

local M = {}

-- Delete the entire current line
function M.delete_line()
    local _, y = pome.get_cursor_pos()
    local max = pome.get_max_line_index()
    local target_y
    local target_x
    
    if y == max then
        -- last line, just delete the entire thing
        target_x = pome.get_line_end(y)
        target_y = y
    else
        target_x = 0
        target_y = y+1
    end

    pome.delete_range(0, y, target_x, target_y)
end

-- Insert a newline at the cursor (splits the line)
function M.insert_newline()
  pome.insert_char('\n')
end

-- Simple character deletion (forward and backward)
-- either wrap the Pome API directly or add small safety checks.
function M.delete_before_cursor()
  pome.delete_before()
end

function M.delete_after_cursor()
  pome.delete_after()
end

-- Insert a character at the cursor
function M.insert_char(ch)
  pome.insert_char(ch)
end

-- delete to end of line
function M.delete_to_line_end()
  local x, y = pome.get_cursor_pos()
  local line_end = pome.get_line_end(y)
  if not line_end then return end
  if x == line_end then
    -- Nothing to delete
    return
  end
  pome.set_anchor(x, y)
  motion.move_cursor_to(line_end, y)
  pome.delete_selected()
  pome.clear_anchor()
end

-- add newline and enter insert mode
-- sidestepping moving cursor 'cause it would cause
-- redo/undo misbehave
function M.openline(shift)
    local _, y = pome.get_cursor_pos()

    if shift ~= nil and shift ~= false then
        -- open line above
        pome.insert_char('\n', 0, y)
        motion.move_cursor_to(0, y)
        mode.safe_set_mode("insert")
        return
    end
    
    -- open line below
    local next_y = y + 1
    local max = pome.get_max_line_index()

    -- if at the last line
    if next_y > max then
        local line_end = pome.get_line_end(y)
        motion.move_cursor_to(line_end, y)
        pome.insert_char('\n')
    else
        pome.insert_char('\n', 0, next_y)
        motion.move_cursor_to(0, next_y)
    end
    
    mode.safe_set_mode("insert")
end


return M
