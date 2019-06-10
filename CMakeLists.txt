project(gspeakers LANGUAGES C CXX)
cmake_minimum_required(VERSION 3.10)

option(GSPK_ENABLE_PROFILE "Enable profiling flags" OFF)

# find_package(GTK2 REQUIRED COMPONENTS gtk gtkmm)
find_package(LibXml2 REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0 gtkmm-3.0 gdkmm-3.0)

add_subdirectory(${CMAKE_SOURCE_DIR}/src)

add_executable(gspeakers ${CMAKE_SOURCE_DIR}/src/main.cpp)
target_link_libraries(gspeakers gspeaker_core LibXml2::LibXml2)

add_subdirectory(pixmaps)
add_subdirectory(xml)

install(TARGETS gspeakers RUNTIME DESTINATION bin)
#install(FILES gspeakers.desktop DESTINATION /usr/share/applications)
