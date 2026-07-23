# CMakeList.txt : CMake project for RaylibPuzzle, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.14)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("RaylibPuzzle")

# raylib
find_package(raylib QUIET)
if (NOT raylib_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG 6.0
        GIT_SHALLOW 1
    )
    FetchContent_MakeAvailable(raylib)
endif()

# raylib-cpp
find_package(raylib_cpp QUIET)
if (NOT raylib_cpp_FOUND)
    if (NOT DEFINED RAYLIB_CPP_VERSION)
        set(RAYLIB_CPP_VERSION v6.0.0)
    endif()
    include(FetchContent)
    FetchContent_Declare(
        raylib_cpp
        GIT_REPOSITORY https://github.com/RobLoach/raylib-cpp.git
        GIT_TAG ${RAYLIB_CPP_VERSION}
    )
    FetchContent_MakeAvailable(raylib_cpp)
endif()



# This is the main part:
set(SOURCES
    "src/main.cpp"
    "src/core/Game.cpp"
    "src/editor/Editor.cpp"
    "src/gameplay/GameplayController.cpp"
    "src/gameplay/GameplayLogic.cpp"
    "src/gameplay/GameplayRenderer.cpp"
    "src/systems/AnimationSystem.cpp"
    "src/systems/InputManager.cpp"
    "src/systems/SoundSystem.cpp"
    "src/ui/DebugDrawer.cpp"
    "src/ui/MainMenu.cpp"
    "src/world/Grid.cpp"
    "src/world/Level.cpp"
    "src/core/SaveData.cpp"
)
add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE src)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 20)
target_link_libraries(${PROJECT_NAME} PUBLIC raylib raylib_cpp)

# raylib's own CMakeLists.txt sets a narrow EXPORTED_RUNTIME_METHODS=ccall
# as a PUBLIC link option, which clobbers ours since it's the last one
# Emscripten sees. Strip it so only our own setting (added below via
# target_link_options on our executable) takes effect.
if (${PLATFORM} STREQUAL "Web")
    get_target_property(_raylib_link_opts raylib INTERFACE_LINK_OPTIONS)
    if (_raylib_link_opts)
        list(FILTER _raylib_link_opts EXCLUDE REGEX "EXPORTED_RUNTIME_METHODS")
        set_target_properties(raylib PROPERTIES INTERFACE_LINK_OPTIONS "${_raylib_link_opts}")
    endif()
endif()

option(FORCE_ENABLE_MOBILE_GAMEPAD "Enable mobile gamepad support" OFF)

if(FORCE_ENABLE_MOBILE_GAMEPAD)
    target_compile_definitions(${PROJECT_NAME} PRIVATE FORCE_ENABLE_MOBILE_GAMEPAD)
endif()

# Add source to this project's executable.
# add_executable (RaylibPuzzle "RaylibPuzzle.cpp" "RaylibPuzzle.h")

# Web Configurations
if (${PLATFORM} STREQUAL "Web")
    # Tell Emscripten to build an example.html file.
    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".html")
    # Required linker flags for using Raylib with Emscripten
    target_link_options(${PROJECT_NAME} PRIVATE 
        --preload-file ${CMAKE_SOURCE_DIR}/resources@/resources
        -sEXPORTED_FUNCTIONS=['_main','_malloc']
        -sEXPORTED_RUNTIME_METHODS=['ccall','requestFullscreen','setCanvasSize']
        -sUSE_GLFW=3
        "--shell-file=$<IF:$<CONFIG:Release>,${CMAKE_SOURCE_DIR}/src/release_shell.html,${CMAKE_SOURCE_DIR}/src/debug_shell.html>"
    )
endif()

# TODO: Add tests and install targets if needed.
