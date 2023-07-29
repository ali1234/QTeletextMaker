file (GLOB SOURCES *.cpp)

add_library(qteletextwidget ${SOURCES})

target_include_directories(qteletextwidget PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(qteletextwidget Qt5::Widgets)