cmake_minimum_required(VERSION 3.14)
project(Spike_game)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

if(WIN32)
    set(SFML_ROOT "C:/SFML-3.0.2" CACHE PATH "Path to the SFML 3.0.2 root directory (Windows)")
    set(SFML_INCLUDE_DIR "${SFML_ROOT}/include")
    set(SFML_LIB_DIR "${SFML_ROOT}/lib")
    set(SFML_BIN_DIR "${SFML_ROOT}/bin")

    if(NOT EXISTS "${SFML_INCLUDE_DIR}/SFML/Graphics.hpp" OR
       NOT EXISTS "${SFML_LIB_DIR}/libsfml-graphics.a" OR
       NOT EXISTS "${SFML_BIN_DIR}/sfml-graphics-3.dll")
        message(FATAL_ERROR
            "SFML 3.0.2 was not found in '${SFML_ROOT}'. "
            "Install SFML or pass its root directory with -DSFML_ROOT=C:/path/to/SFML-3.0.2")
    endif()
else()
    find_package(SFML 3 COMPONENTS Graphics Window System REQUIRED)
endif()

if(WIN32 AND MINGW)
    get_filename_component(MINGW_BIN_DIR "${CMAKE_CXX_COMPILER}" DIRECTORY)
    set(MINGW_LIBSTDCXX_DLL "${MINGW_BIN_DIR}/libstdc++-6.dll")

    if(EXISTS "${MINGW_LIBSTDCXX_DLL}")
        file(STRINGS "${SFML_BIN_DIR}/sfml-system-3.dll" SFML_SYSTEM_CRT_HINTS
             REGEX "api-ms-win-crt|ucrtbase|msvcrt\\.dll")
        file(STRINGS "${MINGW_LIBSTDCXX_DLL}" MINGW_LIBSTDCXX_CRT_HINTS
             REGEX "api-ms-win-crt|ucrtbase|msvcrt\\.dll")

        string(FIND "${SFML_SYSTEM_CRT_HINTS}" "api-ms-win-crt" SFML_UCRT_API_INDEX)
        string(FIND "${SFML_SYSTEM_CRT_HINTS}" "ucrtbase" SFML_UCRTBASE_INDEX)
        string(FIND "${MINGW_LIBSTDCXX_CRT_HINTS}" "api-ms-win-crt" MINGW_UCRT_API_INDEX)
        string(FIND "${MINGW_LIBSTDCXX_CRT_HINTS}" "ucrtbase" MINGW_UCRTBASE_INDEX)

        if((NOT SFML_UCRT_API_INDEX EQUAL -1 OR NOT SFML_UCRTBASE_INDEX EQUAL -1) AND
           MINGW_UCRT_API_INDEX EQUAL -1 AND MINGW_UCRTBASE_INDEX EQUAL -1)
            message(FATAL_ERROR
                "The selected SFML package uses UCRT, but the selected MinGW libstdc++ does not. "
                "This can build successfully and then fail at startup with 'entry point not found'. "
                "Use the WinLibs UCRT compiler or pass it explicitly, for example "
                "-DCMAKE_C_COMPILER=C:/mingw64/bin/gcc.exe "
                "-DCMAKE_CXX_COMPILER=C:/mingw64/bin/g++.exe. "
                "Detected libstdc++: ${MINGW_LIBSTDCXX_DLL}")
        endif()
    endif()
endif()

set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
set(tinyxml2_BUILD_TESTING OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    tinyxml2
    URL https://github.com/leethomason/tinyxml2/archive/refs/tags/11.0.0.zip
    URL_HASH SHA256=1C35B8D8A586125D5E43380FD2137ABBFDE645183ED1641F86C815359320412F
    # Winlibs CMake may miss the Windows CA store; URL_HASH still verifies the archive.
    TLS_VERIFY FALSE
    DOWNLOAD_EXTRACT_TIMESTAMP FALSE
)
FetchContent_MakeAvailable(tinyxml2)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
)
add_executable(Spike_game ${SOURCES} ${HEADERS})

target_include_directories(Spike_game PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if(WIN32)
    target_include_directories(Spike_game PRIVATE ${SFML_INCLUDE_DIR})
    target_link_directories(Spike_game PRIVATE ${SFML_LIB_DIR})
    target_link_libraries(Spike_game PRIVATE
        tinyxml2
        sfml-graphics
        sfml-window
        sfml-system
    )
else()
    target_link_libraries(Spike_game PRIVATE
        tinyxml2
        SFML::Graphics
        SFML::Window
        SFML::System
    )
endif()

# ---------- Copy runtime files ----------
if(WIN32)
    get_filename_component(MINGW_BIN_DIR "${CMAKE_CXX_COMPILER}" DIRECTORY)

    add_custom_command(TARGET Spike_game POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/assets"
            "$<TARGET_FILE_DIR:Spike_game>/assets"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${SFML_BIN_DIR}/sfml-graphics-3.dll"
            "$<TARGET_FILE_DIR:Spike_game>"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${SFML_BIN_DIR}/sfml-window-3.dll"
            "$<TARGET_FILE_DIR:Spike_game>"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${SFML_BIN_DIR}/sfml-system-3.dll"
            "$<TARGET_FILE_DIR:Spike_game>"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${MINGW_BIN_DIR}/libgcc_s_seh-1.dll"
            "$<TARGET_FILE_DIR:Spike_game>"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${MINGW_BIN_DIR}/libstdc++-6.dll"
            "$<TARGET_FILE_DIR:Spike_game>"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${MINGW_BIN_DIR}/libwinpthread-1.dll"
            "$<TARGET_FILE_DIR:Spike_game>"
    )
else()
    add_custom_command(TARGET Spike_game POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/assets"
            "$<TARGET_FILE_DIR:Spike_game>/assets"
    )
endif()

message(STATUS "C++ compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "Compiler ID: ${CMAKE_CXX_COMPILER_ID}")
