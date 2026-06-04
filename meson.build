project('pome', 'c', default_options: ['c_std=c23'])

cc = meson.get_compiler('c')

lua = dependency('lua5.5')
m = cc.find_library('m')

sources = files(run_command('find', 'src', '-name', '*.c', check: true).stdout().strip().split('\n'))

executable('pome',
  sources,
  dependencies: [lua, m],
  include_directories: include_directories('include'),
  c_args: ['-Wextra', '-Wall', '-pedantic', '-g', '-O0'],
  build_by_default: true)
