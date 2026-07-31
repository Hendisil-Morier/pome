-- stdlib/register.lua
-- Basic implementation of the register system in vim

local state = require("stdlib.state")
local motion = require("stdlib.motion")
local editing = require("stdlib.editing")

local M = {}
local store = {}

function M.yank()
  local text = pome.get_selected()
  
  -- Read the current register dynamically, default to '"'
  local current_register = state.register or '"'
  
  if text then
    store[current_register] = text
  end
end

function M.paste()
    local current_register = state.register or '"'
    local txt = store[current_register]
    if not txt or (txt == "") then return end;
        
    if txt:sub(-1) == '\n' then
        -- strip off the \n, dont know if it neccessary or not
        txt = txt:sub(1, -2) 
        motion.cursor_line_end()
        editing.insert_newline()
    end
    
    pome.insert_string(txt)
end

return M
