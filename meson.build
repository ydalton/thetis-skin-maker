project(
  'win32skinmaker',
  'c'
)

cc = meson.get_compiler('c')

windows = import('windows')

win32skinmaker_sources = files(
  'main.c',
  'window.c',
  'preview.c',
)

win32skinmaker_deps = [
  cc.find_library('shlwapi'),
]

win32skinmaker_sources += windows.compile_resources('resources.rc')

executable(
  meson.project_name(),
  win32skinmaker_sources,
  dependencies: win32skinmaker_deps,
  win_subsystem: 'windows,6.0'
)