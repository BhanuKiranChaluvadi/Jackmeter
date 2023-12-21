project(jackmeter)
cmake_minimum_required(VERSION 3.14)

set(COMMON_FLAGS
    -Wall
    -Wextra
    -Werror
)

set(COMMON_C_FLAGS
    ${COMMON_FLAGS}
    -std=c11
)

set(COMMON_CXX_FLAGS
    ${COMMON_FLAGS}
    -std=c++2a
)

string(REPLACE ";" " " CMAKE_COMMON_C_FLAGS "${COMMON_C_FLAGS}")
string(REPLACE ";" " " CMAKE_COMMON_CXX_FLAGS "${COMMON_CXX_FLAGS}")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CMAKE_COMMON_C_FLAGS}" CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  ${CMAKE_COMMON_CXX_FLAGS}" CACHE STRING "" FORCE)

find_package(PkgConfig REQUIRED)
pkg_check_modules(JACK REQUIRED jack)
find_package(fmt REQUIRED)
find_package(Curses REQUIRED)

add_executable(${PROJECT_NAME} 
  src/jackmeter.cpp
  src/JackClient.cpp
  src/SimplePeakProcessor.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE 
  include)

target_link_libraries(${PROJECT_NAME} ${JACK_LIBRARIES} fmt::fmt ${CURSES_LIBRARY})

install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_BINDIR})