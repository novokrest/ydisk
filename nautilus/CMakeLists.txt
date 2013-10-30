cmake_minimum_required(VERSION 2.8)

project(library)

find_package(PkgConfig REQUIRED)
pkg_check_modules(NAUTILUS_EXTENSION REQUIRED libnautilus-extension)

find_path(NAUTILUS_INCLUDE_DIR libnautilus-extension/nautilus-menu-provider.h 
	HINTS ${NAUTILUS_EXTENSION_INCLUDEDIR} ${NAUTILUS_EXTENSION_INCLUDE_DIRS})

include_directories(${NAUTILUS_EXTENSION_INCLUDE_DIRS})

add_library(ydisk SHARED ydisk.c)
install(TARGETS ydisk DESTINATION /usr/lib/nautilus/extensions-3.0/)
