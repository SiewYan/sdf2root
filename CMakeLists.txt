# ----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)
project(sdf2root)

add_definitions("-std=c++17 -march=native -mtune=native")

# Making json file for auto complete
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Searching ROOT. FindROOT.cmake was copied from ROOT source
include(FindROOT.cmake)

# ----------------------------------------------------------------------------
# Locate sources and headers for this project
# NB: headers are included so they will show up in IDEs
#
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include
    ${ROOT_INCLUDE_DIR})
link_directories(${ROOT_LIBRARY_DIR})
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cpp)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hpp)

# ----------------------------------------------------------------------------
# Generating executable file and linking
#
add_executable(sdf2root main.cpp ${sources} ${headers})
target_link_libraries(sdf2root ${ROOT_LIBRARIES})
add_executable(parTree parTree.cpp ${sources} ${headers})
target_link_libraries(parTree ${ROOT_LIBRARIES})

# ----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS sdf2root DESTINATION bin)
install(TARGETS parTree DESTINATION bin)
