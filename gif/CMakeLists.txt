cmake_minimum_required(VERSION 3.25)
project(GIF)



# Настраиваем исходные и заголовочные файлы
set(GIF_HEADERS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/headers")
set(GIF_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/source")
set(GIF_LOCAL_TEST_DIR "${CMAKE_CURRENT_SOURCE_DIR}/local_test")
set(GIF_TEST_DIR "${CMAKE_CURRENT_SOURCE_DIR}/test")
set(IFC_HEADER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../interfaces/")

# Исходники и заголовки
file(GLOB GIF_SRC_FILES "${GIF_SOURCE_DIR}/*.cpp")
file(GLOB GIF_HEADER_FILES "${GIF_HEADERS_DIR}/*.hpp")
file(GLOB IFC_HEADER_FILES "${IFC_HEADER_DIR}/*.hpp")

# Создаем библиотеку
add_library(GifLibrary STATIC ${GIF_SRC_FILES})

add_compile_options(
    -Wall
    -Wextra
    -Wpedantic
    -fpermissive
    -pedantic-errors
    -pthread
    $<$<CONFIG:DEBUG>:-g>
    $<$<CONFIG:TEST>:-g>
    $<$<CONFIG:RELEASE>:-O3>
)

if (WIN32)
    target_compile_definitions(GifLibrary PRIVATE GIF_LIBRARY_EXPORT)
endif()

# 1. Публичные заголовки библиотеки (будут доступны зависимым проектам)
target_include_directories(GifLibrary
    PUBLIC 
        ${GIF_HEADERS_DIR}  # Заголовки подпроекта
        ${IFC_HEADER_DIR}
        $<BUILD_INTERFACE:${GIF_HEADERS_DIR}>  # Для сборки
        $<INSTALL_INTERFACE:include>  # Для установки (опционально)

)

# 3. Тесты (оставляем как было, но используем target_include_directories)
if(BUILD_TYPE STREQUAL "test")
    file(GLOB LOCAL_TEST_SRC_FILES "${GIF_LOCAL_TEST_DIR}/*.cpp")
    foreach(TEST_FILE ${LOCAL_TEST_SRC_FILES})
        get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)
        add_executable(${TEST_NAME} ${TEST_FILE})
        target_link_libraries(${TEST_NAME} PRIVATE GifLibrary gtest gtest_main)
    endforeach()

    file(GLOB TEST_SRC_FILES "${GIF_TEST_DIR}/*.cpp")
    foreach(TEST_FILE ${TEST_SRC_FILES})
        get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)
        add_executable(${TEST_NAME} ${TEST_FILE})
        target_link_libraries(${TEST_NAME} PRIVATE GifLibrary gtest gtest_main)
    endforeach()
endif()