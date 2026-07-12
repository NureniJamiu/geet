cmake_minimum_required(VERSION 3.14)
project(geet LANGUAGES CXX)

# Suppress deprecation warnings from third-party dependencies (like CLI11)
# that specify old CMake minimum versions (e.g. 3.5) which are deprecated in CMake 4.x.
if(COMMAND cmake_diagnostic)
  cmake_diagnostic(SET CMD_DEPRECATED IGNORE)
else()
  set(CMAKE_WARN_DEPRECATED OFF)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Export compile_commands.json to help IDE language servers find header paths (like CLI11)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


include(FetchContent)

FetchContent_Declare(
  cli11
  GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
  GIT_TAG        v2.4.2
)

FetchContent_MakeAvailable(cli11)

add_executable(geet src/main.cpp)

target_link_libraries(geet PRIVATE CLI11::CLI11)