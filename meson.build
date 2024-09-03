project(
  'ThetisSkinMaker',
  ['c', 'cpp']
)

cc = meson.get_compiler('c')

windows = import('windows')

win32skinmaker_sources = files(
  'main.c',
  'window.c',
  'preview.c',
  'image.cpp',
)

win32skinmaker_deps = [
  cc.find_library('shlwapi'),
  cc.find_library('gdiplus'),
]

win32skinmaker_sources += windows.compile_resources('resources.rc')

executable(
  meson.project_name(),
  win32skinmaker_sources,
  dependencies: win32skinmaker_deps,
  c_args: [
    '-DUNICODE',
    '-D_UNICODE',
  ],
  win_subsystem: 'windows,6.0'
)
