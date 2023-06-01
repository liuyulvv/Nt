cmake_minimum_required(VERSION 3.15)
project(NtGraphics LANGUAGES CXX VERSION 0.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(BUILD_NT_TEST "" ON)
option(BUILD_NT_STATIC "" OFF)

add_compile_options(/wd4251)

set(IS_DEBUG_BUILD CMAKE_BUILD_TYPE STREQUAL "Debug")
if(NOT ${IS_DEBUG_BUILD})
    add_compile_definitions(NOT_DEBUG)
endif()

file(GLOB_RECURSE SRCS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE HEADERS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")

set(Vulkan_SDK "D:/VulkanSDK/1.3.236.0")
find_package(Vulkan REQUIRED COMPONENTS glslc)
find_program(glslc_executable NAMES glslc HINTS Vulkan::glslc)
file(GLOB shaders ${CMAKE_CURRENT_SOURCE_DIR}/shaders/Vulkan/*.vert ${CMAKE_CURRENT_SOURCE_DIR}/shaders/Vulkan/*.frag)
foreach(shader IN LISTS shaders)
    get_filename_component(filename ${shader} NAME ABSOLUTE)
    add_custom_command(
        COMMAND
        ${glslc_executable}
        -o ${CMAKE_CURRENT_SOURCE_DIR}/shaders/Vulkan/${filename}.spv
        ${shader}
        OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/shaders/Vulkan/${filename}.spv
        DEPENDS ${shader} ${CMAKE_CURRENT_SOURCE_DIR}/shaders/Vulkan
        COMMENT "Compiling ${filename}"
    )
    list(APPEND spv_shaders ${CMAKE_CURRENT_SOURCE_DIR}/shaders/Vulkan/${filename}.spv)
endforeach()
add_custom_target(shaders ALL DEPENDS ${spv_shaders})

include_directories(
    "include"
    ${Vulkan_INCLUDE_DIRS}
)

add_library(
    ${PROJECT_NAME}
    SHARED
    ${SRCS}
    ${HEADERS}
)

target_link_libraries(
    ${PROJECT_NAME} PRIVATE
    ${Vulkan_LIBRARIES}
)

target_compile_options(
    ${PROJECT_NAME} PRIVATE 
    /EHsc /W4 /WX
)

if(BUILD_NT_STATIC)
    add_library(
        ${PROJECT_NAME}_static
        STATIC
        ${SRCS}
        ${HEADERS}
    )

    target_link_libraries(
        ${PROJECT_NAME}_static PRIVATE
        ${Vulkan_LIBRARIES}
    )

    target_compile_options(
        ${PROJECT_NAME}_static PRIVATE 
        /EHsc /W4 /WX
    )
    install(
        TARGETS
        ${PROJECT_NAME} ${PROJECT_NAME}_static
        RUNTIME DESTINATION dll
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
    )
else()
    install(
        TARGETS
        ${PROJECT_NAME}
        RUNTIME DESTINATION dll
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
    )
endif()

install(FILES ${HEADERS} DESTINATION include)

if(BUILD_NT_TEST)
    file(GLOB_RECURSE TESTS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/test/*.cpp")

    foreach(mainfile IN LISTS TESTS)
        get_filename_component(srcname ${mainfile} NAME_WE)
        add_executable(
            ${srcname} 
            ${mainfile}
        )
        set_target_properties(
            ${srcname} 
            PROPERTIES 
            LINK_FLAGS 
            "/ENTRY:mainCRTStartup /SUBSYSTEM:WINDOWS"
        )
        target_link_libraries(
            ${srcname}
            ${PROJECT_NAME}
        )
        target_compile_options(
            ${srcname} PRIVATE 
            /EHsc /W4 /WX
        )
    endforeach()
endif()