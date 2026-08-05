# Unit tests for ECU Mems Manager.
#
# These tests are self-contained: they compile the scr/ecu module against a
# librosco test stub so no serial hardware (or the real librosco) is required.
# Configure the project with -DBUILD_TESTING=ON to build them, then run `ctest`.

find_package(Qt5 REQUIRED COMPONENTS Test)

set(CMAKE_AUTOMOC ON)

add_executable(test_ecu
    test_ecu.cpp
    librosco_stub.cpp
    ${CMAKE_SOURCE_DIR}/scr/ecu/ecufactory.cpp
    ${CMAKE_SOURCE_DIR}/scr/ecu/ecuinterface.cpp
    ${CMAKE_SOURCE_DIR}/scr/ecu/mems12ecu.cpp
    ${CMAKE_SOURCE_DIR}/scr/ecu/mems13ecu.cpp
    ${CMAKE_SOURCE_DIR}/scr/ecu/mems16ecu.cpp
)

target_include_directories(test_ecu PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/scr/ecu
)

target_link_libraries(test_ecu PRIVATE Qt5::Test)

add_test(NAME test_ecu COMMAND test_ecu)
