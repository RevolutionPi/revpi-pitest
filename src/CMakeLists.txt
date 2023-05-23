set(TARGET piTest)
set(SOURCES
	piTest.c
	piControlIf.c
)

add_executable(${TARGET} ${SOURCES})
target_include_directories(${TARGET} PRIVATE ../include ../lib/piControl)

install(TARGETS ${TARGET} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

# install the symbolic link by first creating it in the binary directory and
# then moving it to the install directory.
# this needs to be done this way because the alternative way of doing
# `install(CODE...` doesn't respect the installation prefix.
add_custom_target(piControlReset ALL
	COMMAND ln -fs ${TARGET} ${PROJECT_BINARY_DIR}/piControlReset)
install(FILES ${PROJECT_BINARY_DIR}/piControlReset
	DESTINATION ${CMAKE_INSTALL_BINDIR})
