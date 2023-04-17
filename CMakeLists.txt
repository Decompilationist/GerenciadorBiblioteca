cmake_minimum_required(VERSION 3.14)

project(livraria LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets LinguistTools)

set(TS_FILES livraria_pt_BR.ts)

add_library(livraria SHARED
  livraria_global.h
  livraria.cpp
  livraria.h
  ${TS_FILES}
)

target_link_libraries(livraria PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

target_compile_definitions(livraria PRIVATE LIVRARIA_LIBRARY)

if(COMMAND qt_create_translation)
    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()
