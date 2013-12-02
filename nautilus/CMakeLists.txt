cmake_minimum_required(VERSION 2.8)

project(library)

#Nautilus
find_package(PkgConfig REQUIRED)
pkg_check_modules(NAUTILUS_EXTENSION REQUIRED libnautilus-extension)
find_path(NAUTILUS_INCLUDE_DIR libnautilus-extension/nautilus-menu-provider.h 
	HINTS ${NAUTILUS_EXTENSION_INCLUDEDIR} ${NAUTILUS_EXTENSION_INCLUDE_DIRS})
include_directories(${NAUTILUS_EXTENSION_INCLUDE_DIRS})

#Glib
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLIB_PKG glib-2.0)

if (GLIB_PKG_FOUND)
    message(Found glib-2.0)
endif (GLIB_PKG_FOUND)

include_directories(${GLIB_PKG_INCLUDE_DIRS})


#DBus
if (DBUS_FOUND)
message ("==DBus found==")
else (DBUS_FOUND)
message ("==DBus not found==")

pkg_check_modules(_DBUS_PC REQUIRED dbus-1)

FIND_PATH(DBUS_INCLUDE_DIR dbus/dbus.h
    ${_DBUS_PC_INCLUDE_DIRS}
    /usr/include
    /usr/include/dbus-1.0
    /usr/local/include
  )

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

endif (DBUS_FOUND)

include_directories (${DBUS_INCLUDE_DIR})
include_directories (${DBUS_ARCH_INCLUDE_DIR})
#include_directories (${DBUS_LIBDIR})


#DBus-Glib

if (DBUSGLIB_FOUND)
message ("==DBUSGLIB found==")
else (DBUSGLIB_FOUND)
message ("==DBUSGLIB not found==")
endif (DBUSGLIB_FOUND)
include_directories (${DBUSGLIB_INCLUDE_DIRS})


#Library

FIND_LIBRARY(DBUSGLIB_LIBRARY
  NAMES 
  dbus-glib-1
  PATHS
  ${DBUSGLIB_LIBRARY_DIR}
  DOC 
  ${DBUSGLIB_LIB_PATH_DESCRIPTION}
  #NO_DEFAULT_PATH TODO fix PKGCONFIG_LIBRARY_DIR when pkg-config returns multiple directories
  )

SET(DBUSGLIB_LIBRARIES ${DBUSGLIB_LIBRARY} ${DBUS_LIBRARIES}) 

IF(DBUSGLIB_LIBRARIES)
  MESSAGE(STATUS "D-Bus glib library set to  ${DBUSGLIB_LIBRARIES}")
ELSE(DBUSGLIB_LIBRARIES)
  MESSAGE(FATAL "D-Bus glib library cannot be found")
ENDIF(DBUSGLIB_LIBRARIES)


#link
add_library(ydisk SHARED ydisk.c)

target_link_libraries (ydisk ${DBUS_LIBRARIES})
target_link_libraries (ydisk ${DBUSGLIB_LIBRARIES})
message (${DBUSGLIB_LIBRARIES})

install(TARGETS ydisk DESTINATION /usr/lib/nautilus/extensions-3.0/)




