
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
	pub selecting: bool,
}
