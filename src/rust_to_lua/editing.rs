use mlua::Lua;
use crate::data_types::misc::{Position, SelectionMode};
use crate::helpers::{*};
use crate::{get_editor, validate_usize};

pub fn lua_insert_char(lua: &Lua,
  (ch, x, y): (char, Option<i64>, Option<i64>)) 
-> mlua::Result<()>
{
    get_editor!(mut editor from lua);
    let cur_pos = editor.cursor_pos();

    let x = x.unwrap_or(cur_pos.x as i64);
    let y = y.unwrap_or(cur_pos.y as i64);

    if !validate_usize!(x, y)
    {
        lua.warning("insert_char: negative cordinate, ignoring", false);
        return Ok(());
    }

    let pos = Position{x: x as usize, y: y as usize};

    editor.insert_char_at(ch, pos);
    return Ok(());
}

pub fn lua_insert_string(lua: &Lua,
  (text, x, y): (String, Option<i64>, Option<i64>)) 
-> mlua::Result<()>
{
    get_editor!(mut editor from lua);
    let cur_pos = editor.cursor_pos();

    let x = x.unwrap_or(cur_pos.x as i64);
    let y = y.unwrap_or(cur_pos.y as i64);

    if !validate_usize!(x, y)
    {
        lua.warning("insert_string: negative cordinate, ignoring", false);
        return Ok(());
    }

    let pos = Position{x: x as usize, y: y as usize};

    editor.insert_string_at(text, pos);
    return Ok(());
}

pub fn lua_set_anchor(lua: &Lua,
    (mode, x, y): (Option<String>, Option<i64>, Option<i64>))
-> mlua::Result<()>
{
    get_editor!(mut editor from lua);

    let cur_pos = editor.cursor_pos();
    let mode = mode.unwrap_or_else( || "char".to_string() );
    
    let anchor_x = x.unwrap_or(cur_pos.x as i64);
    let anchor_y = y.unwrap_or(cur_pos.y as i64);

    if !validate_usize!(anchor_x, anchor_y)
    {
        lua.warning("set_anchor: negative cordinate, ignoring", false);
        return Ok(());
    }
    
    let sel_mode = match mode.to_lowercase().as_str()
    {
        "line" => SelectionMode::Line,
        _ => SelectionMode::Char,
    };

    let anchor_pos = Position{x: anchor_x as usize, y : anchor_y as usize};

    let abs_pos = editor.repos_to_abspos(anchor_pos);
    editor.set_anchor(abs_pos, sel_mode);

    return Ok(());
}

pub fn lua_clear_anchor(lua: &Lua, _: ())
-> mlua::Result<()>
{
    get_editor!(mut editor from lua);

    editor.clear_anchor();

    return Ok(());
}

pub fn lua_delete_range(lua: &Lua, (x1, y1, x2, y2): (i64, i64, i64, i64))
-> mlua::Result<Option<String>>
{
    if !validate_usize!(x1, x2, y1, y2)
    {
        lua.warning("delete_range: negative cordinate, ignoring", false);
        return Ok(None);
    }
    let (x1, x2, y1, y2) = (x1 as usize, x2 as usize, y1 as usize, y2 as usize);
    
    get_editor!(mut editor from lua);
    let start = editor.repos_to_abspos( (x1, y1).into() );
    let end = editor.repos_to_abspos( (x2, y2).into() );
    
    let removed = editor.delete_range((start, end));
    
    return Ok(removed);
}

pub fn lua_delete_selected(lua: &Lua, _: ())
-> mlua::Result<Option<String>>
{
    get_editor!(mut editor from lua);

    let removed = editor.delete_selected();

    return Ok(removed);
}

pub fn lua_get_selected(lua: &Lua, _: ())
-> mlua::Result<Option<String>>
{
    get_editor!(editor from lua);

    let selected = editor.get_selected();

    return Ok(selected);
}

pub fn lua_delete_after(lua: &Lua, _: ())
-> mlua::Result<()>
{
    get_editor!(mut editor from lua);
    editor.delete_after();
    return Ok(());
}

pub fn lua_delete_before(lua: &Lua, _: ())
-> mlua::Result<()>
{
    get_editor!(mut editor from lua);
    editor.delete_before();
    return Ok(());
}
