project(jackmeter)
cmake_minimum_required(VERSION 3.14)

# Set the CMP0048 policy to NEW
cmake_policy(SET CMP0048 NEW)
project(jackmeter)

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


#############################################
# Unit tests

# enable CTest testing
enable_testing()

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Add a testing executable
add_executable(
  jackclient_tests 
  tests/test_SimplePeakProcessor.cpp
  src/SimplePeakProcessor.cpp
)

target_link_libraries(
  jackclient_tests
  gtest_main
  gtest
  # other libraries your tests might need
)

target_include_directories(jackclient_tests PRIVATE 
  ${googletest_SOURCE_DIR}/include 
  ${googletest_SOURCE_DIR}
  ${googletest_SOURCE_DIR}/googlemock/include
  ${googletest_SOURCE_DIR}/googlemock
  include
)

include(GoogleTest)
gtest_discover_tests(jackclient_tests)