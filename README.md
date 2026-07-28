# pome

A terminal text editor where the editing logic lives entirely in Lua.

The Rust core is mode-agnostic — it exposes primitives for cursor movement, text mutation, rendering, and file I/O. Lua composes those primitives into a full modal editor with keymaps, modes, and the event loop itself. You can redefine or replace the entire editing system without touching Rust.

## AI Usage

I used AI, yes. This README, the API reference document and mostly the Lua core's code. Except for this AI usage section I guess.

My reason for this blasphemy: I dont have much experience in Lua or writing documents. This project was worked on in my free time and I want something presentable. For now.

Any help to atone my sins is always welcome.

## Features

- Modal editing (normal, insert, visual) defined purely in Lua
- Vim-inspired keybindings out of the box
- Word motions (`w`, `b`) with character-class awareness
- Undo/redo with time-based batch merging and explicit grouping
- Hot-reload config at runtime (`Ctrl+r` or `space e r`)
- Extensible panel-based rendering (buffer view, status bar)
- Configurable cursor shape per mode

## Building

Requires the Rust toolchain (2024 edition).

```sh
cargo build --release
```

## Usage

```sh
# Open a file
./target/release/pome myfile.txt

# Use a custom config entry point
./target/release/pome --config path/to/init.lua myfile.txt
```

If no file is given, pome opens an empty buffer.

## Default Keybindings

### Universal (all modes)

| Key | Action |
|---|---|
| Arrow keys | Move cursor |
| `Esc` | Return to normal mode |
| `Ctrl+s` | Save |
| `Ctrl+q` | Quit |
| `Ctrl+r` | Reload config |

### Normal mode

| Key | Action |
|---|---|
| `h` `j` `k` `l` | Left, down, up, right |
| `w` / `b` | Word forward / backward |
| `i` | Enter insert mode |
| `v` | Enter visual mode |
| `o` | Open line below |
| `G` | Go to last line |
| `gg` | Go to first line |
| `dd` | Delete line |
| `u` / `U` | Undo / redo |
| `space q q` | Quit |
| `space e r` | Reload config |

### Insert mode

| Key | Action |
|---|---|
| Type normally | Insert characters |
| `Backspace` | Delete before cursor |
| `Delete` | Delete after cursor |
| `Enter` | Insert newline |

### Visual mode

| Key | Action |
|---|---|
| `h` `j` `k` `l` | Extend selection |
| `d` | Delete selection |
| `v` | Return to normal mode |

## Configuration

Check this out for the entire thing (or so I hope) **[Lua API Reference](docs/API.md)**

The config directory (`config/`) is a Lua package loaded at startup. The entry point is `config/init.lua`. You can point pome at your own config directory with `--config`.

A mode is just a Lua table:

```lua
define_mode("my_mode", {
    keymap = {
        [key.x] = function() pome.delete_after() end,
    },
    default = function(ch) pome.insert_char(ch) end,
    on_enter = function() pome.set_cursor_shape("bar") end,
    on_exit  = function() end,
    minor = false, -- true makes it auto-exit after one key
})
```

The full `pome.*` API exposes cursor movement, text insertion and deletion, search, undo grouping, rendering, and terminal control.

## Not yet implemented

These are known gaps in the current version.

- **Syntax highlighting** — no tokenizer or highlight infrastructure exists yet
- **Line numbers** — not rendered
- **Search** — no `/` forward search or `?` backward search
- **Yank and paste** — no register system; visual delete does not copy to a clipboard
- **Count prefixes** — `5j`, `3w`, etc. are not supported
- **Multiple buffers** — only one file can be open at a time
- **Split windows** — the panel system supports arbitrary rects but no built-in split management
- **Mouse support** — click to move cursor, scroll, etc.
- **Theming** — selection highlight is hardcoded blue; no color scheme system

## Dependencies

| Crate | Purpose |
|---|---|
| `mlua` | Embedded Lua 5.5 runtime |
| `ratatui` | TUI rendering |
| `crossterm` | Terminal input and cursor control |
| `ropey` | Rope data structure for the text buffer |
| `serde` | Panel layout deserialization |
