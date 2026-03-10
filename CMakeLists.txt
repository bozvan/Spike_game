cmake_minimum_required(VERSION 3.8)
project(Spike_game)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m64")

# Укажите путь к новой, правильно подобранной версии SFML
set(SFML_DIR "C:/Users/ivanb/Downloads/SFML-3.0.2-windows-gcc-14.2.0-mingw-64-bit/SFML-3.0.2")
set(SFML_INCLUDE_DIR "${SFML_DIR}/include")
set(SFML_LIB_DIR "${SFML_DIR}/lib")
set(SFML_BIN_DIR "${SFML_DIR}/bin")

file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")
add_executable(Spike_game ${SOURCES} ${HEADERS})

target_include_directories(Spike_game PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${SFML_INCLUDE_DIR}
)

target_link_directories(Spike_game PRIVATE ${SFML_LIB_DIR})

# Динамическая линковка (обычные библиотеки без -s)
target_link_libraries(Spike_game
    ${SFML_LIB_DIR}/libsfml-graphics.a
    ${SFML_LIB_DIR}/libsfml-window.a
    ${SFML_LIB_DIR}/libsfml-system.a
)

# Копирование DLL
add_custom_command(TARGET Spike_game POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${SFML_BIN_DIR}/sfml-graphics-3.dll"
        "$<TARGET_FILE_DIR:Spike_game>"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${SFML_BIN_DIR}/sfml-window-3.dll"
        "$<TARGET_FILE_DIR:Spike_game>"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${SFML_BIN_DIR}/sfml-system-3.dll"
        "$<TARGET_FILE_DIR:Spike_game>"
)

message(STATUS "C++ compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "Compiler ID: ${CMAKE_CXX_COMPILER_ID}")
