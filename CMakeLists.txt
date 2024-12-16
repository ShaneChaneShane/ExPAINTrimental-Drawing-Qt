cmake_minimum_required(VERSION 3.16)

project(ExPAINTrimental-Drawing-Qt VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Multimedia)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Multimedia)

set(PROJECT_SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/canvaswidget.cpp
    src/brushstroke.cpp
    src/colorpickerwidget.cpp
    src/colorpalettebutton.cpp
    src/UndoRedoManager.cpp
    src/mainwindow.ui
    resources.qrc
)
set(PROJECT_HEADERS
    include/mainwindow.h
    include/canvaswidget.h
    include/brushstroke.h
    include/colorpickerwidget.h
    include/colorpalettebutton.h
    include/UndoRedoManager.h
)
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(ExPAINTrimental-Drawing-Qt
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        ${PROJECT_HEADERS}

    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET ExPAINTrimental-Drawing-Qt APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(ExPAINTrimental-Drawing-Qt SHARED
            ${PROJECT_SOURCES}
            ${PROJECT_HEADERS}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(ExPAINTrimental-Drawing-Qt
            ${PROJECT_SOURCES}
            ${PROJECT_HEADERS}
        )
    endif()
endif()
target_include_directories(ExPAINTrimental-Drawing-Qt PRIVATE include)

target_link_libraries(ExPAINTrimental-Drawing-Qt PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Multimedia)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.ExPAINTrimental-Drawing-Qt)
endif()
set_target_properties(ExPAINTrimental-Drawing-Qt PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS ExPAINTrimental-Drawing-Qt
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(ExPAINTrimental-Drawing-Qt)
endif()
