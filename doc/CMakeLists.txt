# SPDX-FileCopyrightText: 2023-2025 KUNBUS GmbH
#
# SPDX-License-Identifier: MIT

find_program(SCDOC scdoc REQUIRED)

add_custom_target(
	piTest.1 ALL
	COMMAND ${SCDOC} < ${CMAKE_CURRENT_SOURCE_DIR}/piTest.1.scd > ${CMAKE_CURRENT_BINARY_DIR}/piTest.1
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/piTest.1.scd
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/piTest.1
	DESTINATION ${CMAKE_INSTALL_MANDIR}/man1/
)

install(FILES ../lib/piControl/src/picontrol_ioctl.4 DESTINATION ${CMAKE_INSTALL_MANDIR}/man4)
