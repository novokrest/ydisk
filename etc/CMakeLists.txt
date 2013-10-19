cmake_minimum_required(VERSION 2.8)

project(library)

include_directories(/usr/include/nautilus/libnautilus-extension)
include_directories(/usr/include/glib-2.0)
include_directories(/usr/lib/i386-linux-gnu/glib-2.0/include)
include_directories(/usr/include/gtk-3.0)
include_directories(/usr/include/pango-1.0)
include_directories(/usr/include/gio-unix-2.0/)
include_directories(/usr/include/atk-1.0)
include_directories(/usr/include/cairo)
include_directories(/usr/include/gdk-pixbuf-2.0)
include_directories(/usr/include/freetype2)
include_directories(/usr/include/pixman-1)
include_directories(/usr/include/libpng12)

add_library(ydisk SHARED ydisk.c)
install(TARGETS ydisk DESTINATION /usr/lib/nautilus/extensions-3.0/)
