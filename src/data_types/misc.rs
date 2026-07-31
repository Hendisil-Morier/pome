
pub enum Direction
{
	Left,
	Right,
	Up,
	Down,
}

#[derive(Default, Clone, Copy)]
pub struct Position
{
	pub x: usize,
	pub y: usize,
}

impl From<(usize, usize)> for Position
{
    fn from((x, y): (usize, usize)) -> Self
    {
        return Position{ x, y};
    }
}

#[derive(Default, Clone)]
pub struct CursorInfo
{
	pub abs_pos: usize,
	pub anchor: Option<usize>,
	pub selection_mode: Option<SelectionMode>,
	pub selecting: bool,
}

#[derive(Clone, Copy, PartialEq, Eq)]
pub enum SelectionMode
{
    Char,
    Line
}