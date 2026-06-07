#!/bin/sh
python tools/gen_header.py src/c_to_lua.c include/c_to_lua.h
python tools/gen_header.py src/editor.c include/editor.h

python tools/gen_header.py src/gap_buffer.c include/gap_buffer.h
python tools/gen_header.py src/file_handling.c include/file_handling.h

python tools/gen_header.py src/lua_vm.c include/lua_vm.h
python tools/gen_header.py src/render.c include/render.h
