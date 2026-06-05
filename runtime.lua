modes = {}

function define_mode(name, config)
    modes[name] = config
end

function bind(f, ...)
  local args = {...}
  return function() f(table.unpack(args)) end
end

function inherit(extra, base)
  return setmetatable(extra, {__index = base})
end
