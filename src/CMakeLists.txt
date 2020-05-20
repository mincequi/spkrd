
file(GLOB_RECURSE SPKRD_SOURCES "*.cc" "*.cpp")
list(REMOVE_ITEM SPKRD_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp")

add_library(spkrd_core ${SPKRD_SOURCES})
target_link_libraries(spkrd_core
PUBLIC LibXml2::LibXml2 ${GTKMM3_LIBRARIES})

target_include_directories(spkrd_core
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
                                   ${CMAKE_CURRENT_SOURCE_DIR}/crossover
                                   ${CMAKE_CURRENT_SOURCE_DIR}/driver
                                   ${CMAKE_CURRENT_SOURCE_DIR}/enclosure
                                   ${CMAKE_CURRENT_SOURCE_DIR}/plot
                           PUBLIC ${GTKMM3_INCLUDE_DIRS})

target_compile_definitions(spkrd_core
                           PRIVATE
                           GSPEAKERS_PREFIX="${CMAKE_INSTALL_PREFIX}"
                           VERSION="0.13")
target_compile_options(spkrd_core PRIVATE -Wall -Wpedantic)

set_target_properties(spkrd_core PROPERTIES CXX_STANDARD 14
                                               CXX_STANDARD_REQUIRED YES
                                               CXX_EXTENSIONS NO)

if(GSPK_ENABLE_PROFILE)
  set_target_properties(spkrd_core PROPERTIES COMPILE_FLAGS "-fno-omit-frame-pointer")
endif()
