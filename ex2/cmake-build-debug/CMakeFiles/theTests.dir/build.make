# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.17

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
CMAKE_COMMAND = /usr/local/APP/jetbrains/clion/2020.2/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /usr/local/APP/jetbrains/clion/2020.2/bin/cmake/linux/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /cs/usr/davidponar/OS-HUJI-Course/ex2

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /cs/usr/davidponar/OS-HUJI-Course/ex2/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/theTests.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/theTests.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/theTests.dir/flags.make

CMakeFiles/theTests.dir/tests_to_be_ran_separately.cpp.o: CMakeFiles/theTests.dir/flags.make
CMakeFiles/theTests.dir/tests_to_be_ran_separately.cpp.o: ../tests_to_be_ran_separately.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/cs/usr/davidponar/OS-HUJI-Course/ex2/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/theTests.dir/tests_to_be_ran_separately.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/theTests.dir/tests_to_be_ran_separately.cpp.o -c /cs/usr/davidponar/OS-HUJI-Course/ex2/tests_to_be_ran_separately.cpp

CMakeFiles/theTests.dir/tests_to_be_ran_separately.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/theTests.dir/tests_to_be_ran_separately.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /cs/usr/davidponar/OS-HUJI-Course/ex2/tests_to_be_ran_separately.cpp > CMakeFiles/theTests.dir/tests_to_be_ran_separately.cpp.i

CMakeFiles/theTests.dir/tests_to_be_ran_separately.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/theTests.dir/tests_to_be_ran_separately.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /cs/usr/davidponar/OS-HUJI-Course/ex2/tests_to_be_ran_separately.cpp -o CMakeFiles/theTests.dir/tests_to_be_ran_separately.cpp.s

CMakeFiles/theTests.dir/Threads.cpp.o: CMakeFiles/theTests.dir/flags.make
CMakeFiles/theTests.dir/Threads.cpp.o: ../Threads.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/cs/usr/davidponar/OS-HUJI-Course/ex2/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/theTests.dir/Threads.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/theTests.dir/Threads.cpp.o -c /cs/usr/davidponar/OS-HUJI-Course/ex2/Threads.cpp

CMakeFiles/theTests.dir/Threads.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/theTests.dir/Threads.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /cs/usr/davidponar/OS-HUJI-Course/ex2/Threads.cpp > CMakeFiles/theTests.dir/Threads.cpp.i

CMakeFiles/theTests.dir/Threads.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/theTests.dir/Threads.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /cs/usr/davidponar/OS-HUJI-Course/ex2/Threads.cpp -o CMakeFiles/theTests.dir/Threads.cpp.s

CMakeFiles/theTests.dir/uthreads.cpp.o: CMakeFiles/theTests.dir/flags.make
CMakeFiles/theTests.dir/uthreads.cpp.o: ../uthreads.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/cs/usr/davidponar/OS-HUJI-Course/ex2/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/theTests.dir/uthreads.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/theTests.dir/uthreads.cpp.o -c /cs/usr/davidponar/OS-HUJI-Course/ex2/uthreads.cpp

CMakeFiles/theTests.dir/uthreads.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/theTests.dir/uthreads.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /cs/usr/davidponar/OS-HUJI-Course/ex2/uthreads.cpp > CMakeFiles/theTests.dir/uthreads.cpp.i

CMakeFiles/theTests.dir/uthreads.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/theTests.dir/uthreads.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /cs/usr/davidponar/OS-HUJI-Course/ex2/uthreads.cpp -o CMakeFiles/theTests.dir/uthreads.cpp.s

# Object files for target theTests
theTests_OBJECTS = \
"CMakeFiles/theTests.dir/tests_to_be_ran_separately.cpp.o" \
"CMakeFiles/theTests.dir/Threads.cpp.o" \
"CMakeFiles/theTests.dir/uthreads.cpp.o"

# External object files for target theTests
theTests_EXTERNAL_OBJECTS =

theTests: CMakeFiles/theTests.dir/tests_to_be_ran_separately.cpp.o
theTests: CMakeFiles/theTests.dir/Threads.cpp.o
theTests: CMakeFiles/theTests.dir/uthreads.cpp.o
theTests: CMakeFiles/theTests.dir/build.make
theTests: lib/libgtest_maind.a
theTests: lib/libgtestd.a
theTests: CMakeFiles/theTests.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/cs/usr/davidponar/OS-HUJI-Course/ex2/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Linking CXX executable theTests"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/theTests.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/theTests.dir/build: theTests

.PHONY : CMakeFiles/theTests.dir/build

CMakeFiles/theTests.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/theTests.dir/cmake_clean.cmake
.PHONY : CMakeFiles/theTests.dir/clean

CMakeFiles/theTests.dir/depend:
	cd /cs/usr/davidponar/OS-HUJI-Course/ex2/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /cs/usr/davidponar/OS-HUJI-Course/ex2 /cs/usr/davidponar/OS-HUJI-Course/ex2 /cs/usr/davidponar/OS-HUJI-Course/ex2/cmake-build-debug /cs/usr/davidponar/OS-HUJI-Course/ex2/cmake-build-debug /cs/usr/davidponar/OS-HUJI-Course/ex2/cmake-build-debug/CMakeFiles/theTests.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/theTests.dir/depend

