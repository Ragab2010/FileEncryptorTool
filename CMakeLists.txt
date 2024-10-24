cmake_minimum_required(VERSION 3.14)

# Set the project name and specify the C++ standard
project(FileEncryptorTool LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find the Qt5 packages required for the project, including Widgets for QFileDialog
find_package(Qt5 COMPONENTS Core Gui Quick Qml Widgets Concurrent REQUIRED)

# Find OpenSSL for encryption/decryption
find_package(OpenSSL REQUIRED)

# Add the executable
add_executable(FileEncryptorTool
    main.cpp
    fileEncryptor.cpp
    MainApp.cpp
    ${CMAKE_SOURCE_DIR}/qml/MainWindow.qml
)

# Enable AUTOMOC for the target
set_target_properties(FileEncryptorTool PROPERTIES
    AUTOMOC ON
)

# Add resources (like QML files)
qt5_add_resources(QT_RESOURCES resources.qrc)

# Link the Qt5 libraries and OpenSSL
target_link_libraries(FileEncryptorTool PRIVATE
    Qt5::Core
    Qt5::Gui
    Qt5::Quick
    Qt5::Qml
    Qt5::Widgets  # Added Widgets for QFileDialog
    OpenSSL::SSL
    OpenSSL::Crypto
    Qt5::Concurrent

)

# Specify the resource file to be added in the build
target_sources(FileEncryptorTool PRIVATE ${QT_RESOURCES})
