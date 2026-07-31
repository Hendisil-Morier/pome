-- core/modes.lua

_G.engine = {}
engine.modes = {}
engine.mode_state = {
    cur_mode    = nil,
    prev_mode   = nil,
    pending_seq = "",
    change_count = 0,
}

function engine.get_current_mode()
    return engine.mode_state.cur_mode
end

function engine.get_mode_table(mode_name)
    local t = engine.modes[mode_name]
    if type(t) == "table" then return t end
    return nil
end

function engine.set_mode(mode_name, ...)
    local old = engine.mode_state.cur_mode
    if old then engine.call_mode_hook(old, "on_exit") end
    
    engine.mode_state.cur_mode = mode_name
    engine.mode_state.change_count = engine.mode_state.change_count + 1
    engine.mode_state.pending_seq = ""
    
    local mode_table = engine.get_mode_table(mode_name)
    engine.mode_state.sequences = mode_table and mode_table.sequences or nil
    
    engine.call_mode_hook(mode_name, "on_enter", ...)
end

function engine.save_mode(mode_name)
    engine.mode_state.prev_mode = mode_name
end

function engine.restore_mode()
    local prev = engine.mode_state.prev_mode
    if not prev then return end
    
    engine.mode_state.prev_mode = nil
    engine.set_mode(prev)
end

function engine.is_minor_mode(mode_name)
    local mt = engine.get_mode_table(mode_name)
    return mt and not not mt.minor
end

function engine.call_mode_hook(mode_name, hook_name, ...)
    local mt = engine.get_mode_table(mode_name)
    if not mt then return end
    
    local fn = mt[hook_name]
    if type(fn) ~= "function" then return end
    
    pcall(fn, ...)
end
