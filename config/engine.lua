-- engine.lua

require("core.modes")
require("core.keys")

local layout = require("stdlib.layout")

local row_offset = 0

function pome.render()
    local w, h = pome.get_term_size()
    local screen = { x=0, y=0, width=w, height=h }
    
    local buf_rect, bar_rect = layout.hsplit(screen, h - 1)

    local cx, cy = pome.get_cursor_pos()
    row_offset = layout.compute_scroll(row_offset, cy, buf_rect.height)

function pome.save_mode(mode_name)
    pome.mode_state.prev_mode = mode_name
end

function pome.restore_mode()
    local prev = pome.mode_state.prev_mode
    if prev then
        pome.mode_state.prev_mode = nil
        pome.set_mode(prev)
    end
end

function pome.is_minor_mode(mode_name)
    local mt = get_mode_table(mode_name)
    return mt and not not mt.minor
end

function pome.call_mode_hook(mode_name, hook_name)
    local mt = get_mode_table(mode_name)
    if mt and type(mt[hook_name]) == "function" then
        pcall(mt[hook_name])
    end
end

function pome.call_keymap(key_str)
    local cur = pome.mode_state.cur_mode
    if not cur then return false end
    local mt = get_mode_table(cur)
    if not mt then return false end
    local km = mt.keymap
    if type(km) ~= "table" then return false end
    local fn = km[key_str]
    if type(fn) ~= "function" then return false end
    local ok, _ = pcall(fn)
    return ok
end

function pome.call_default(ch)
    local cur = pome.mode_state.cur_mode
    if not cur then return false end
    local mt = get_mode_table(cur)
    if not mt then return false end
    local df = mt.default
    if type(df) ~= "function" then return false end
    local ok, _ = pcall(df, ch)
    return ok
end

function pome.process_sequences(key_str)
    local key_seqs = (key_str == " ") and "space" or key_str
    local state = pome.mode_state
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
    else
        if is_in_table(sequences, pending) then
            return true
        else
            state.pending_seq = ""
            return false
        end
    end
end

function pome.dispatch_key(key_str)
    local change_before = pome.mode_state.change_count
    local handled = pome.process_sequences(key_str)
    if not handled then handled = pome.call_keymap(key_str) end
    if not handled then handled = pome.call_default(key_str) end
    local unchanged = (change_before == pome.mode_state.change_count)
    if unchanged and pome.mode_state.cur_mode then
        local cur_minor = pome.is_minor_mode(pome.mode_state.cur_mode)
        local prev = pome.mode_state.prev_mode
        local saved_major = prev and not pome.is_minor_mode(prev)
        if cur_minor and saved_major then
            pome.restore_mode()
        end
    end
end

function pome.statusline()
    local mode = pome.mode_state.cur_mode or "?"
    local fname = pome.get_filename() or "[No Name]"
    local mode  = pome.mode_state.cur_mode or "?"
    local status = string.format(" %s | %s | Ln %d, Col %d ", mode, fname, cy+1, cx+1)

    local text_buffer_panel = {
        type = "buffer",
        rect = buf_rect,
        row_offset = row_offset,
        tab_width = 4,
    }

    local status_line_panel
    if pome.mode_state.cur_mode == "command" then
        local cmd_config = pome.modes["command"]
        local text = cmd_config.get_text and cmd_config.get_text() or ""
        
        status_line_panel = {
            type = "text",
            rect = bar_rect,
            content = ":" .. text,
            bg = "DarkGray",
            fg = "White",
            cursor = { #text + 1, 0 },
        }
    else
        status_line_panel = {
            type = "text",
            rect = bar_rect,
            content = status,
            bg = "DarkGray",
            fg = "White",
        }
    end

    pome.draw_panels({ text_buffer_panel, status_line_panel })
end

function pome.main()
    while pome.is_running() do
        pcall(pome.render)

        local ok, err = pcall(function()
            local key = pome.next_key()
            if key then
                pome.dispatch_key(key)
            end
        end)
        
        if not ok then
            local w, h = pome.get_term_size()
            local error_report_panel = {
                type = "text",
                rect = { x=0, y=h-1, width=w, height=1 },
                content = "ERROR: " .. tostring(err),
                bg = "Red",
                fg = "White",
            }
            pome.draw_panels({ error_report_panel })
            pome.next_key()
        end
    end
end