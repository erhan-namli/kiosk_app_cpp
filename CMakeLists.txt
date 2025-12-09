cmake_minimum_required(VERSION 3.16)

project(PhotoScannerKiosk VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find Qt6
find_package(Qt6 REQUIRED COMPONENTS Core Gui Quick Qml Network Concurrent)

# Find required libraries
find_package(CURL REQUIRED)
find_package(JPEG REQUIRED)
find_package(TIFF REQUIRED)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CURL_INCLUDE_DIRS}
    ${JPEG_INCLUDE_DIRS}
    ${TIFF_INCLUDE_DIRS}
)

# Source files
set(SOURCES
    src/main.cpp
    src/AppController.cpp
    src/ScannerManager.cpp
    src/PaymentManager.cpp
    src/EmailManager.cpp
    src/ImageProcessor.cpp
    src/AppSettings.cpp
)

# Header files
set(HEADERS
    include/AppController.h
    include/ScannerManager.h
    include/PaymentManager.h
    include/EmailManager.h
    include/ImageProcessor.h
    include/AppSettings.h
)

# QML files - will be added to resources
qt6_add_resources(RESOURCES qml/qml.qrc)

# Create executable
add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${HEADERS}
    ${RESOURCES}
)

# Link libraries
target_link_libraries(${PROJECT_NAME}
    Qt6::Core
    Qt6::Gui
    Qt6::Quick
    Qt6::Qml
    Qt6::Network
    Qt6::Concurrent
    ${CURL_LIBRARIES}
    ${JPEG_LIBRARIES}
    ${TIFF_LIBRARIES}
    sane
)

# Install target
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)

# Install desktop file for kiosk mode
install(FILES resources/photo-scanner-kiosk.desktop
    DESTINATION /etc/xdg/autostart
    OPTIONAL
)
