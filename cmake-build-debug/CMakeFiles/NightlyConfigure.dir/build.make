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

# Utility rule file for NightlyConfigure.

# Include the progress variables for this target.
include CMakeFiles\NightlyConfigure.dir\progress.make

CMakeFiles\NightlyConfigure:
	echo >nul && "C:\dev\programs\CLion 2020.3.2\bin\cmake\win\bin\ctest.exe" -D NightlyConfigure

NightlyConfigure: CMakeFiles\NightlyConfigure
NightlyConfigure: CMakeFiles\NightlyConfigure.dir\build.make

.PHONY : NightlyConfigure

# Rule to build all files generated by this target.
CMakeFiles\NightlyConfigure.dir\build: NightlyConfigure

.PHONY : CMakeFiles\NightlyConfigure.dir\build

CMakeFiles\NightlyConfigure.dir\clean:
	$(CMAKE_COMMAND) -P CMakeFiles\NightlyConfigure.dir\cmake_clean.cmake
.PHONY : CMakeFiles\NightlyConfigure.dir\clean

CMakeFiles\NightlyConfigure.dir\depend:
	$(CMAKE_COMMAND) -E cmake_depends "NMake Makefiles" C:\dev\projects\cpp\transport_manager C:\dev\projects\cpp\transport_manager C:\dev\projects\cpp\transport_manager\cmake-build-debug C:\dev\projects\cpp\transport_manager\cmake-build-debug C:\dev\projects\cpp\transport_manager\cmake-build-debug\CMakeFiles\NightlyConfigure.dir\DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles\NightlyConfigure.dir\depend
