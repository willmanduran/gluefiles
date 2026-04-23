cmake_minimum_required(VERSION 3.16)
project(gluefiles VERSION 1.1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
find_package(Qt6 REQUIRED COMPONENTS Widgets LinguistTools)
include(GNUInstallDirs)

set(GLUEFILES_SOURCES
        main.cpp
        core/Glue.cpp
        ui/MainWindow.cpp
        ui/FileListWidget.cpp
        icons.qrc
)

if (WIN32)
  add_executable(gluefiles WIN32 ${GLUEFILES_SOURCES})
  if (EXISTS "${CMAKE_SOURCE_DIR}/icons/app.rc")
    target_sources(gluefiles PRIVATE icons/app.rc)
  endif()
else()
  add_executable(gluefiles ${GLUEFILES_SOURCES})
endif()

qt_add_translations(gluefiles
        TS_FILES i18n/gluefiles_es.ts
)

target_link_libraries(gluefiles PRIVATE Qt6::Widgets)

if (WIN32)
  install(TARGETS gluefiles RUNTIME DESTINATION .)
  if(EXISTS "${CMAKE_SOURCE_DIR}/cmake/windeployqt.cmake")
    install(SCRIPT "${CMAKE_SOURCE_DIR}/cmake/windeployqt.cmake")
  endif()
else()
  install(TARGETS gluefiles RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()

if (UNIX AND NOT APPLE AND NOT WIN32)
  install(FILES "icons/icon.svg"
          DESTINATION "${CMAKE_INSTALL_DATADIR}/icons/hicolor/scalable/apps"
          RENAME "gluefiles.svg")
  install(FILES "gluefiles.desktop"
          DESTINATION "${CMAKE_INSTALL_DATADIR}/applications")
endif()

set(CPACK_PACKAGE_NAME "gluefiles")
set(CPACK_PACKAGE_VENDOR "Silicon Quality Commons")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
if (WIN32)
  set(CPACK_GENERATOR "NSIS")
  set(CPACK_NSIS_DISPLAY_NAME "gluefiles")
  set(CPACK_PACKAGE_INSTALL_DIRECTORY "gluefiles")
  set(CPACK_NSIS_INSTALLED_ICON_NAME "gluefiles.exe")
  set(CPACK_NSIS_MODIFY_PATH OFF)
  set(CPACK_PACKAGE_EXECUTABLES "gluefiles" "gluefiles")
  set(CPACK_NSIS_EXECUTABLES_DIRECTORY ".")
endif()
if (UNIX AND NOT APPLE AND NOT WIN32)
  set(CPACK_GENERATOR "DEB;TGZ")
  set(CPACK_DEBIAN_PACKAGE_MAINTAINER "willman <ticuette@gmail.com>")
  set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "A utility to glue multiple files into one.")
  set(CPACK_DEBIAN_PACKAGE_SECTION "utils")
  set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
endif()
include(CPack)