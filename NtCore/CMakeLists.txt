cmake_minimum_required(VERSION 3.15)
project(NtCore LANGUAGES CXX VERSION 0.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(BUILD_NT_TEST "" ON)
option(BUILD_NT_STATIC "" OFF)

add_compile_options(/wd4251)

file(GLOB_RECURSE SRCS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE HEADERS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")

include_directories(
    "include"
)

add_library(
    ${PROJECT_NAME}
    SHARED
    ${SRCS}
    ${HEADERS}
)

if(BUILD_NT_STATIC)
    add_library(
        ${PROJECT_NAME}_static
        STATIC
        ${SRCS}
        ${HEADERS}
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