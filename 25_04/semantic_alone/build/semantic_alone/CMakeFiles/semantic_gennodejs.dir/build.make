# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.5

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


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

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/ubuntu/TESI/src

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/ubuntu/TESI/build

# Utility rule file for semantic_gennodejs.

# Include the progress variables for this target.
include semantic_alone/CMakeFiles/semantic_gennodejs.dir/progress.make

semantic_gennodejs: semantic_alone/CMakeFiles/semantic_gennodejs.dir/build.make

.PHONY : semantic_gennodejs

# Rule to build all files generated by this target.
semantic_alone/CMakeFiles/semantic_gennodejs.dir/build: semantic_gennodejs

.PHONY : semantic_alone/CMakeFiles/semantic_gennodejs.dir/build

semantic_alone/CMakeFiles/semantic_gennodejs.dir/clean:
	cd /home/ubuntu/TESI/build/semantic_alone && $(CMAKE_COMMAND) -P CMakeFiles/semantic_gennodejs.dir/cmake_clean.cmake
.PHONY : semantic_alone/CMakeFiles/semantic_gennodejs.dir/clean

semantic_alone/CMakeFiles/semantic_gennodejs.dir/depend:
	cd /home/ubuntu/TESI/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/ubuntu/TESI/src /home/ubuntu/TESI/src/semantic_alone /home/ubuntu/TESI/build /home/ubuntu/TESI/build/semantic_alone /home/ubuntu/TESI/build/semantic_alone/CMakeFiles/semantic_gennodejs.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : semantic_alone/CMakeFiles/semantic_gennodejs.dir/depend

