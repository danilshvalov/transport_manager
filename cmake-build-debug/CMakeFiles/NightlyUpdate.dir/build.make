# CMAKE generated file: DO NOT EDIT!
# Generated by "NMake Makefiles" Generator, CMake Version 3.17

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


.SUFFIXES: .hpux_make_needs_suffix_list


# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE
NULL=nul
!ENDIF
SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = "C:\dev\programs\CLion 2020.3.2\bin\cmake\win\bin\cmake.exe"

# The command to remove a file.
RM = "C:\dev\programs\CLion 2020.3.2\bin\cmake\win\bin\cmake.exe" -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = C:\dev\projects\cpp\transport_manager

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = C:\dev\projects\cpp\transport_manager\cmake-build-debug

# Utility rule file for NightlyUpdate.

# Include the progress variables for this target.
include CMakeFiles\NightlyUpdate.dir\progress.make

CMakeFiles\NightlyUpdate:
	echo >nul && "C:\dev\programs\CLion 2020.3.2\bin\cmake\win\bin\ctest.exe" -D NightlyUpdate

NightlyUpdate: CMakeFiles\NightlyUpdate
NightlyUpdate: CMakeFiles\NightlyUpdate.dir\build.make

.PHONY : NightlyUpdate

# Rule to build all files generated by this target.
CMakeFiles\NightlyUpdate.dir\build: NightlyUpdate

.PHONY : CMakeFiles\NightlyUpdate.dir\build

CMakeFiles\NightlyUpdate.dir\clean:
	$(CMAKE_COMMAND) -P CMakeFiles\NightlyUpdate.dir\cmake_clean.cmake
.PHONY : CMakeFiles\NightlyUpdate.dir\clean

CMakeFiles\NightlyUpdate.dir\depend:
	$(CMAKE_COMMAND) -E cmake_depends "NMake Makefiles" C:\dev\projects\cpp\transport_manager C:\dev\projects\cpp\transport_manager C:\dev\projects\cpp\transport_manager\cmake-build-debug C:\dev\projects\cpp\transport_manager\cmake-build-debug C:\dev\projects\cpp\transport_manager\cmake-build-debug\CMakeFiles\NightlyUpdate.dir\DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles\NightlyUpdate.dir\depend

