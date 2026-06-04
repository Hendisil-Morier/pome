#include "termbox2.h"
#include "gap_buffer.h"
#include "editor.h"
#include "render.h"
#include "file_handling.h"
#include "lua_vm.h"

int main(int argc, char* argv[])
{
  struct tb_event ev;
  Editor editor = {};
  editor.buffer = init_gap_buffer();

  editor.current_mode = nullptr;
  editor.pre_mode = nullptr;

  tb_init();

  //editor setup

  editor.term_stat.width = tb_width();
  editor.term_stat.height = tb_height();

  editor.filename = (argc > 1) ? argv[1] : nullptr;
  editor.running = true;

  //lua set up
  init_lua(&editor);
  register_primitives(editor.lua);
  register_constant(editor.lua);

  //load config
  lua_load_config(editor.lua, "init.lua");

  load_file(&editor);
  move_gap_to(editor.buffer, 0);


  while(editor.running)
  {
    render_char(&editor);

    tb_poll_event(&ev);

    if(ev.type == TB_EVENT_KEY)
      process_key(&ev, &editor);

    if (ev.type == TB_EVENT_RESIZE)
    {
      editor.term_stat.width = tb_width();
      editor.term_stat.height = tb_height();
    }

    update_scroll(&editor);
  }

  tb_shutdown();
  lua_close(editor.lua);
  free_gb(editor.buffer);
  return 0;
}
