cmake_minimum_required(VERSION 2.8.4)
project(calory-counter)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


set(CMAKE_C_FLAGS "-g -Wall -std=c99 -D _POSIX_C_SOURCE=200809L")

FIND_PACKAGE ( Threads REQUIRED )

file( GLOB LIB_SOURCES lib/food.c lib/foodlist.c lib/foodlistnode.c lib/sock.c )
file( GLOB LIB_HEADERS lib/food.h lib/foodlist.h lib/foodlistnode.h lib/sock.h )
add_library( calory-lib ${LIB_SOURCES} ${LIB_HEADERS} )

add_executable(calory-server server/sockethandler.c server/diet-server.c)
add_executable(calory-client client/diet-client.c)

set(LIBS calory-lib)

target_link_libraries(calory-server ${LIBS}  ${CMAKE_THREAD_LIBS_INIT} )
target_link_libraries(calory-client ${LIBS} ${CMAKE_THREAD_LIBS_INIT}  )
