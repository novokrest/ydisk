cmake_minimum_required(VERSION 2.8)

project(library)

find_package(PkgConfig REQUIRED)
pkg_check_modules(NAUTILUS_EXTENSION REQUIRED libnautilus-extension)
find_path(NAUTILUS_INCLUDE_DIR libnautilus-extension/nautilus-menu-provider.h 
	HINTS ${NAUTILUS_EXTENSION_INCLUDEDIR} ${NAUTILUS_EXTENSION_INCLUDE_DIRS})

find_package(PkgConfig REQUIRED)
pkg_check_modules(GLIB_PKG glib-2.0)
if (GLIB_PKG_FOUND)
    message(Found glib-2.0)
endif (GLIB_PKG_FOUND)


#FIND_PATH(DBUS_INCLUDE_DIR dbus/dbus.h)
#FIND_LIBRARY(DBUS_LIBRARIES NAMES dbus-1)
message (${_DBUS_PC_INCLUDE_DIRS})
MESSAGE(${DBUS_INCLUDE_DIR})

message (${_DBUS_PC_LIBDIR})
MESSAGE(${DBUS_LIBRARIES})

pkg_check_modules(_DBUS_PC REQUIRED dbus-1)
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



include_directories(${NAUTILUS_EXTENSION_INCLUDE_DIRS})
include_directories(${GLIB_PKG_INCLUDE_DIRS})
include_directories(${_DBUS_PC_INCLUDE_DIRS})
include_directories(${_DBUS_PC_LIBDIR})

link_directories (${_DBUS_PC_LIBDIR})
add_library(${DBUS_LIBRARIES} SHARED IMPORTED)
add_library(ydisk SHARED ydisk.c)
link_directories (/lib/x86_64-linux-gnu/)
add_library(/lib/x86_64-linux-gnu/libdbus-1.so.3.5.8 SHARED IMPORTED)



set_target_properties(ydisk
      PROPERTIES
      LINK_INTERFACE_LIBRARIES /lib/x86_64-linux-gnu/libdbus-1.so.3.5.8
  )

#target_link_libraries (ydisk ${DBUS_LIBRARIES})
target_link_libraries (ydisk /lib/x86_64-linux-gnu/libdbus-1.so.3.5.8)

install(TARGETS ydisk DESTINATION /usr/lib/nautilus/extensions-3.0/)




