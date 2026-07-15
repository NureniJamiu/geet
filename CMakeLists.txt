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

# Declare the CLI11 library dependency (Argument Parser).
FetchContent_Declare(
  cli11
  GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
  GIT_TAG        v2.4.2
)

# Declare the inipp library dependency (INI Parser/Generator).
FetchContent_Declare(
  inipp
  GIT_REPOSITORY https://github.com/mcmtroffaes/inipp.git
  GIT_TAG        1.0.12
)

FetchContent_Declare(
  picosha2
  GIT_REPOSITORY https://github.com/okdshin/PicoSHA2.git
  GIT_TAG        master
)

FetchContent_MakeAvailable(cli11 inipp picosha2)

find_package(ZLIB REQUIRED)

add_executable(geet src/main.cpp src/repository.cpp src/object.cpp)

target_link_libraries(geet PRIVATE CLI11::CLI11 ZLIB::ZLIB)

target_include_directories(geet PRIVATE ${inipp_SOURCE_DIR}/inipp ${picosha2_SOURCE_DIR})