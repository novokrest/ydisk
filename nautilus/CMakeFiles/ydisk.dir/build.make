# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list

# Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/novokrest/Desktop/ydisk/nautilus

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/novokrest/Desktop/ydisk/nautilus

# Include any dependencies generated for this target.
include CMakeFiles/ydisk.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/ydisk.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/ydisk.dir/flags.make

CMakeFiles/ydisk.dir/ydisk.c.o: CMakeFiles/ydisk.dir/flags.make
CMakeFiles/ydisk.dir/ydisk.c.o: ydisk.c
	$(CMAKE_COMMAND) -E cmake_progress_report /home/novokrest/Desktop/ydisk/nautilus/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object CMakeFiles/ydisk.dir/ydisk.c.o"
	/usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/ydisk.dir/ydisk.c.o   -c /home/novokrest/Desktop/ydisk/nautilus/ydisk.c

CMakeFiles/ydisk.dir/ydisk.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/ydisk.dir/ydisk.c.i"
	/usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -E /home/novokrest/Desktop/ydisk/nautilus/ydisk.c > CMakeFiles/ydisk.dir/ydisk.c.i

CMakeFiles/ydisk.dir/ydisk.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/ydisk.dir/ydisk.c.s"
	/usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -S /home/novokrest/Desktop/ydisk/nautilus/ydisk.c -o CMakeFiles/ydisk.dir/ydisk.c.s

CMakeFiles/ydisk.dir/ydisk.c.o.requires:
.PHONY : CMakeFiles/ydisk.dir/ydisk.c.o.requires

CMakeFiles/ydisk.dir/ydisk.c.o.provides: CMakeFiles/ydisk.dir/ydisk.c.o.requires
	$(MAKE) -f CMakeFiles/ydisk.dir/build.make CMakeFiles/ydisk.dir/ydisk.c.o.provides.build
.PHONY : CMakeFiles/ydisk.dir/ydisk.c.o.provides

CMakeFiles/ydisk.dir/ydisk.c.o.provides.build: CMakeFiles/ydisk.dir/ydisk.c.o

# Object files for target ydisk
ydisk_OBJECTS = \
"CMakeFiles/ydisk.dir/ydisk.c.o"

# External object files for target ydisk
ydisk_EXTERNAL_OBJECTS =

libydisk.so: CMakeFiles/ydisk.dir/ydisk.c.o
libydisk.so: /usr/lib/x86_64-linux-gnu/libdbus-1.so
libydisk.so: /usr/lib/x86_64-linux-gnu/libdbus-glib-1.so
libydisk.so: /usr/lib/x86_64-linux-gnu/libdbus-1.so
libydisk.so: /usr/lib/x86_64-linux-gnu/libdbus-glib-1.so
libydisk.so: CMakeFiles/ydisk.dir/build.make
libydisk.so: CMakeFiles/ydisk.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking C shared library libydisk.so"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/ydisk.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/ydisk.dir/build: libydisk.so
.PHONY : CMakeFiles/ydisk.dir/build

CMakeFiles/ydisk.dir/requires: CMakeFiles/ydisk.dir/ydisk.c.o.requires
.PHONY : CMakeFiles/ydisk.dir/requires

CMakeFiles/ydisk.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/ydisk.dir/cmake_clean.cmake
.PHONY : CMakeFiles/ydisk.dir/clean

CMakeFiles/ydisk.dir/depend:
	cd /home/novokrest/Desktop/ydisk/nautilus && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/novokrest/Desktop/ydisk/nautilus /home/novokrest/Desktop/ydisk/nautilus /home/novokrest/Desktop/ydisk/nautilus /home/novokrest/Desktop/ydisk/nautilus /home/novokrest/Desktop/ydisk/nautilus/CMakeFiles/ydisk.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/ydisk.dir/depend

