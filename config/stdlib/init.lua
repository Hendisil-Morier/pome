-- stdlib/init.lua
-- Aggregates all stdlib modules into one table for easy import

local charset = require("stdlib.charset")
local motion  = require("stdlib.motion")
local editing = require("stdlib.editing")
local mode    = require("stdlib.mode")
local layout  = require("stdlib.layout")
local state   = require("stdlib.state")
local register = require("stdlib.register")

-- when writing stuff inside this stdlib module, require("stdlib.[stuff]") directly
-- doing otherwise may cause dependency loop

return
{
    charset = charset,

    motion = motion,
    
    editing = editing,
    
    mode = mode,
    
    layout = layout,
  
    register = register,
  
    state = state,
}
