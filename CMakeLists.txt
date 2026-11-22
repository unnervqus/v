
cmake_minimum_required(VERSION 3.20)

project(v
    VERSION 0.1.0
    LANGUAGES C
)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# --------------------------------------------------
# Sources
# --------------------------------------------------
 
file(GLOB_RECURSE ENGINE_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/core/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/ui/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/graphics/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/graphics/context/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/platform/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/math/*.c"
)

add_executable(v
    main.c
    ${ENGINE_SOURCES}
)

# --------------------------------------------------
# Include directories
# --------------------------------------------------

target_include_directories(v PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/"
    "${CMAKE_CURRENT_SOURCE_DIR}/external/"
)

# --------------------------------------------------
# OpenGL
# --------------------------------------------------

find_package(OpenGL REQUIRED)

# --------------------------------------------------
# GLEW
# --------------------------------------------------

find_package(GLEW REQUIRED)

# --------------------------------------------------
# Linking
# --------------------------------------------------

target_link_libraries(v PRIVATE
    OpenGL::GL
    GLEW::GLEW
    user32
    gdi32
)

# --------------------------------------------------
# Copy GLEW DLL
# --------------------------------------------------

find_file(GLEW_DLL
    NAMES glew32.dll
    PATHS "$ENV{MSYSTEM_PREFIX}/bin"
    NO_DEFAULT_PATH
)

if(GLEW_DLL)
    add_custom_command(TARGET v POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${GLEW_DLL}"
            "$<TARGET_FILE_DIR:veltrion>"
    )
endif()

# --------------------------------------------------
# Warnings
# --------------------------------------------------

if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
    target_compile_options(v PRIVATE
        -g
        -Wpedantic
    )
endif()
