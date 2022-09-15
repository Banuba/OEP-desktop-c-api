add_library(camera STATIC)

target_include_directories(camera PUBLIC ${OEP_SUBMODULE_DIR})
target_include_directories(camera PUBLIC ${PROJECT_SOURCE_DIR}/oep)

if (APPLE)
    file(GLOB_RECURSE apple_srcs
        ${CMAKE_CURRENT_LIST_DIR}/*.hpp
        ${CMAKE_CURRENT_LIST_DIR}/macos/*.hpp
        ${CMAKE_CURRENT_LIST_DIR}/macos/*.mm
    )

    set_target_properties(camera PROPERTIES XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC "YES")

    target_sources(camera PUBLIC ${apple_srcs})
    target_link_libraries(camera INTERFACE
        "-framework QuartzCore"
        "-framework AVFoundation"
        "-framework CoreMedia"
    )
elseif (WIN32)
    file(GLOB_RECURSE win_srcs
        ${CMAKE_CURRENT_LIST_DIR}/*.hpp
        ${CMAKE_CURRENT_LIST_DIR}/windows/*.hpp
        ${CMAKE_CURRENT_LIST_DIR}/windows/*.cpp
    )
    target_sources(camera PUBLIC ${win_srcs})
else ()
    message(FATAL_ERROR "Unsupported platform for 'camera', CMake will exit.")
endif ()
