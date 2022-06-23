cmake_minimum_required(VERSION 3.4.1)

project(midlayer_plc_ctl)

message(STATUS "src dir = ${CMAKE_SOURCE_DIR}")
message(STATUS "binary dir = ${CMAKE_BINARY_DIR}")
message(STATUS "project dir = ${PROJECT_SOURCE_DIR}")

#set(CMAKE_BUILD_TYPE Release)
set(CMAKE_BUILD_TYPE Debug)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR})
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/libs)

find_package(Boost 1.79 REQUIRED COMPONENTS
            timer json thread 
            PATHS /usr/lib/boost-1.79 NO_DEFAULT_PATH)


#find_package(zmq REQUIRED)
#find_package(modbus REQUIRED)
find_package(PkgConfig REQUIRED)

pkg_check_modules(zmq REQUIRED IMPORTED_TARGET libzmq)
pkg_check_modules(modbus REQUIRED IMPORTED_TARGET libmodbus)

message(STATUS "Boost_INCLUDE_DIRS  = ${Boost_INCLUDE_DIRS}")
message(STATUS "Boost_LIBRARIES  = ${Boost_LIBRARIES}")

message(STATUS "zmq_INCLUDE_DIRS  = ${zmq_INCLUDE_DIRS}")
message(STATUS "zmq_LIBRARIES  = ${zmq_LIBRARIES}")

message(STATUS "modbus_INCLUDE_DIRS  = ${modbus_INCLUDE_DIRS}")
message(STATUS "modbus_LIBRARIES  = ${modbus_LIBRARIES}")

include_directories( 
    ./
    ${Boost_INCLUDE_DIRS}
    ${zmq_INCLUDE_DIRS}
    ${modbus_INCLUDE_DIRS}
)


add_executable(mlpc
    main.cpp
)

target_link_libraries(mlpc
  ${Boost_LIBRARIES}
  ${zmq_LIBRARIES}
  ${modbus_LIBRARIES}
)

add_executable(testsend
    testSend.cpp
)

target_link_libraries(testsend
  ${Boost_LIBRARIES}
  ${zmq_LIBRARIES}
  ${modbus_LIBRARIES}
)