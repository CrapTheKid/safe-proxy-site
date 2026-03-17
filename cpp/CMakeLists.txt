cmake_minimum_required(VERSION 3.16)
project(safe_proxy_internal LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(SAFE_PROXY_BUILD_RUNNER "Build local dynamic-loader runner" OFF)

add_library(safe_proxy_internal SHARED
    src/dllmain.cpp
    src/core/logger.cpp
    src/core/hook_manager.cpp
    src/core/render_loop.cpp
    src/core/feature_manager.cpp
    src/core/project.cpp
    src/features/example_feature.cpp
    src/sdk/entity.cpp
)

target_include_directories(safe_proxy_internal
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_definitions(safe_proxy_internal
    PRIVATE
        SAFE_PROXY_BUILD_DLL
)

if (MSVC)
    target_compile_options(safe_proxy_internal PRIVATE /W4 /permissive-)
else()
    target_compile_options(safe_proxy_internal PRIVATE -Wall -Wextra -Wpedantic)
endif()

if (SAFE_PROXY_BUILD_RUNNER)
    add_executable(safe_proxy_runner tools/runner.cpp)

    if (MSVC)
        target_compile_options(safe_proxy_runner PRIVATE /W4 /permissive-)
        add_custom_command(TARGET safe_proxy_runner POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_FILE:safe_proxy_internal>
                $<TARGET_FILE_DIR:safe_proxy_runner>
        )
    else()
        target_compile_options(safe_proxy_runner PRIVATE -Wall -Wextra -Wpedantic)
        if (UNIX AND NOT APPLE)
            target_link_libraries(safe_proxy_runner PRIVATE dl)
        endif()
    endif()

    add_dependencies(safe_proxy_runner safe_proxy_internal)
endif()
