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

set(LINK_NAME ${CMAKE_CURRENT_BINARY_DIR}/piControlReset)

add_custom_command(
    OUTPUT ${LINK_NAME}
    COMMAND ${CMAKE_COMMAND} -E create_symlink ${TARGET} piControlReset
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS ${TARGET}
)

add_custom_target(generate_piControlReset ALL DEPENDS ${LINK_NAME})

install(FILES ${LINK_NAME} DESTINATION ${CMAKE_INSTALL_BINDIR})
