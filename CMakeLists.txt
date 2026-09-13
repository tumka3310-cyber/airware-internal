cmake_minimum_required(VERSION 3.20)
project(AirwareAndroid LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS_RELEASE "-O2 -fvisibility=hidden -ffunction-sections -fdata-sections")
set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "-Wl,--gc-sections -Wl,--strip-all")

include(FetchContent)

FetchContent_Declare(imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.90.4
    GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(imgui)

# BepInEx/Dobby — форк jmpews/Dobby с исправлениями стабильности
set(DOBBY_GENERATE_SHARED OFF CACHE BOOL "" FORCE)
set(DOBBY_DEBUG OFF CACHE BOOL "" FORCE)
FetchContent_Declare(dobby
    GIT_REPOSITORY https://github.com/BepInEx/Dobby.git
    GIT_TAG master
    GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(dobby)

set(IMGUI_SOURCES
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_android.cpp)

add_library(airware SHARED
    src/main.cpp
    src/sdk/il2cpp.cpp
    src/sdk/game.cpp
    src/utils/memory.cpp
    src/hooks/hooks.cpp
    src/hooks/input.cpp
    src/features/esp.cpp
    src/features/aimbot.cpp
    src/features/chams.cpp
    src/features/antiaim.cpp
    src/menu/menu.cpp
    src/menu/watermark.cpp
    ${IMGUI_SOURCES})

target_include_directories(airware PRIVATE
    src
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
    ${dobby_SOURCE_DIR}/include)

target_compile_definitions(airware PRIVATE
    IMGUI_IMPL_OPENGL_ES3
    IMGUI_IMPL_OPENGL_LOADER_CUSTOM
    GL_GLEXT_PROTOTYPES)

target_link_libraries(airware PRIVATE
    dobby
    android
    log
    GLESv3
    EGL)

set_target_properties(airware PROPERTIES
    PREFIX ""
    OUTPUT_NAME "libairware")

add_executable(injector injector/injector.cpp)
target_link_libraries(injector PRIVATE dl)
set_target_properties(injector PROPERTIES
    PREFIX ""
    OUTPUT_NAME "injector")
