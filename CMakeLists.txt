cmake_minimum_required(VERSION 3.10.0)
project(GameDB VERSION 0.1.0 LANGUAGES C)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output/lib)

# Check if it's being compiled on Termux
if(DEFINED ENV{TERMUX_VERSION})
    # Set the CMAKE_PREFIX_PATH to Termux prefix dir
    set(CMAKE_PREFIX_PATH "$ENV{HOME}/usr" ${CMAKE_PREFIX_PATH})
endif()

# Option to build a static binary
option(BUILD_STATIC "Build a static binary" OFF)

find_package(cJSON CONFIG REQUIRED)
find_package(Curses REQUIRED)
find_package(OpenSSL REQUIRED)

# Add the executable
add_executable(GameDB index.c)

# Include directories
target_include_directories(GameDB PRIVATE ${CURSES_INCLUDE_DIRS} ${OPENSSL_INCLUDE_DIR})

# Link libraries
target_link_libraries(GameDB ${CURSES_LIBRARIES} ${OPENSSL_LIBRARIES} cjson menu)

# If the BUILD_STATIC option is ON, set the target to be static
if(BUILD_STATIC)
    set_target_properties(GameDB PROPERTIES
        LINK_SEARCH_START_STATIC TRUE
        LINK_SEARCH_END_STATIC TRUE
        STATIC TRUE
    )
endif()

# Copy data files
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/data
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/output/bin)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/info.json
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/output/bin/)

include(CTest)
enable_testing()

add_test(NAME GameDB_test COMMAND GameDB)