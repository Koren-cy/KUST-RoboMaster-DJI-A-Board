cmake_minimum_required(VERSION 3.15)
project(Test_Framework C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

option(ENABLE_COVERAGE "Enable code coverage" OFF)

# ---- Output directories ----
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# ---- Compile flags ----
if(MSVC)
    add_compile_options(/W4 /WX-)
else()
    add_compile_options(-Wall -Wextra -Wno-variadic-macros -Wno-long-long)
endif()

# ---- Absolute paths ----
set(PROJECT_ROOT "C:/Users/Koren/RoboMaster/KUST-RoboMaster-DJI-A-Board")

# ---- Build src staging directory ----
# Files are copied from the firmware project into a build-local directory so that
# relative includes like #include "../../Core/Inc/bsp.h" resolve correctly.
set(BUILD_SRC ${CMAKE_BINARY_DIR}/src)

file(MAKE_DIRECTORY ${BUILD_SRC}/User_Algorithm/Src)
file(MAKE_DIRECTORY ${BUILD_SRC}/User_Algorithm/Inc)
file(MAKE_DIRECTORY ${BUILD_SRC}/Core/Inc)

# Copy algorithm source and header files
foreach(name user_newton user_sgdm)
    configure_file(${PROJECT_ROOT}/User_Algorithm/Src/${name}.c
                   ${BUILD_SRC}/User_Algorithm/Src/${name}.c COPYONLY)
    configure_file(${PROJECT_ROOT}/User_Algorithm/${name}.h
                   ${BUILD_SRC}/User_Algorithm/${name}.h COPYONLY)
    configure_file(${PROJECT_ROOT}/User_Algorithm/${name}.h
                   ${BUILD_SRC}/User_Algorithm/Inc/${name}.h COPYONLY)
endforeach()

# Copy bsp_config.h (needed transitively by bsp.h)
configure_file(${PROJECT_ROOT}/Core/Inc/bsp_config.h
               ${BUILD_SRC}/Core/Inc/bsp_config.h COPYONLY)

# Place mock bsp.h (overrides the real one)
configure_file(${PROJECT_ROOT}/Test_Framework/mocks/Core/Inc/bsp.h
               ${BUILD_SRC}/Core/Inc/bsp.h COPYONLY)

# ---- Test sources ----
file(GLOB TEST_SOURCES "${CMAKE_SOURCE_DIR}/tests/*.c")

# ---- Build the unified test executable ----
add_executable(all_tests
    runner/test_runner.c
    unity/unity.c
    ${TEST_SOURCES}
)

target_sources(all_tests PRIVATE
    ${BUILD_SRC}/User_Algorithm/Src/user_newton.c
    ${BUILD_SRC}/User_Algorithm/Src/user_sgdm.c
)

# Include paths:
#   mocks/           -> mock main.h, stm32f4xx_hal.h, SEGGER_RTT.h
#   unity/           -> unity.h
#   tests/           -> test source files
#   build/src/       -> copied source files (with relative includes resolved)
#   build/src/User_Algorithm  -> copied headers
#   build/src/Core/Inc        -> mock bsp.h
target_include_directories(all_tests PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/unity
    ${CMAKE_SOURCE_DIR}/mocks
    ${CMAKE_SOURCE_DIR}/tests
    ${BUILD_SRC}
    ${BUILD_SRC}/User_Algorithm
    ${BUILD_SRC}/User_Algorithm/Inc
    ${BUILD_SRC}/Core/Inc
)

# ---- Coverage ----
if(ENABLE_COVERAGE)
    target_compile_options(all_tests PRIVATE --coverage)
    target_link_options(all_tests PRIVATE --coverage)
endif()

# ---- Convenience targets ----
add_custom_target(run_tests
    COMMAND ${CMAKE_CTEST} --output-on-failure
    DEPENDS all_tests
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

add_custom_target(test
    COMMAND $<TARGET_FILE:all_tests>
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
