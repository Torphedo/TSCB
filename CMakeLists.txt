cmake_minimum_required(VERSION 3.16) # Latest version on apt.
project(TSCB
	VERSION 0.2.0
	LANGUAGES CXX C
)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 20)

# Using this setup to run other CMakeLists.txt build scripts makes it
# easier to add unit tests or other separate scripts in the future.
add_subdirectory(src)

# Copy binary ALR files to [build]/src/bin/ so they can be accessed during debugging. 
# It would be better to change the working directory to /alr_parser/, but I couldn't
# find a good cross-platform (directly from CMake) way to do this.
file(COPY "bin/" DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/src/bin)
