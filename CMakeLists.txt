cmake_minimum_required(VERSION 3.16.0)

project(qteletextmaker VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Core Widgets REQUIRED)
qt_standard_project_setup()

add_subdirectory(qteletextwidget)

file (GLOB SOURCES qteletextmaker/*.cpp)
qt_add_executable(qteletextmaker ${SOURCES} qteletextmaker.qrc)

target_link_libraries(qteletextmaker PRIVATE qteletextwidget Qt6::Widgets)

set_target_properties(qteletextmaker PROPERTIES
        WIN32_EXECUTABLE ON
        MACOSX_BUNDLE ON
)

# include examples
install(DIRECTORY
        ${CMAKE_CURRENT_LIST_DIR}/examples/
        DESTINATION ./examples/
)

install(FILES
        ${CMAKE_CURRENT_LIST_DIR}/README.md
        ${CMAKE_CURRENT_LIST_DIR}/LICENSE
        DESTINATION .
)

install(TARGETS qteletextmaker
        BUNDLE DESTINATION .
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_generate_deploy_app_script(
        TARGET qteletextmaker
        FILENAME_VARIABLE deploy_script
        NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})

set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)
if (WIN32)
        set(CPACK_GENERATOR "NSIS")
else ()
        set(CPACK_GENERATOR "TGZ")
endif (WIN32)
include(CPack)
