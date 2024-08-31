project(
  'win32skinmaker',
  'c'
)

windows = import('windows')

win32skinmaker_sources = files(
  'main.c',
)

win32skinmaker_sources += windows.compile_resources('resources.rc')

executable(
  meson.project_name(),
  win32skinmaker_sources,
  win_subsystem: 'windows,6.0'
)