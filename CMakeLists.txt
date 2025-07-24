cmake_minimum_required(VERSION 3.10.0)
project(GameDB VERSION 0.1.0 LANGUAGES C)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output/lib)

#Check if its being compiled on termux

if(DEFINED ENV{TERMUX_VERSION})

    # Set the CMAKE_PREFIX_PATH to Termux prefix dir
    set(CMAKE_PREFIX_PATH "$ENV{HOME}/usr" ${CMAKE_PREFIX_PATH})

endif()

find_package(cJSON CONFIG REQUIRED)

find_package(Curses REQUIRED)

find_package(OpenSSL REQUIRED)





add_executable(GameDB index.c)


# include_directories(${CURSES_INCLUDE_DIRS})
target_include_directories(GameDB PRIVATE ${CURSES_INCLUDE_DIRS} ${OPENSSL_INCLUDE_DIR})
target_link_libraries(GameDB ${CURSES_LIBRARIES} ${OPENSSL_LIBRARIES} cjson menu)


file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/data

     DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/output/bin)


file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/info.json

     DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/output/bin/)


include(CTest)
enable_testing()

add_test(NAME GameDB_test COMMAND GameDB)