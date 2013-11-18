cmake_minimum_required(VERSION 2.8)

project(library)

find_package(PkgConfig REQUIRED)
pkg_check_modules(NAUTILUS_EXTENSION REQUIRED libnautilus-extension)
pkg_check_modules(_DBUS_PC REQUIRED dbus-1)

find_path(NAUTILUS_INCLUDE_DIR libnautilus-extension/nautilus-menu-provider.h 
	HINTS ${NAUTILUS_EXTENSION_INCLUDEDIR} ${NAUTILUS_EXTENSION_INCLUDE_DIRS})

find_path (DBUS_INCLUDE_DIR dbus/dbus.h 
	${_DBUS_PC_INCLUDE_DIRS}
	/usr/include
	/usr/include/dbus-1.0
	/usr/local/include)

FIND_PATH(DBUS_ARCH_INCLUDE_DIR dbus/dbus-arch-deps.h
    ${_DBUS_PC_INCLUDE_DIRS}
    /usr/lib${LIB_SUFFIX}/include
    /usr/lib${LIB_SUFFIX}/dbus-1.0/include
    /usr/lib64/include
    /usr/lib64/dbus-1.0/include
    /usr/lib/include
    /usr/lib/dbus-1.0/include
  )

FIND_LIBRARY(DBUS_LIBRARIES NAMES dbus-1 dbus
    PATHS
     ${_DBUS_PC_LIBDIR}
  )


include_directories(${NAUTILUS_EXTENSION_INCLUDE_DIRS})
include_directories(${_DBUS_PC_INCLUDE_DIRS})
include_directories(${_DBUS_PC_LIBDIR})
add_library(${DBUS_LIBRARIES} SHARED IMPORTED)
add_library(ydisk SHARED ydisk.c)
install(TARGETS ydisk DESTINATION /usr/lib/nautilus/extensions-3.0/)




