cmake_minimum_required(VERSION 3.4)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Check if sub project
if(DEFINED PROJECT_NAME)
    set(IS_SUBPROJECT ON)
endif()

project(libsroc
    VERSION 0.0.1
    DESCRIPTION "Small Read-Only Configuration"
    LANGUAGES C
)

set(PACKAGE_VERSION_STRING v${PROJECT_VERSION})
set(PROJECT_STRING "${PROJECT_NAME} ${PACKAGE_VERSION_STRING}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_SRCS_DIRECTORY ${PROJECT_SOURCE_DIR}/src)

add_compile_definitions(SROC_VERSION_MAJOR=${PROJECT_VERSION_MAJOR})
add_compile_definitions(SROC_VERSION_MINOR=${PROJECT_VERSION_MINOR})
add_compile_definitions(SROC_VERSION_PATCH=${PROJECT_VERSION_PATCH})
add_compile_definitions(SROC_VERSION_STRING="${PROJECT_STRING}")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(IS_DEBUG_BUILD=1)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_definitions(IS_DEBUG_BUILD=0)
else()
    message(STATUS "No CMAKE_BUILD_TYPE specified - Defaulting to \"Debug\"")
    set(CMAKE_BUILD_TYPE "Debug")
    add_compile_definitions(IS_DEBUG_BUILD=1)
endif()

option(SROC_ENABLE_TESTING "Enable automated testing" OFF)
option(SROC_WITH_EXAMPLES "Build example projects" OFF)

add_library(sroc SHARED
    src/sroc.c
    src/parse_helper.h
    src/parse_helper.c
    src/string_helper.h
    src/string_helper.c
)

add_library(SROC::sroc ALIAS sroc)

target_include_directories(sroc
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src
)

# Clang Format target

set(CLANG_FORMAT_POSTFIX "-7.0")
find_program(CLANG_FORMAT
    NAMES clang-format${CLANG_FORMAT_POSTFIX}
          clang-format
)

if(CLANG_FORMAT)
    add_custom_target(clang-format
        COMMAND find ${CMAKE_SRCS_DIRECTORY} -iname *.h -o -iname *.c | xargs ${CLANG_FORMAT} -i
        COMMENT "Running clang-format against *.{c,h} inside ${CMAKE_SRCS_DIRECTORY}"
    )
else()
    message(WARNING "Unable to find clang-format! Target was not created")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Compilation flags

string(APPEND CMAKE_C_FLAGS_DEBUG " -g3"
    " -O")

string(APPEND CMAKE_C_FLAGS_RELEASE " -O2"
    " -march=native"
    " -mtune=native"
    " -ftree-vectorize")

string(APPEND CMAKE_C_FLAGS " -Wall"
    " -D_FORTIFY_SOURCE=2"
    " -Wextra -Wcast-align -Wcast-qual -Wpointer-arith"
    " -Waggregate-return -Wunreachable-code -Wfloat-equal"
    " -Wformat=2 -Wredundant-decls -Wundef"
    " -Wdisabled-optimization -Wshadow -Wmissing-braces"
    " -Wstrict-aliasing=2 -Wstrict-overflow=5 -Wconversion"
    " -Wno-unused-parameter"
    " -pedantic")

if(SROC_ENABLE_TESTING AND NOT IS_SUBPROJECT)
    include(CTest)
    include(AddSrocTest)
    # Add a target for calling ctest with verbose output on failure
    add_custom_target(check
        COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    )

    add_subdirectory(vendor)
    add_subdirectory(tests)
endif()

if (SROC_WITH_EXAMPLES AND NOT IS_SUBPROJECT)
    add_subdirectory(examples)
endif()

if (NOT IS_SUBPROJECT)
    include(GNUInstallDirs)

    set(INSTALL_CONFIG_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/sroc)

    install(TARGETS sroc
        EXPORT sroc-targets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

    install(DIRECTORY include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    )

    install(EXPORT sroc-targets
        FILE
            srocTargets.cmake
        NAMESPACE
            SROC::
        DESTINATION
            ${INSTALL_CONFIG_DIR}
    )

    include(CMakePackageConfigHelpers)

    write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/srocUtilsConfigVersion.cmake
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion
    )

    configure_file(libsroc.pc.in libsroc.pc @ONLY)

    install(FILES ${CMAKE_BINARY_DIR}/libsroc.pc
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkconfig
    )
endif()

