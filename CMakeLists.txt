cmake_minimum_required(VERSION 3.24)
project(ProceduralAnimation)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS NO)

add_compile_options(
        -Wall
        #-Wextra
        #-Werror
        #-pedantic-errors
        # -Wconversion
        # -Wsign-conversion
        -Wformat=2
        -Wuninitialized
        -Wnull-dereference
        -Wnon-virtual-dtor
        -Woverloaded-virtual
        -Wdeprecated-copy-dtor
        -Wsuggest-override
        -Wno-old-style-cast
        -Wno-write-strings
)

# debug builds should be compiled with sanitizers
set(CMAKE_CXX_FLAGS_DEBUG_INIT "-fsanitize=address,undefined")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO_INIT "-fsanitize=address,undefined")
set(CMAKE_CXX_EXE_LINKER_FLAGS_DEBUG_INIT "-fsanitize=address,undefined")
set(CMAKE_CXX_EXE_LINKER_FLAGS_RELWITHDEBINFO_INIT "-fsanitize=address,undefined")

# include directories
include_directories(
        include/
        include/raylib/
        include/imgui/
		src/
)

# link directories
link_directories(
        lib/
)

# add the imgui library
add_library(imgui STATIC
        include/imgui/imconfig.h
        include/imgui/imgui.cpp
        include/imgui/imgui.h
        include/imgui/imgui_demo.cpp
        include/imgui/imgui_draw.cpp
        include/imgui/imgui_widgets.cpp
        include/imgui/imgui_tables.cpp
        # rlImGUI being linked
        include/raylib/rlImGuiColors.h
        include/raylib/rlImGui.cpp
        include/raylib/rlImGui.h
        )


add_executable(ProceduralAnimation
		src/main.cpp
		src/PVector.h src/ParticleSystem.h)

target_link_libraries(ProceduralAnimation PRIVATE raylib winmm imgui)

