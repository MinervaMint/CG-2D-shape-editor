# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.18

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Disable VCS-based implicit rules.
% : %,v


# Disable VCS-based implicit rules.
% : RCS/%


# Disable VCS-based implicit rules.
% : RCS/%,v


# Disable VCS-based implicit rules.
% : SCCS/s.%


# Disable VCS-based implicit rules.
% : s.%


.SUFFIXES: .hpux_make_needs_suffix_list


# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/local/Cellar/cmake/3.18.2/bin/cmake

# The command to remove a file.
RM = /usr/local/Cellar/cmake/3.18.2/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/sensui/Documents/NYU/CG/cg-master/Assignment_6

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/sensui/Documents/NYU/CG/cg-master/Assignment_6/build

# Include any dependencies generated for this target.
include CMakeFiles/Assignment_6.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/Assignment_6.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/Assignment_6.dir/flags.make

CMakeFiles/Assignment_6.dir/src/SDLViewer.cpp.o: CMakeFiles/Assignment_6.dir/flags.make
CMakeFiles/Assignment_6.dir/src/SDLViewer.cpp.o: ../src/SDLViewer.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/sensui/Documents/NYU/CG/cg-master/Assignment_6/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/Assignment_6.dir/src/SDLViewer.cpp.o"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Assignment_6.dir/src/SDLViewer.cpp.o -c /Users/sensui/Documents/NYU/CG/cg-master/Assignment_6/src/SDLViewer.cpp

CMakeFiles/Assignment_6.dir/src/SDLViewer.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Assignment_6.dir/src/SDLViewer.cpp.i"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/sensui/Documents/NYU/CG/cg-master/Assignment_6/src/SDLViewer.cpp > CMakeFiles/Assignment_6.dir/src/SDLViewer.cpp.i

CMakeFiles/Assignment_6.dir/src/SDLViewer.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Assignment_6.dir/src/SDLViewer.cpp.s"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/sensui/Documents/NYU/CG/cg-master/Assignment_6/src/SDLViewer.cpp -o CMakeFiles/Assignment_6.dir/src/SDLViewer.cpp.s

# Object files for target Assignment_6
Assignment_6_OBJECTS = \
"CMakeFiles/Assignment_6.dir/src/SDLViewer.cpp.o"

# External object files for target Assignment_6
Assignment_6_EXTERNAL_OBJECTS =

libAssignment_6.a: CMakeFiles/Assignment_6.dir/src/SDLViewer.cpp.o
libAssignment_6.a: CMakeFiles/Assignment_6.dir/build.make
libAssignment_6.a: CMakeFiles/Assignment_6.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/sensui/Documents/NYU/CG/cg-master/Assignment_6/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX static library libAssignment_6.a"
	$(CMAKE_COMMAND) -P CMakeFiles/Assignment_6.dir/cmake_clean_target.cmake
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/Assignment_6.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/Assignment_6.dir/build: libAssignment_6.a

.PHONY : CMakeFiles/Assignment_6.dir/build

CMakeFiles/Assignment_6.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/Assignment_6.dir/cmake_clean.cmake
.PHONY : CMakeFiles/Assignment_6.dir/clean

CMakeFiles/Assignment_6.dir/depend:
	cd /Users/sensui/Documents/NYU/CG/cg-master/Assignment_6/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/sensui/Documents/NYU/CG/cg-master/Assignment_6 /Users/sensui/Documents/NYU/CG/cg-master/Assignment_6 /Users/sensui/Documents/NYU/CG/cg-master/Assignment_6/build /Users/sensui/Documents/NYU/CG/cg-master/Assignment_6/build /Users/sensui/Documents/NYU/CG/cg-master/Assignment_6/build/CMakeFiles/Assignment_6.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/Assignment_6.dir/depend

