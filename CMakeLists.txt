cmake_minimum_required(VERSION 3.16)
project(VocalTrainer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Найти Qt
find_package(Qt6 REQUIRED COMPONENTS Widgets Core)

# Найти PortAudio через pkg-config
find_package(PkgConfig REQUIRED)
pkg_check_modules(PA REQUIRED IMPORTED_TARGET portaudio-2.0)

# Собираем приложение
add_executable(${PROJECT_NAME}
    main.cpp
    MainWindow.cpp
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    WIN32_EXECUTABLE TRUE
)

# Подключаем Qt и PortAudio
target_link_libraries(${PROJECT_NAME}
    Qt6::Widgets
    PkgConfig::PA  # ← вот так просто!
)