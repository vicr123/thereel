cmake_minimum_required(VERSION 3.22.0)

project(thereel VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
include(GNUInstallDirs)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

find_package(Qt6 REQUIRED COMPONENTS Widgets DBus Svg Network Multimedia)
find_package(libcontemporary CONFIG REQUIRED)
#find_library(libcontemporary REQUIRED)

add_compile_definitions(SYSTEM_LIBRARY_DIRECTORY="${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")

set(SOURCES
        controlstrip.cpp
        main.cpp
        mainwindow.cpp

        controlstrip.ui
        mainwindow.ui

        resources.qrc)

set(HEADERS
        "controlstrip.h"
        "mainwindow.h")

add_executable(thereel ${SOURCES} ${HEADERS})
set_target_properties(thereel PROPERTIES
        MACOSX_BUNDLE TRUE)

target_link_libraries(thereel Qt6::Widgets Qt6::DBus Qt6::Svg Qt6::Network Qt6::Multimedia libcontemporary)

install(TARGETS thereel
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        BUNDLE DESTINATION /Applications)