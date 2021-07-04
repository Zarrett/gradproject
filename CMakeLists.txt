project(facesQT LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(THREADS_PREFER_PTHREAD_FLAG ON)

set(CONTROLLER_PATH "Controller")
set(USERACC_PATH "UserProfile")
set(FINGERPTINT_PATH "FingerprintLib")

# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check http://doc.qt.io/qt-5/deployment-android.html for more information.
# They need to be set before the find_package(Qt5 ...) call.

#if(ANDROID)
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#    if (ANDROID_ABI STREQUAL "armeabi-v7a")
#        set(ANDROID_EXTRA_LIBS
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so)
#    endif()
#endif()
find_package( Qt5 REQUIRED COMPONENTS 	Core
                                        SerialPort )
find_package(Qt5 COMPONENTS Widgets LinguistTools REQUIRED)
find_package(Threads REQUIRED)

#set(TS_FILES facesQT_en_US.ts)

if(ANDROID)
  add_library(facesQT SHARED
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    ${TS_FILES}
  )
else()
  add_executable(facesQT
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    ${TS_FILES}
  )
endif()

target_link_libraries(facesQT PRIVATE Qt5::Widgets Threads::Threads Qt5::SerialPort)

target_sources(
    facesQT
    PRIVATE
    ${CONTROLLER_PATH}/applicationcontroller.cpp
    ${CONTROLLER_PATH}/applicationcontroller.h
    ${USERACC_PATH}/userprofile.cpp
    ${USERACC_PATH}/userprofile.h
    ${USERACC_PATH}/userprofilecontroller.cpp
    ${USERACC_PATH}/userprofilecontroller.h
    ${FINGERPTINT_PATH}/Adafruit_Fingerprint.h
    ${FINGERPTINT_PATH}/Adafruit_Fingerprint.cpp
    ${FINGERPTINT_PATH}/enrollExample.hpp
    ${FINGERPTINT_PATH}/enrollExample.cpp
    ${FINGERPTINT_PATH}/fingerprintExample.hpp
    ${FINGERPTINT_PATH}/fingerprintExample.cpp
    ${FINGERPTINT_PATH}/deleteExample.hpp
    ${FINGERPTINT_PATH}/deleteExample.cpp
    ${FINGERPTINT_PATH}/Logger.hpp
    ${FINGERPTINT_PATH}/Logger.cpp
    ${FINGERPTINT_PATH}/FastPimpl.hpp
    ${FINGERPTINT_PATH}/Platform.hpp
    )

target_include_directories(
    facesQT
    PUBLIC
    ${CONTROLLER_PATH}
    ${USERACC_PATH}
    ${FINGERPTINT_PATH}
    )

project( DisplayImage )
find_package( OpenCV REQUIRED )
target_link_libraries( facesQT ${OpenCV_LIBS} )

qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
