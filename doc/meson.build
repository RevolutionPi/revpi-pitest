# SPDX-FileCopyrightText: 2026 KUNBUS GmbH
#
# SPDX-License-Identifier: MIT

scdoc = dependency('scdoc', required: get_option('man-pages'))
if scdoc.found()
  scdoc_prog = find_program(scdoc.get_variable(pkgconfig: 'scdoc'), native: true)
  mandir = get_option('mandir')
  custom_target(
    'piTest.1',
    input: 'piTest.1.scd',
    output: 'piTest.1',
    command: scdoc_prog,
    install: true,
    feed: true,
    capture: true,
    install_dir: '@0@/man1'.format(mandir),
  )
endif
