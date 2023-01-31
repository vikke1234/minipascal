project('minipascal', 'cpp',
  version : '0.1',
  default_options : ['warning_level=3', 'cpp_std=c++17', 'werror=true'])

sources = ['minipascal.cpp', 'lexer.cpp', 'parser.cpp']

exe = executable('minipascal', sources,
  install : true)

test('basic', exe)
