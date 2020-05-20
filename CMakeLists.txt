project(spkrd LANGUAGES CXX)
cmake_minimum_required(VERSION 3.10)

option(GSPK_ENABLE_PROFILE "Enable profiling flags" OFF)

enable_testing()

find_package(LibXml2 REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTKMM3 REQUIRED gtk+-3.0 glibmm-2.4 gtkmm-3.0 gdkmm-3.0)

find_package(Catch2 2.3)

add_subdirectory(${CMAKE_SOURCE_DIR}/src)

add_executable(spkrd ${CMAKE_SOURCE_DIR}/src/main.cpp)
target_link_libraries(spkrd spkrd_core LibXml2::LibXml2 ${GTKMM3_LIBRARIES})

add_subdirectory(pixmaps)
add_subdirectory(xml)

install(TARGETS spkrd RUNTIME DESTINATION bin)
#install(FILES spkrd.desktop DESTINATION /usr/share/applications)

add_subdirectory(test)
