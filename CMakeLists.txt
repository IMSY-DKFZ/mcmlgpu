cmake_minimum_required(VERSION 3.18 FATAL_ERROR)

project(MCML LANGUAGES CXX CUDA VERSION 0.0.4)

enable_language(CUDA)
set(PYBIND11_FINDPYTHON ON)
find_package(pybind11 CONFIG REQUIRED)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -no-pie")

include_directories(${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_BINARY_DIR})

# Add definitions for project build details
# build date definitions
string(TIMESTAMP CURRENT_DATETIME "%Y-%m-%d %H:%M:%S")
add_definitions("-DBUILD_TIMESTAMP=\"${CURRENT_DATETIME}\"")
# compiler definitions
add_definitions("-DCMAKE_CXX_COMPILER=\"${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}\"")
# version definitions
add_definitions("-DPROJECT_VERSION_MAJOR=\"${PROJECT_VERSION_MAJOR}\"")
add_definitions("-DPROJECT_VERSION_MINOR=\"${PROJECT_VERSION_MINOR}\"")
add_definitions("-DPROJECT_VERSION_PATCH=\"${PROJECT_VERSION_PATCH}\"")
# git definitions
execute_process(
    COMMAND git rev-parse HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_COMMIT
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
add_definitions("-DGIT_COMMIT=\"${GIT_COMMIT}\"")
# system definitions
add_definitions("-DCMAKE_SYSTEM=\"${CMAKE_SYSTEM}\"")
add_definitions("-DCMAKE_SYSTEM_PROCESSOR=\"${CMAKE_SYSTEM_PROCESSOR}\"")

# Set the NVCC flags
if(NOT DEFINED CUDA_ARCH)
  set(CUDA_ARCH 86)
endif()
set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -O3 -DUNIX --use_fast_math -Xptxas -v -lineinfo")

# CPU code
add_library(mcml_io STATIC src/gpumcml_io.cpp)

# CUDA source files
set(CUDA_SRCS src/gpumcml_main.cu src/gpumcml_kernel.cu src/gpumcml_mem.cu src/gpumcml_rng.cu)

pybind11_add_module(mcmlgpu src/pybinding.cpp ${CUDA_SRCS} src/gpumcml_io.cpp)
set_target_properties(
    mcmlgpu
    PROPERTIES
    CUDA_SEPARABLE_COMPILATION ON
    CUDA_ARCHITECTURES ${CUDA_ARCH}
)
if (NOT DEFINED SKBUILD_PROJECT_NAME OR SKBUILD_PROJECT_NAME STREQUAL "")
    set(SKBUILD_PROJECT_NAME ".")
endif()
install(TARGETS mcmlgpu LIBRARY DESTINATION ${SKBUILD_PROJECT_NAME})
install(FILES resources/safeprimes_base32.txt DESTINATION ${SKBUILD_PROJECT_NAME})

# GPU executable
add_executable(
        MCML
        ${CUDA_SRCS}
)
set_target_properties(
    MCML
    PROPERTIES
    CUDA_SEPARABLE_COMPILATION ON
)
set_property(TARGET MCML PROPERTY CUDA_ARCHITECTURES ${CUDA_ARCH})
target_link_libraries(
        MCML
        cuda
        cudart
        mcml_io
)

file(COPY resources/safeprimes_base32.txt DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# Setup the installation target
install(TARGETS MCML
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib/static)
install(FILES resources/safeprimes_base32.txt DESTINATION bin)

# uninstall configuration
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)

# DEB packaging
include(InstallRequiredSystemLibraries)
set(CPACK_GENERATOR "DEB")
set(CPACK_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}-${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}-${CMAKE_SYSTEM_NAME}-cuda${CUDA_VERSION_STRING}-sm${CUDA_ARCH}")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Leonardo Ayala")
set(CPACK_PACKAGE_DESCRIPTION "Application to run Monte Carlo simulations of light transport in turbid media")
set(CPACK_PACKAGE_HOMEPAGE_URL "https://github.com/IMSY-DKFZ/mcmlgpu")
include(CPack)
