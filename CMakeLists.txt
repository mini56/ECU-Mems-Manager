cmake_minimum_required(VERSION 3.16)

set(PROJECTNAME "ECU_Mems_Manager")

string(TOLOWER ${PROJECTNAME} PNAME)
project(${PNAME} CXX)

add_definitions(-DPROJECTNAME=\"${PROJECTNAME}\")

set(VER_MAJOR 0)
set(VER_MINOR 9)
set(VER_PATCH 0)
add_compile_definitions(
    APP_NAME="ECU Mems Manager"
    APP_VERSION="0.9.0"
)

add_definitions("-DVER_MAJOR=${VER_MAJOR}"
                "-DVER_MINOR=${VER_MINOR}"
                "-DVER_PATCH=${VER_PATCH}")

# Configuration de Qt
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 REQUIRED COMPONENTS Widgets SerialPort)

# Inclusions des dossiers de sous-modules
include_directories("${CMAKE_SOURCE_DIR}"
                    "${CMAKE_SOURCE_DIR}/qledindicator"
                    "${CMAKE_SOURCE_DIR}/analogwidgets"
                    "${CMAKE_CURRENT_BINARY_DIR}")

# Génération du fichier help.html
configure_file(
  "help.html.in"
  "${CMAKE_SOURCE_DIR}/help.html"
)

# Liste des fichiers sources du projet (Y COMPRIS LES FICHIERS .ui)
set(SOURCES
  main.cpp
  memsinterface.cpp
  helpviewer.cpp
  logger.cpp
  serialdevenumerator.cpp
  splashprogress.cpp
  mainwindow.cpp
  aboutbox.cpp
  optionsdialog.cpp
  qledindicator/qledindicator.cpp
  analogwidgets/led.cpp
  analogwidgets/functions.cpp
  analogwidgets/widgetwithbackground.cpp
  analogwidgets/manometer.cpp
  analogwidgets/abstractmeter.cpp
  analogwidgets/manometer100.cpp
  analogwidgets/abstractmeter100.cpp
  analogwidgets/manometer900.cpp
  analogwidgets/abstractmeter900.cpp
  
  # --- Fichiers d'interface utilisateur (.ui) ---
  # CMAKE_AUTOUIC générera automatiquement ui_memsinterface.h, ui_mainwindow.h, etc.
  mainwindow.ui
  
  # --- Fichiers de ressources (.qrc) ---
  resources.qrc
)

# Déclaration de l'exécutable
add_executable(${PNAME} ${SOURCES})

# Correctif MSVC pour la compatibilité avec Qt 5.15 et les en-têtes STL
if(MSVC)
    add_compile_definitions(_HAS_STD_BYTE=0)
    target_compile_options(${PNAME} PRIVATE /permissive)
endif()

# --- CONFIGURATION SELON LE SYSTÈME D'EXPLOITATION ---

if(WIN32)
  message(STATUS "Plateforme Windows détectée (MSVC / MinGW).")

  # Empêche l'ouverture d'une fenêtre de terminal noire en arrière-plan sous Windows
  if(MSVC)
    set_target_properties(${PNAME} PROPERTIES WIN32_EXECUTABLE TRUE)
  elseif(MINGW)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mwindows")
  endif()

  # Recherche optionnelle de la bibliothèque rosco sous Windows
  find_library(LIBROSCO_DLL rosco)
  if(LIBROSCO_DLL)
    message(STATUS "Bibliothèque rosco trouvée : ${LIBROSCO_DLL}")
    target_link_libraries(${PNAME} PRIVATE rosco Qt5::Widgets Qt5::SerialPort)
  else()
    message(STATUS "Bibliothèque rosco non trouvée, compilation avec Qt5::Widgets et Qt5::SerialPort.")
    target_link_libraries(${PNAME} PRIVATE Qt5::Widgets Qt5::SerialPort)  # <-- Ajoute Qt5::SerialPort ici
  endif()

  # Recherche optionnelle de la bibliothèque rosco sous Windows
  find_library(LIBROSCO_DLL rosco)
  if(LIBROSCO_DLL)
    message(STATUS "Bibliothèque rosco trouvée : ${LIBROSCO_DLL}")
    target_link_libraries(${PNAME} PRIVATE rosco Qt5::Widgets Qt5::SerialPort)
  else()
    message(STATUS "Bibliothèque rosco non trouvée, compilation avec Qt5::Widgets uniquement.")
    target_link_libraries(${PNAME} PRIVATE Qt5::Widgets)
  endif()

else()
  message(STATUS "Environnement Linux/Unix détecté.")

  # Sur Linux, on lie directement rosco et Qt5
  target_link_libraries(${PNAME} PRIVATE rosco Qt5::Widgets)

  set(CMAKE_SKIP_RPATH TRUE)
  set(CMAKE_INSTALL_PREFIX "/usr")
  set(EXE_FILE "${CMAKE_CURRENT_BINARY_DIR}/${PNAME}")

  install(FILES ${EXE_FILE} DESTINATION "bin"
           PERMISSIONS
            OWNER_READ OWNER_EXECUTE OWNER_WRITE
            GROUP_READ GROUP_EXECUTE
            WORLD_READ WORLD_EXECUTE)

  install(DIRECTORY DESTINATION "share/doc/${PNAME}" DIRECTORY_PERMISSIONS
           OWNER_READ OWNER_EXECUTE OWNER_WRITE
           GROUP_READ GROUP_EXECUTE
           WORLD_READ WORLD_EXECUTE)

  install(FILES "${CMAKE_SOURCE_DIR}/README"
                 "${CMAKE_SOURCE_DIR}/LICENSE"
           DESTINATION "share/doc/${PNAME}"
           PERMISSIONS
            OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)

  # Configuration du paquet Debian (.deb) via CPack
  if("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "x86_64")
    set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE amd64)
  elseif("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "i686")
    set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE i386)
  endif()

  set(CPACK_GENERATOR "DEB")
  set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
  set(CPACK_PACKAGE_VERSION_MAJOR ${VER_MAJOR})
  set(CPACK_PACKAGE_VERSION_MINOR ${VER_MINOR})
  set(CPACK_PACKAGE_VERSION_PATCH ${VER_PATCH})
  set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Colin Bourassa <colin.bourassa@gmail.com>")
  set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Graphical display for data read from Rover MEMS 1.6")
  set(CPACK_DEBIAN_PACKAGE_SECTION "Science")
  set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.13), libstdc++6 (>= 4.6.3), libqt5core5a (>= 5.0.2), libqt5gui5 (>= 5.0.2), libqt5widgets5 (>= 5.0.2)")
  set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-${VER_MAJOR}.${VER_MINOR}.${VER_PATCH}-${CMAKE_SYSTEM_NAME}-${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}")
  set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
  set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README")

  include(CPack)

endif()
