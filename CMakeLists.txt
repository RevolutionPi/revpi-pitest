# SPDX-FileCopyrightText: 2023-2026 KUNBUS GmbH
#
# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.7)

project(piTest
	LANGUAGES C
	VERSION 2.1.1
)

include(GNUInstallDirs)

add_subdirectory(include)
add_subdirectory(src)
add_subdirectory(doc)
