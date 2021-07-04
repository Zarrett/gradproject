cmake_minimum_required(VERSION 3.0.0)

project(untitled3 VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CXX_STANDARD 17)
set(CXX_STANDARD_REQUIRED ON)

find_package( Qt5 REQUIRED COMPONENTS 	Core
                                        SerialPort )

add_executable(${PROJECT_NAME} "main.cpp")


set_target_properties(
     untitled3
     PROPERTIES
     CXX_STANDARD 17
 )

target_sources( untitled3
    PRIVATE
    Adafruit_Fingerprint.h
    Adafruit_Fingerprint.cpp
    enrollExample.hpp
    enrollExample.cpp
    fingerprintExample.hpp
    fingerprintExample.cpp
    deleteExample.hpp
    deleteExample.cpp
    Logger.hpp
    Logger.cpp
    FastPimpl.hpp
    Platform.hpp
)

target_link_libraries(${PROJECT_NAME} Qt5::Core  Qt5::SerialPort)
