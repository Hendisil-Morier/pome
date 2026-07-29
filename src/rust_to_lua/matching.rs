use mlua::Lua;
use crate::helpers::{*};
use crate::{get_editor, validate_usize};

pub fn lua_forward_match(lua: &Lua,
	(matcher, from_x, from_y): (char, Option<i64>, Option<i64>))
-> mlua::Result<mlua::MultiValue>
{
    get_editor!(editor from lua);

    let cur_pos = editor.cursor_pos();
    let from_x = from_x.unwrap_or(cur_pos.x as i64);
    let from_y = from_y.unwrap_or(cur_pos.y as i64);

    if !validate_usize!(from_x, from_y)
    {
        lua.warning("forward_match: negative cordinate, ignoring", false);

        let nil = mlua::MultiValue::from_iter([mlua::Value::Nil]);
        return Ok(nil);
    }

    let repos = (from_x as usize, from_y as usize).into();

    let from_abs = editor.repos_to_abspos(repos);

    match editor.forward_match(from_abs, matcher)
    {
        Some(abs_result) =>
        {
            let pos = editor.abspos_to_repos(abs_result);
            let iter_result = [
                mlua::Value::Integer(pos.x as i64),
                mlua::Value::Integer(pos.y as i64),
            ];
            let result = mlua::MultiValue::from_iter(iter_result);
            return Ok(result);
        },

        None =>
        {
            let result = mlua::MultiValue::from_iter([mlua::Value::Nil]);
            return Ok(result);
        }
    }
}

pub fn lua_backward_match(lua: &Lua,
	(matcher, from_x, from_y): (char, Option<i64>, Option<i64>))
-> mlua::Result<mlua::MultiValue>
{
    get_editor!(editor from lua);

    let cur_pos = editor.cursor_pos();
    let from_x = from_x.unwrap_or(cur_pos.x as i64);
    let from_y = from_y.unwrap_or(cur_pos.y as i64);

    if !validate_usize!(from_x, from_y)
    {
        lua.warning("backward_match: negative cordinate, ignoring", false);

        let nil = mlua::MultiValue::from_iter([mlua::Value::Nil]);
        return Ok(nil);
    }

    let repos = (from_x as usize, from_y as usize).into();

    let from_abs = editor.repos_to_abspos(repos);

    match editor.backward_match(from_abs, matcher)
    {
        Some(abs_result) =>
        {
            let pos = editor.abspos_to_repos(abs_result);
            let iter_result = [
                mlua::Value::Integer(pos.x as i64),
                mlua::Value::Integer(pos.y as i64),
            ];
            let result = mlua::MultiValue::from_iter(iter_result);
            return Ok(result);
        },

        None =>
        {
            let result = mlua::MultiValue::from_iter([mlua::Value::Nil]);
            return Ok(result);
        }
    }
}

fn match_set_impl(lua: &Lua,
    (charset, from_x, from_y): (mlua::Table, Option<i64>, Option<i64>),
    forward: bool, want_in_set: bool)
-> mlua::Result<mlua::MultiValue>
{
    get_editor!(editor from lua);
    let cur_pos = editor.cursor_pos();

    let from_x = from_x.unwrap_or(cur_pos.x as i64);
    let from_y = from_y.unwrap_or(cur_pos.y as i64);
    let result_pos;

    if !validate_usize!(from_x, from_y)
    {
        let dir = if forward {"forward"} else {"backward"};
        let inset = if want_in_set {"set"} else {"notset"};
        let msg = format!("{dir}_match_{inset}: negative cordinate, ignoring");
        let result = [mlua::Value::Nil];

        lua.warning(msg, false);

        return Ok(mlua::MultiValue::from_iter(result));
    }

    let repos = (from_x as usize, from_y as usize).into();

    let from_abs = editor.repos_to_abspos(repos);

    let logic_len = editor.buffer.len_chars();
    let mut i = from_abs;
    loop
    {
        let cond;
        if forward
        {cond = i >= logic_len;}
        else
        {cond = i == usize::MAX;}

        if cond  {break;}

        let c;
        if let Some(t) = editor.buffer.get_char(i)
        {c = t;}
        else {break;}

        let in_set = charset.contains_key(c)?;

        if in_set == want_in_set
        {
            result_pos = editor.abspos_to_repos(i);
            let result = [
            mlua::Value::Integer(result_pos.x as i64),
            mlua::Value::Integer(result_pos.y as i64),
            ];
            return Ok(mlua::MultiValue::from_iter(result));
        }

        if forward  {i+=1;}
        else
        {
            if i == 0  {break;}
            i-=1;
        }
    }

    let result = [mlua::Value::Nil];
    return Ok(mlua::MultiValue::from_iter(result));
}

pub fn lua_forward_match_set(lua: &Lua,
    (charset, from_x, from_y): (mlua::Table, Option<i64>, Option<i64>))
-> mlua::Result<mlua::MultiValue>
{
    return match_set_impl(lua, (charset, from_x, from_y), true, true);
}

pub fn lua_backward_match_set(lua: &Lua,
    (charset, from_x, from_y): (mlua::Table, Option<i64>, Option<i64>))
-> mlua::Result<mlua::MultiValue>
{
    return match_set_impl(lua, (charset, from_x, from_y), false, true);
}

pub fn lua_forward_match_notset(lua: &Lua,
    (charset, from_x, from_y): (mlua::Table, Option<i64>, Option<i64>))
-> mlua::Result<mlua::MultiValue>
{
    return match_set_impl(lua, (charset, from_x, from_y), true, false);
}

pub fn lua_backward_match_notset(lua: &Lua,
        (charset, from_x, from_y): (mlua::Table, Option<i64>, Option<i64>))
-> mlua::Result<mlua::MultiValue>
{
    return match_set_impl(lua, (charset, from_x, from_y), false, false);
}
