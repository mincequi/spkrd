file(GLOB_RECURSE GSPEAKERS_SOURCES "*.cc")
list(REMOVE_ITEM GSPEAKERS_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/gfilter.cc")
list(REMOVE_ITEM GSPEAKERS_SOURCES
                 "${CMAKE_CURRENT_SOURCE_DIR}/gspeakersplot2.cc")
list(REMOVE_ITEM GSPEAKERS_SOURCES
                 "${CMAKE_CURRENT_SOURCE_DIR}/speakerlistselector.cc")
list(REMOVE_ITEM GSPEAKERS_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/main.cc")

file(GLOB_RECURSE LIBEGG_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/libegg/*.c")
add_library(libegg ${LIBEGG_SOURCES})
target_link_libraries(libegg PRIVATE ${GTK2_LIBRARIES} LibXml2::LibXml2)
target_include_directories(libegg PUBLIC ${GTK2_INCLUDE_DIRS})

add_library(gspeaker_core ${GSPEAKERS_SOURCES})
target_link_libraries(gspeaker_core
                      PUBLIC LibXml2::LibXml2
                      PRIVATE ${GTK2_LIBRARIES} libegg)
target_include_directories(gspeaker_core
                           PUBLIC ${GTK2_INCLUDE_DIRS}
                                  ${CMAKE_CURRENT_SOURCE_DIR}/libegg)
target_compile_definitions(gspeaker_core
                           PRIVATE
                           GSPEAKERS_PREFIX="${CMAKE_INSTALL_PREFIX}"
                           VERSION="0.13")
                       #$GTKMM_DISABLE_DEPRECATED=0)
target_compile_options(gspeaker_core PRIVATE -Wall -Wpedantic)

if(GSPK_ENABLE_PROFILE)
  set_target_properties(gspeaker_core PROPERTIES COMPILE_FLAGS "-fno-omit-frame-pointer")
endif()
