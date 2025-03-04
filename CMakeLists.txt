cmake_minimum_required (VERSION 3.28)
project (GameloftJ2METool)

# Build options
option(SDL_VENDORED "Use vendored SDL libraries" OFF) #Switch between a system sdl library and a vendored sdl library
option(BUILD_DEBUG "Create debug build" ON)
option(NATIVE_OPTIMIZE "Optimize for native CPU" OFF)
set(CMAKE_CXX_STANDARD 14)

# Build settings
if(BUILD_DEBUG)
    set(CMAKE_BUILD_TYPE Debug)
    set(CMAKE_DEBUG_POSTFIX d)
else()
    set(CMAKE_BUILD_TYPE Release)
    set(CMAKE_CXX_FLAGS_RELEASE "-O2")
endif()
if(NATIVE_OPTIMIZE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
endif()

set(CMAKE_CXX_FLAGD "${CMAKE_CXX_FLAGS} -Wall -Werror -Wextra -Wpedantic")

if(SDL_VENDORED)
    add_subdirectory(vendored/SDL EXCLUDE_FROM_ALL)
else()
    # 1. Look for a SDL3 package, 2. look for the SDL3 component and 3. fail if none can be found
    find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3-shared)
endif()

# Find dependencies
#find_package(SDL3_image REQUIRED)
find_package(SDL3 REQUIRED)
include_directories(${SDL3_INCLUDE_DIRS})

add_library(ImGui
    include/imgui/imgui.cpp
    include/imgui/imgui_demo.cpp
    include/imgui/imgui_draw.cpp
    include/imgui/imgui_tables.cpp
    include/imgui/imgui_widgets.cpp
    include/imgui/misc/cpp/imgui_stdlib.cpp
    include/imgui/backends/imgui_impl_sdl3.cpp
    include/imgui/backends/imgui_impl_sdlrenderer3.cpp
    include/imgui/backends/imgui_impl_opengl2.cpp
)

target_link_libraries(ImGui SDL3::SDL3)

# Include source directories and add executable
include_directories(include)
include_directories(include/imgui)
include_directories(include/imgui/misc/cpp)
include_directories(include/imgui/backends)

include_directories(src/Core)
include_directories(src/UI)
include_directories(src/GameloftGfx)
add_subdirectory(src/Core)
add_subdirectory(src/UI)
add_subdirectory(src/GameloftGfx)
add_executable (GameloftJ2METool src/Main.cpp)

if(WIN32)
    target_link_options(GameloftJ2METool PRIVATE "-mwindows") #Disable console pop-up
endif(WIN32)

# Indicate which libraries to include during the link process.
target_link_libraries(GameloftJ2METool SDL3::SDL3)
target_link_libraries(GameloftJ2METool ImGui)
target_link_libraries(GameloftJ2METool Core)
target_link_libraries(GameloftJ2METool UI)
target_link_libraries(GameloftJ2METool GameloftGfx)
