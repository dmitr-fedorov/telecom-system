cmake_minimum_required(VERSION 3.16)

project(Client LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Core Network)
find_package(Qt6 REQUIRED COMPONENTS Core Network)

add_executable(${PROJECT_NAME}
  include/deviceemulator.h
  include/tcpclient.h
  include/datagenerationscheduler.h
  include/datagenerator.h

  src/main.cpp
  src/deviceemulator.cpp
  src/tcpclient.cpp
  src/datagenerationscheduler.cpp
  src/datagenerator.cpp
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_NAME "Client"
)

target_link_libraries(${PROJECT_NAME}
    Qt6::Core
    Qt6::Network
    Shared
)

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
