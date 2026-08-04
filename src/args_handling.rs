use std::path::PathBuf;

use crate::embedded::RuntimeAssets;

pub struct ParsedArgs
{
    pub filename: Option<PathBuf>,
    pub config_file: PathBuf,
    pub config_dir: PathBuf,
}

pub fn parse_arguments(args: Vec<String>) -> Result<ParsedArgs, String>
{
    let mut filename: Option<PathBuf> = None;
    let mut config_path: Option<String> = None;

    let mut i = 1;
    while i < args.len()
    {
        if args[i] == "--config" || args[i] == "-c"
        {
            i += 1;
            if i >= args.len()
            {
                return Err(format!("missing argument for {}", args[i - 1]));
            }
            config_path = Some(args[i].clone());
        }
        else if filename.is_none()
        {
            filename = Some(PathBuf::from(&args[i]));
        }

        i += 1;
    }
    let hard_coded_dir = PathBuf::from("runtime/init.lua");
    let config_file = match config_path
        {
            Some(p) => PathBuf::from(p),
            None    => resolve_runtime().unwrap_or( hard_coded_dir),
        };

    let config_file = std::fs::canonicalize(&config_file)
        .unwrap_or(config_file);

    // On Windows, canonicalize() returns a \\?\-prefixed UNC path.
    // Lua uses '?' as the module name placeholder in package.path, so the
    // prefix corrupts path strings passed to Lua. Strip it if present.
    let config_file = strip_unc_prefix(config_file);

    let parent = config_file.parent();
    
    let config_dir = match parent
        {
            Some(p) if !p.as_os_str().is_empty() => p.to_path_buf(),
            _ => PathBuf::from("."),
        };
    
    return Ok(ParsedArgs { filename, config_file, config_dir });
}

fn extract_embedded_runtime()
->Option<PathBuf>
{
    let base_dir = dirs::config_dir()?.join("pome/runtime");

    eprintln!("warning: no runtime found, extracting defaults at {}", base_dir.display());

    for file_path in RuntimeAssets::iter()
    {
        let dest = base_dir.join(file_path.as_ref());

        std::fs::create_dir_all( dest.parent()? ).ok()?;

        let data = RuntimeAssets::get( file_path.as_ref() )?;

        std::fs::write(&dest, data.data).ok()?;
    }

    return Some(base_dir.join("init.lua"));
}

fn resolve_runtime()
-> Option<PathBuf>
{
    //check the config_dir
    if let Some(cfg) = dirs::config_dir()
    {
        let p = cfg.join("pome/runtime/init.lua");
        if p.exists() {return Some(p);}
    }

    //if failed, check exe dir
    if let Ok(exe) = std::env::current_exe()
       && let Some(parent) = exe.parent()
    {
        let p = parent.join("runtime/init.lua");
        if p.exists() {return Some(p);}
    }

    //all failed, generate one
    return extract_embedded_runtime();
}

/// Strip the `\\?\` extended-length prefix that `canonicalize` adds on Windows.
/// On other platforms this is a no-op.
fn strip_unc_prefix(path: PathBuf) -> PathBuf
{
    #[cfg(windows)]
    {
        let s = path.to_string_lossy();
        if let Some(stripped) = s.strip_prefix(r"\\?\")
        {
            return PathBuf::from(stripped);
        }
    }
    return path;
}
