-- config/runtime.lua

-- Global mode registry
function _G.define_mode(name, config)
    engine.modes[name] = config
end

-- Helpers
function _G.bind(f, ...)
    local args = { ... }
    return function() f(table.unpack(args)) end
end

function _G.inherit(extra, base)
    return setmetatable(extra, { __index = base })
end

-- Import the key builder
_G.key = require("key_builder")