cmake_minimum_required(VERSION 3.5)

project(GUIClient VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
        main.cpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(GUIClient
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        Connection/Client.cpp
        Connection/Client.h
        Auth/LoginWidget.ui
        Auth/LoginWidget.h Auth/loginwidget.cpp
        Auth/AuthWindow.h Auth/AuthWindow.cpp
        Auth/AuthWindow.ui
        Auth/LoginWidget.cpp
        Auth/RegistrationWidget.h
        Auth/RegistrationWidget.cpp
        Auth/RegistrationWidget.ui
        Chat/ChatWindow.h Chat/ChatWindow.cpp
        Chat/ChatWindow.ui
        Chat/DialogWidget.h Chat/DialogWidget.cpp
        Chat/MessageWidget.h Chat/MessageWidget.cpp
        Chat/InputWidget.h Chat/InputWidget.cpp
        Chat/DialogWidget.ui
        Chat/MessageWidget.ui
        Chat/InputWidget.ui
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET GUIClient APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(GUIClient SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(GUIClient
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(GUIClient PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.GUIClient)
endif()
set_target_properties(GUIClient PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS GUIClient
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(GUIClient)
endif()

include_directories(include build/Desktop_Qt_6_7_0_MSVC2019_64bit-Debug/GUIClient_autogen/include)
