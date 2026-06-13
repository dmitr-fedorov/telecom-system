cmake_minimum_required(VERSION 3.16)

project(Server LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Network)

set(CMAKE_AUTOUIC_SEARCH_PATHS
    ${CMAKE_SOURCE_DIR}/server/forms
)

add_executable(${PROJECT_NAME}
    include/mainwindow.h
    include/configlimitsdialog.h
    include/tcpservercontroller.h
    include/tcpserver.h

    src/mainwindow.cpp
    src/configlimitsdialog.cpp
    src/tcpservercontroller.cpp
    src/tcpserver.cpp
    src/main.cpp

    forms/mainwindow.ui
    forms/configlimitsdialog.ui
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_NAME ${PROJECT_NAME}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Widgets
    Qt6::Network
    Shared
)
