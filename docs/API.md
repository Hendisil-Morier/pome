# `pome.*` API Reference

All functions live on the global `pome` table, which is populated by the Rust host at startup. They are the only way Lua can read or mutate editor state.

Coordinates are always **0-indexed**. `x` is the column, `y` is the line. When an `x` or `y` argument is marked optional, it defaults to the current cursor position.

---

## Cursor

### `pome.move_cursor(dir, times)`

Moves the cursor in the given direction by `times` steps.

- `dir` — one of `"left"`, `"right"`, `"up"`, `"down"`.
- `times` — a non-negative integer. Negative values are silently ignored.

Left/right movement operates on the absolute character offset and will cross line boundaries. Up/down movement preserves the column by converting to row/col space, computing the target row, and converting back. The cursor is clamped to the buffer bounds in all directions.

```lua
pome.move_cursor("right", 1)
pome.move_cursor("down", 5)
```

---

### `pome.move_cursor_to(x, y)`

Moves the cursor to an exact row/column coordinate.

Both `x` and `y` are clamped: `y` is clamped to the last line, and `x` is clamped to the length of the target line. Passing negative values is a no-op.

```lua
pome.move_cursor_to(0, 0)   -- beginning of file
pome.move_cursor_to(10, 3)  -- column 10 of line 3
```

---

### `pome.get_cursor_pos()`

Returns the current cursor position as two integers `(x, y)`.

This is derived from the internal absolute position by converting to row/col space, so it always reflects the true visual position.

```lua
local x, y = pome.get_cursor_pos()
```

---

## Editing

### `pome.insert_char(ch, x?, y?)`

Inserts a single character `ch` at the given position, or at the cursor if no position is provided.

If the insertion point is at or before the cursor, the cursor shifts right by one to stay on the same logical character. The operation is recorded to the undo history.

```lua
pome.insert_char('a')          -- inserts at cursor
pome.insert_char('\n', 0, 3)   -- inserts newline at start of line 3
```

---

### `pome.insert_string(text, x?, y?)`

Inserts a multi-character string at the given position, or at the cursor if no position is provided.

Behaves like `insert_char` but for an arbitrary string. The cursor shifts by the full character count of `text` if the insertion is at or before the cursor. Recorded as a single `Insert` edit in the undo history.

```lua
pome.insert_string("hello")
pome.insert_string("--[[", 0, 0)
```

---

### `pome.delete_after()`

Deletes the character at the cursor position (the character the cursor is sitting on).

Does nothing if the cursor is at or past the end of the buffer. The deleted character is recorded in the undo history.

```lua
pome.delete_after()
```

---

### `pome.delete_before()`

Deletes the character immediately before the cursor and moves the cursor one step left.

Does nothing if the cursor is at position 0. The deleted character is recorded in the undo history.

```lua
pome.delete_before()
```

---

### `pome.set_anchor(x?, y?)`

Sets the selection anchor at the given position, or at the current cursor position if no arguments are given, and enables selection mode.

The anchor is the fixed end of a selection; the cursor is the moving end. Both `pome.delete_selected()` and the rendering system use the anchor/cursor pair to determine the selected range.

```lua
pome.set_anchor()           -- anchor at cursor
pome.set_anchor(0, 5)       -- anchor at start of line 5
```

---

### `pome.clear_anchor()`

Disables selection mode without moving the cursor or the anchor value.

This does not erase the stored anchor position — calling `set_anchor` again will re-enable selection. It just sets the `selecting` flag to false, which makes `delete_selected` a no-op and removes the highlight from the renderer.

```lua
pome.clear_anchor()
```

---

### `pome.delete_selected()`

Deletes the text between the anchor and the cursor.

The range is `[min(anchor, cursor), max(anchor, cursor)]`, so direction does not matter. The cursor moves to the start of the deleted range. Does nothing if selection mode is not active or if anchor and cursor are at the same position. Recorded as a single `Delete` edit.

```lua
pome.set_anchor(0, 2)
pome.move_cursor_to(10, 2)
pome.delete_selected()
pome.clear_anchor()
```

---

## Search and Matching

All matching functions accept an optional `(x, y)` starting position. If omitted, the search starts from the current cursor position. They return `(x, y)` on a successful match, or `nil` if nothing is found.

---

### `pome.forward_match(ch, x?, y?)`

Scans forward from the starting position and returns the position of the first occurrence of the character `ch`.

The search includes the starting position itself. Returns `nil` if the character is not found before the end of the buffer.

```lua
local nx, ny = pome.forward_match('\n')  -- find next newline
```

---

### `pome.backward_match(ch, x?, y?)`

Scans backward from the starting position and returns the position of the nearest occurrence of `ch`.

The starting position is included in the search. Returns `nil` if the character is not found before the beginning of the buffer.

```lua
local nx, ny = pome.backward_match('\n')  -- find previous newline
```

---

### `pome.forward_match_set(set, x?, y?)`

Scans forward and returns the position of the first character that is a member of `set`.

`set` is a Lua table where keys are single-character strings and values are `true`. This is the same format produced by `stdlib.charset.make_charset`. The search includes the starting position.

```lua
local ws = require("stdlib.charset").whitespace
local nx, ny = pome.forward_match_set(ws)
```

---

### `pome.forward_match_notset(set, x?, y?)`

Scans forward and returns the position of the first character that is **not** a member of `set`.

Useful for skipping over a run of characters in the same class. The search includes the starting position.

```lua
local id = require("stdlib.charset").identifier
local nx, ny = pome.forward_match_notset(id)  -- end of current word
```

---

### `pome.backward_match_set(set, x?, y?)`

Scans backward from the starting position (inclusive) and returns the position of the nearest character that is a member of `set`.

```lua
local nx, ny = pome.backward_match_set(whitespace)
```

---

### `pome.backward_match_notset(set, x?, y?)`

Scans backward from the starting position (inclusive) and returns the position of the nearest character that is **not** a member of `set`.

```lua
local nx, ny = pome.backward_match_notset(whitespace)  -- skip whitespace going left
```

---

## Buffer Queries

### `pome.get_line_end(line?)`

Returns the number of characters on `line`, **excluding** the trailing newline.

If `line` is omitted, uses the current cursor line. Returns `nil` if `line` is negative or beyond the last line. This value is the maximum valid `x` coordinate for that line.

```lua
local len = pome.get_line_end()         -- current line length
local len = pome.get_line_end(0)        -- length of line 0
```

---

### `pome.char_at(x?, y?)`

Returns the character at the given position as a Lua string, or `nil` if the position is out of bounds.

Defaults to the cursor position if no arguments are given.

```lua
local ch = pome.char_at()              -- char under cursor
local ch = pome.char_at(0, 0)         -- first char in buffer
```

---

### `pome.get_max_line_index()`

Returns the 0-based index of the last line in the buffer.

Equivalent to `line_count - 1`. An empty buffer returns `0`.

```lua
local last = pome.get_max_line_index()
pome.move_cursor_to(0, last)  -- jump to last line
```

---

## History (Undo / Redo)

### `pome.undo()`

Undoes the last committed edit batch, restoring the buffer and cursor to their state before that batch.

If there is an open (uncommitted) batch, it is committed first, then immediately undone. Does nothing if there is no history to undo.

```lua
pome.undo()
```

---

### `pome.redo()`

Redoes the next edit batch in the undo stack, replaying the edits forward.

Does nothing if the stack pointer is already at the latest edit. Redo history is discarded whenever a new edit is recorded.

```lua
pome.redo()
```

---

### `pome.begin_undo_group()`

Starts an explicit undo group, forcing all subsequent edits into a single batch regardless of timing or contiguity.

Returns `true` if the group was started, `false` if one was already active. Commits the current open batch before starting the group, so the group always begins clean.

Use this to make a multi-step operation (e.g. delete a line and insert replacement text) appear as a single undo step.

```lua
pome.begin_undo_group()
pome.delete_after()
pome.insert_string("replacement")
pome.end_undo_group()
-- pressing undo reverts both operations at once
```

---

### `pome.end_undo_group()`

Ends the current explicit undo group and commits the accumulated batch.

Returns `true` if a group was active and was ended, `false` otherwise. After this call, normal time-based merging resumes.

---

### `pome.can_undo()`

Returns `true` if there is at least one edit that can be undone.

This includes both committed batches and any currently open (uncommitted) batch.

```lua
if pome.can_undo() then pome.undo() end
```

---

### `pome.can_redo()`

Returns `true` if there is at least one batch ahead of the current stack position that can be redone.

```lua
if pome.can_redo() then pome.redo() end
```

---

### `pome.set_undo_timeout(ms)`

Sets the time window in milliseconds within which consecutive compatible edits are automatically merged into one undo step.

The default is `200` ms. Set to `0` to disable automatic merging entirely (every keystroke becomes its own undo step). Set to a large value to merge aggressively.

```lua
pome.set_undo_timeout(500)  -- merge edits within half a second
pome.set_undo_timeout(0)    -- never auto-merge
```

---

## File and Application

### `pome.quit_editor()`

Signals the editor to stop. Sets the internal `running` flag to `false`, which causes `pome.is_running()` to return `false` on the next loop iteration, cleanly exiting `pome.main()`.

```lua
pome.quit_editor()
```

---

### `pome.save_file()`

Writes the current buffer to disk using the current filename.

Raises a Lua error if no filename has been set. Use `pome.set_filename` first if needed.

```lua
pome.save_file()
```

---

### `pome.get_filename()`

Returns the current filename as a string, or `nil` if no file is open.

```lua
local name = pome.get_filename() or "[No Name]"
```

---

### `pome.set_filename(path)`

Sets the filename used by `pome.save_file()`.

Does not open or load the file — it only updates the stored path. Useful for implementing a "save as" flow.

```lua
pome.set_filename("/home/user/notes.txt")
pome.save_file()
```

---

### `pome.get_config_dir()`

Returns the directory that contains the loaded config entry point, as an absolute string path.

This is used internally by `config/init.lua` to set up `package.path`, and is available to any Lua code that needs to load resources relative to the config.

```lua
local dir = pome.get_config_dir()
dofile(dir .. "/my_plugin.lua")
```

---

### `pome.set_config_dir(path)`

Overrides the stored config directory path.

Rarely needed in practice; primarily useful if you are programmatically relocating config resources at runtime.

---

### `pome.is_running()`

Returns `true` if the editor is still running, `false` if `quit_editor()` has been called.

This is the condition checked by `pome.main()`'s event loop.

```lua
while pome.is_running() do
    pome.render()
    local key = pome.next_key()
    if key then pome.dispatch_key(key) end
end
```

---

## Terminal and Rendering

### `pome.next_key()`

Blocks until the user presses a key, then returns a canonical key string.

Only press and repeat events are returned. Release events are silently discarded and the call loops internally. Returns `nil` for keys that are not recognized (function keys, media keys, etc.).

The returned string format is `"modifier+...+key"` with modifiers in alphabetical order (`alt`, `ctrl`, `shift`):

| Input | Returned string |
|---|---|
| `s` | `"s"` |
| `Ctrl+S` | `"ctrl+s"` |
| `Shift+G` | `"shift+G"` (capital letter) |
| `Alt+Left` | `"alt+arrow_left"` |
| `Enter` | `"enter"` |
| `Backspace` | `"backspace"` |
| `Delete` | `"delete"` |
| `Esc` | `"esc"` |

```lua
local key = pome.next_key()
if key == "ctrl+s" then pome.save_file() end
```

---

### `pome.get_term_size()`

Returns the current terminal dimensions as `(width, height)` in character cells.

The values reflect the live terminal size, so they change if the user resizes the window.

```lua
local w, h = pome.get_term_size()
```

---

### `pome.set_cursor_shape(shape)`

Sets the terminal cursor shape. Raises a Lua error for unrecognized shape names.

| Shape string | Appearance |
|---|---|
| `"block"` | Solid block, steady |
| `"block_blink"` | Solid block, blinking |
| `"bar"` | Thin vertical bar, steady |
| `"bar_blink"` | Thin vertical bar, blinking |
| `"underline"` | Underline, steady |
| `"underline_blink"` | Underline, blinking |

```lua
pome.set_cursor_shape("bar_blink")  -- typical insert mode cursor
pome.set_cursor_shape("block")      -- typical normal mode cursor
```

---

### `pome.draw_panels(panels)`

Renders one full frame by drawing each panel in the `panels` array in order.

`panels` is a Lua array of panel descriptor tables. Two panel types are supported:

**Buffer panel** — renders the text buffer with scrolling, tab expansion, selection highlight, and the cursor.

```lua
{
    type       = "buffer",
    rect       = { x = 0, y = 0, width = 80, height = 23 },
    row_offset = 0,    -- first line to display (vertical scroll)
    tab_width  = 4,    -- spaces per tab stop, default 4
}
```

The `cursor` field is injected automatically by the Rust host from the current `CursorInfo`; do not set it manually.

**Text panel** — renders a plain string in a colored rectangle. Useful for status bars, prompts, and overlays.

```lua
{
    type    = "text",
    rect    = { x = 0, y = 23, width = 80, height = 1 },
    content = " INSERT | file.txt | Ln 4, Col 12 ",
    bg      = "DarkGray",   -- named color or {r, g, b} triple
    fg      = "White",
}
```

Named colors map directly to ratatui's `Color` enum (`"Black"`, `"Red"`, `"Green"`, `"Yellow"`, `"Blue"`, `"Magenta"`, `"Cyan"`, `"Gray"`, `"DarkGray"`, `"LightRed"`, etc.). RGB triples are written as a Lua array: `{255, 128, 0}`.

Panels are drawn in array order; later panels paint over earlier ones, which is useful for overlays.

```lua
pome.draw_panels({
    { type = "buffer", rect = buf_rect, row_offset = scroll },
    { type = "text",   rect = bar_rect, content = status, bg = "DarkGray", fg = "White" },
})
```
