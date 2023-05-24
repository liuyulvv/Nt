cmake_minimum_required(VERSION 3.15)
project(NtCore LANGUAGES CXX VERSION 0.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE SRCS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE HEADERS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")

include_directories(
    "include"
)

add_library(
    ${PROJECT_NAME}
    SHARED
    ${SRCS}
)

add_library(
    ${PROJECT_NAME}s
    STATIC
    ${SRCS}
)

install(
    TARGETS
    ${PROJECT_NAME} ${PROJECT_NAME}s
    RUNTIME DESTINATION dll
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(FILES ${HEADERS} DESTINATION include)

file(GLOB TESTS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/test/*.cpp")

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