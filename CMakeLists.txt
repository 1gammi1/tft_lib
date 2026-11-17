cmake_minimum_required(VERSION 3.22)

get_filename_component(PROJECT_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)

project(${PROJECT_NAME})

enable_language(CXX C ASM)

add_library(${PROJECT_NAME} INTERFACE)

file(GLOB_RECURSE SOURCE_FILES 
        "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/*.c")

file(GLOB_RECURSE HEADERS_FILES 
        "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

set(CUBEMX_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/stm32cubemx)

target_include_directories(${PROJECT_NAME} INTERFACE 
        ${CMAKE_CURRENT_SOURCE_DIR} 
        ${CUBEMX_DIR})

target_sources(${PROJECT_NAME} INTERFACE ${SOURCE_FILES} ${HEADERS_FILES})

target_link_directories(${PROJECT_NAME} INTERFACE
)

target_link_libraries(${PROJECT_NAME} INTERFACE
)

# Служебный таргет: компилирует исходники ради подсветки в редакторе.
# В обычную сборку потребителя не попадает.
option(TFT_LIB_ENABLE_LINT "Собирать исходники библиотеки для проверки синтаксиса" ON)

if(TFT_LIB_ENABLE_LINT)
    add_library(tft_lib_lint STATIC EXCLUDE_FROM_ALL ${SOURCE_FILES})
    target_include_directories(tft_lib_lint PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CUBEMX_DIR})
    target_link_libraries(tft_lib_lint PRIVATE ${PROJECT_NAME})
endif()
