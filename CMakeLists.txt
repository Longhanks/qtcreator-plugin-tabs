cmake_minimum_required(VERSION 3.14)

if (APPLE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.12" CACHE STRING "Minimum OS X deployment version" FORCE)
    find_program(BREW_EXECUTABLE brew HINTS /usr/local/bin)
    if ("${BREW_EXECUTABLE}" STREQUAL "BREW_EXECUTABLE-NOTFOUND")
        message(STATUS "macOS: Homebrew not found.")
    else ()
        message(STATUS "macOS: Homebrew found at ${BREW_EXECUTABLE}.")
        execute_process(COMMAND ${BREW_EXECUTABLE} --prefix qt OUTPUT_VARIABLE QT_PREFIX_PATH)
        string(REGEX REPLACE "\n$" "" QT_PREFIX_PATH_STRIPPED "${QT_PREFIX_PATH}")
        if ("${QT_PREFIX_PATH}" STREQUAL "")
            message(STATUS "macOS: Homebrew: Qt not installed.")
        else ()
            list(APPEND CMAKE_PREFIX_PATH "${QT_PREFIX_PATH_STRIPPED}")
            message(STATUS "macOS: Homebrew Qt at ${QT_PREFIX_PATH_STRIPPED}.")
        endif ()
    endif ()
endif ()

set(QTCREATOR_SRC "" CACHE PATH "Path to Qt Creator sources")
set(QTCREATOR_BIN "" CACHE FILEPATH "Path to Qt Creator binary")
set(QTCREATOR_VERSION "4.9.0" CACHE STRING "Target version of Qt Creator")

if (NOT EXISTS "${QTCREATOR_SRC}/src/qtcreatorplugin.pri")
    message(FATAL_ERROR "QTCREATOR_SRC must point to Qt Creator sources.")
endif ()

if (NOT EXISTS "${QTCREATOR_BIN}")
    message(FATAL_ERROR "QTCREATOR_BIN must point to the Qt Creator executable.")
endif ()

get_filename_component(QTCREATOR_BIN_DIR "${QTCREATOR_BIN}" DIRECTORY)

enable_language(CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 17)

cmake_policy(SET CMP0020 NEW)
cmake_policy(SET CMP0071 NEW)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

project(tabs LANGUAGES CXX VERSION 0.6.0)

configure_file("${CMAKE_SOURCE_DIR}/tabs.json.in" "${CMAKE_CURRENT_BINARY_DIR}/tabs.json")

add_library(tabs SHARED plugin.cpp tabbar.cpp)

set_target_properties(tabs PROPERTIES AUTOMOC ON)

target_include_directories(tabs PRIVATE
    "${CMAKE_CURRENT_BINARY_DIR}"
    "${Qt5Widgets_INCLUDE_DIRS}"
    "${QTCREATOR_SRC}/src/libs"
    "${QTCREATOR_SRC}/src/plugins"
    )

if (APPLE)
    find_library(CORE_LIB NAMES Core PATHS "${QTCREATOR_BIN_DIR}/../PlugIns")
    find_library(EXTENSIONSYSTEM_LIB NAMES ExtensionSystem PATHS "${QTCREATOR_BIN_DIR}/../Frameworks")
    find_library(PROJECTEXPLORER_LIB NAMES ProjectExplorer PATHS "${QTCREATOR_BIN_DIR}/../PlugIns")
    find_library(TEXTEDITOR_LIB NAMES TextEditor PATHS "${QTCREATOR_BIN_DIR}/../PlugIns")
    find_library(UTILS_LIB NAMES Utils PATHS "${QTCREATOR_BIN_DIR}/../Frameworks")
    find_library(QTWIDGETS_LIB NAMES QtWidgets PATHS "${QTCREATOR_BIN_DIR}/../Frameworks" NO_DEFAULT_PATH)
    find_library(QTGUI_LIB NAMES QtGui PATHS "${QTCREATOR_BIN_DIR}/../Frameworks" NO_DEFAULT_PATH)
    find_library(QTCORE_LIB NAMES QtCore PATHS "${QTCREATOR_BIN_DIR}/../Frameworks" NO_DEFAULT_PATH)
    target_link_options(tabs PRIVATE
        "-Wl,-install_name,@rpath/PlugIns/"
        "-compatibility_version ${QTCREATOR_VERSION}"
    )
else ()
    set(QTWIDGETS_LIB "${Qt5Widgets_LIBRARIES}")
    # TODO linux
endif ()

target_link_libraries(tabs PRIVATE
    "${CORE_LIB}"
    "${EXTENSIONSYSTEM_LIB}"
    "${PROJECTEXPLORER_LIB}"
    "${TEXTEDITOR_LIB}"
    "${UTILS_LIB}"
    "${QTWIDGETS_LIB}"
    "${QTGUI_LIB}"
    "${QTCORE_LIB}"
)

if (APPLE)
    install(TARGETS tabs DESTINATION "${QTCREATOR_BIN_DIR}/../PlugIns")
    install(CODE "execute_process(COMMAND \"sudo xattr -rd com.apple.quarantine ${QTCREATOR_BIN_DIR}/../../\")")
endif ()
