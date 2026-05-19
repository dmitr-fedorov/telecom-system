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
  src/main.cpp
  src/protocol.cpp
  src/clientapp.cpp
  src/messagefactory.cpp
  src/datagenerator.cpp

  include/protocol.h
  include/clientapp.h
  include/messagefactory.h
  include/datagenerator.h
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_NAME "Server"
)

target_link_libraries(${PROJECT_NAME}
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Network
)

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
