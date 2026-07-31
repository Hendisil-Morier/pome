-- core/keys.lua

local function is_in_table(tbl, key)
    for k, _ in pairs(tbl) do
        if type(k) ~= "string" or #k <= #key then goto continue end
        
        local prefix = k:sub(1, #key)
        local next_char = k:sub(#key+1, #key+1)
        
        if prefix == key and next_char == " " then
            return true
        end
        
        ::continue::
    end
    return false
end

function engine.call_keymap(key_str)
    local cur = engine.mode_state.cur_mode
    if not cur then return false end
    
    local mt = engine.get_mode_table(cur)
    if not mt or type(mt.keymap) ~= "table" then return false end
    
    local fn = mt.keymap[key_str]
    if type(fn) ~= "function" then return false end
    
    local ok, _ = pcall(fn)
    return ok
end

function engine.call_default(ch)
    local cur = engine.mode_state.cur_mode
    if not cur then return false end
    
    local mt = engine.get_mode_table(cur)
    if not mt or type(mt.default) ~= "function" then return false end
    
    local ok, _ = pcall(mt.default, ch)
    return ok
end

function engine.process_sequences(key_str)
    local key_seqs = (key_str == " ") and "space" or key_str
    local state = engine.mode_state
    local sequences = state.sequences
    
    if type(sequences) ~= "table" then
        state.pending_seq = ""
        return false
    end
    
    if state.pending_seq == "" then
        state.pending_seq = key_seqs
    else
        state.pending_seq = state.pending_seq .. " " .. key_seqs
    end
    
    local pending = state.pending_seq
    local fn = sequences[pending]
    
    if type(fn) == "function" then
        pcall(fn)
        state.pending_seq = ""
        return true
    end
    
    if is_in_table(sequences, pending) then
        return true
    end
    
    state.pending_seq = ""
    return false
end

function engine.dispatch_key(key_str)
    local change_before = engine.mode_state.change_count
    
    local handled = engine.process_sequences(key_str)
    if not handled then handled = engine.call_keymap(key_str) end
    if not handled then handled = engine.call_default(key_str) end
    
    if change_before ~= engine.mode_state.change_count then return end
    
    local cur_mode = engine.mode_state.cur_mode
    if not cur_mode then return end
    
    local cur_minor = engine.is_minor_mode(cur_mode)
    local prev = engine.mode_state.prev_mode
    local saved_major = prev and not engine.is_minor_mode(prev)
    
    if cur_minor and saved_major then
        engine.restore_mode()
    end
end
