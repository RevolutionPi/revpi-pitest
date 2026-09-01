# SPDX-FileCopyrightText: 2026 KUNBUS GmbH
#
# SPDX-License-Identifier: MIT

sources = [
  'piControlIf.c',
  'piTest.c',
]

threads_dep = dependency('threads', required: true)

executable(
  'piTest',
  sources,
  include_directories: includes,
  dependencies: threads_dep,
  install: true,
  install_dir: bindir,
)

install_symlink(
  'piControlReset',
  install_dir: bindir,
  pointing_to: 'piTest',
)
