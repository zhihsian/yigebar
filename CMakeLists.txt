cmake_minimum_required(VERSION 3.14)
project(yigebar)

set(CMAKE_CXX_STANDARD 20)

if (NOT DEFINED CMAKE_INSTALL_PREFIX)
    set(CMAKE_INSTALL_PREFIX "/usr")
endif ()
add_definitions(-DINSTALL_PREFIX="${CMAKE_INSTALL_PREFIX}")

find_package(yaml-cpp REQUIRED)

include_directories(include)

add_executable(yigebar
        src/main.cc
        src/YigeBar.cc include/YigeBar.h
        src/X11.cc include/X11.h
        src/Atoms.cc include/Atoms.h
        src/Painter.cc include/Painter.h
        src/ModuleLoader.cc include/ModuleLoader.h
        src/Bridge.cc include/Bridge.h)

target_link_libraries(yigebar
        pthread
        xcb
        cairo
        glog
        yaml-cpp
        ${CMAKE_DL_LIBS}
        )

add_library(time MODULE
        modules/time/main.cc
        modules/time/Time.cc modules/time/Time.h
        )
set_target_properties(time PROPERTIES PREFIX "")

add_library(text MODULE
        modules/text/main.cc
        modules/text/Text.cc modules/text/Text.h
        )
set_target_properties(text PROPERTIES PREFIX "")

install(TARGETS yigebar time text
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include/yigebar
        LIBRARY DESTINATION lib/yigebar/modules
        )
