# SPDX-FileCopyrightText: 2023 KUNBUS GmbH
#
# SPDX-License-Identifier: MIT

add_custom_target(
	piTest.1 ALL
	COMMAND scdoc < ${CMAKE_SOURCE_DIR}/doc/piTest.1.scd > ${PROJECT_BINARY_DIR}/piTest.1
	DEPENDS ${CMAKE_SOURCE_DIR}/doc/piTest.1.scd
)

install(FILES ${PROJECT_BINARY_DIR}/piTest.1
	DESTINATION ${CMAKE_INSTALL_MANDIR}/man1/
)

install(FILES ../lib/piControl/picontrol_ioctl.4 DESTINATION ${CMAKE_INSTALL_MANDIR}/man4)
