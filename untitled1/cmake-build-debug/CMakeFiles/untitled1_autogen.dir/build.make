# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 3.29

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

SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = "C:\Program Files\JetBrains\CLion 2024.2.1\bin\cmake\win\x64\bin\cmake.exe"

# The command to remove a file.
RM = "C:\Program Files\JetBrains\CLion 2024.2.1\bin\cmake\win\x64\bin\cmake.exe" -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = C:\Tank-Attack-\untitled1

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = C:\Tank-Attack-\untitled1\cmake-build-debug

# Utility rule file for untitled1_autogen.

# Include any custom commands dependencies for this target.
include CMakeFiles/untitled1_autogen.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/untitled1_autogen.dir/progress.make

CMakeFiles/untitled1_autogen: untitled1_autogen/timestamp

untitled1_autogen/timestamp: C:/Qt/6.7.2/mingw_64/./bin/moc.exe
untitled1_autogen/timestamp: C:/Qt/6.7.2/mingw_64/./bin/uic.exe
untitled1_autogen/timestamp: CMakeFiles/untitled1_autogen.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --blue --bold --progress-dir=C:\Tank-Attack-\untitled1\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Automatic MOC and UIC for target untitled1"
	"C:\Program Files\JetBrains\CLion 2024.2.1\bin\cmake\win\x64\bin\cmake.exe" -E cmake_autogen C:/Tank-Attack-/untitled1/cmake-build-debug/CMakeFiles/untitled1_autogen.dir/AutogenInfo.json Debug
	"C:\Program Files\JetBrains\CLion 2024.2.1\bin\cmake\win\x64\bin\cmake.exe" -E touch C:/Tank-Attack-/untitled1/cmake-build-debug/untitled1_autogen/timestamp

untitled1_autogen: CMakeFiles/untitled1_autogen
untitled1_autogen: untitled1_autogen/timestamp
untitled1_autogen: CMakeFiles/untitled1_autogen.dir/build.make
.PHONY : untitled1_autogen

# Rule to build all files generated by this target.
CMakeFiles/untitled1_autogen.dir/build: untitled1_autogen
.PHONY : CMakeFiles/untitled1_autogen.dir/build

CMakeFiles/untitled1_autogen.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles\untitled1_autogen.dir\cmake_clean.cmake
.PHONY : CMakeFiles/untitled1_autogen.dir/clean

CMakeFiles/untitled1_autogen.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" C:\Tank-Attack-\untitled1 C:\Tank-Attack-\untitled1 C:\Tank-Attack-\untitled1\cmake-build-debug C:\Tank-Attack-\untitled1\cmake-build-debug C:\Tank-Attack-\untitled1\cmake-build-debug\CMakeFiles\untitled1_autogen.dir\DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/untitled1_autogen.dir/depend

