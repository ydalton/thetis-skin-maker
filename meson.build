project(
  'ThetisSkinMaker',
  'c',
  version: 'v0.1.0-alpha',
  default_options: {
    'c_std': 'c89',
    'warning_level': '3',
    'werror': 'true',
  },
)

cc = meson.get_compiler('c')

windows = import('windows')

win32skinmaker_sources = files(
  'image.c',
  'main.c',
  'preview.c',
  'skin.c',
  'window.c',
)

win32skinmaker_deps = [
  cc.find_library('shlwapi'),
  cc.find_library('gdiplus'),
]

win32skinmaker_sources += windows.compile_resources(
  'resources.rc',
  args: [
    # windres seems to have a bug, this seems to fix it
    '--use-temp-file',
    '-DVERSION="' + meson.project_version() + '"',
  ],
)

executable(
  meson.project_name(),
  win32skinmaker_sources,
  dependencies: win32skinmaker_deps,
  c_args: [
    '-DUNICODE',
    '-D_UNICODE',
  ],
  win_subsystem: 'windows,6.0',
)
