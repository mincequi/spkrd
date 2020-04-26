
file(GLOB_RECURSE GSPEAKERS_SOURCES "*.cc" "*.cpp")
list(REMOVE_ITEM GSPEAKERS_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp")

add_library(gspeaker_core ${GSPEAKERS_SOURCES})
target_link_libraries(gspeaker_core
                      PUBLIC LibXml2::LibXml2 ${GTK3_LIBRARIES})

target_include_directories(gspeaker_core
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
                                   ${CMAKE_CURRENT_SOURCE_DIR}/crossover
                                   ${CMAKE_CURRENT_SOURCE_DIR}/driver
                                   ${CMAKE_CURRENT_SOURCE_DIR}/enclosure
                                   ${CMAKE_CURRENT_SOURCE_DIR}/plot
                           PUBLIC ${GTK3_INCLUDE_DIRS})

target_compile_definitions(gspeaker_core
                           PRIVATE
                           GSPEAKERS_PREFIX="${CMAKE_INSTALL_PREFIX}"
                           VERSION="0.13")
target_compile_options(gspeaker_core PRIVATE -Wall -Wpedantic)

set_target_properties(gspeaker_core PROPERTIES CXX_STANDARD 14
                                               CXX_STANDARD_REQUIRED YES
                                               CXX_EXTENSIONS NO)

if(GSPK_ENABLE_PROFILE)
  set_target_properties(gspeaker_core PROPERTIES COMPILE_FLAGS "-fno-omit-frame-pointer")
endif()
