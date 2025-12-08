# SPDX-FileCopyrightText: 2023-2025 KUNBUS GmbH
#
# SPDX-License-Identifier: MIT

set(TARGET piTest)
set(SOURCES
	piTest.c
	piControlIf.c
)

add_executable(${TARGET} ${SOURCES})
target_include_directories(${TARGET} PRIVATE ../include ../lib/piControl/src)

# link pthread
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(${TARGET} PRIVATE Threads::Threads)

install(TARGETS ${TARGET} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

# install the symbolic link by first creating it in the binary directory and
# then moving it to the install directory.
# this needs to be done this way because the alternative way of doing
# `install(CODE...` doesn't respect the installation prefix.
add_custom_target(piControlReset ALL
	BYPRODUCTS piControlReset
	COMMAND ln -fs ${TARGET} ${CMAKE_CURRENT_BINARY_DIR}/piControlReset)
add_dependencies(piControlReset ${TARGET})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/piControlReset
	DESTINATION ${CMAKE_INSTALL_BINDIR})
