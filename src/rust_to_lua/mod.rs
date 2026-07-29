pub mod cursor;
pub mod editing;
pub mod history;
pub mod matching;
pub mod file_app;
pub mod misc;

pub use cursor::*;
pub use editing::*;
pub use history::*;
pub use matching::*;
pub use file_app::*;
pub use misc::*;


#[macro_export]
macro_rules! validate_usize
{
    ($($number: expr),+ $(,)?) =>
    {
        true $(&& $number >= 0) +
    };
}
