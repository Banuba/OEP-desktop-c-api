set(include_dirs
    ${CMAKE_CURRENT_SOURCE_DIR}/include/
)

file(GLOB_RECURSE srcs
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp
)

add_library(camera STATIC ${srcs})

target_include_directories(camera PUBLIC
    ${include_dirs}
)

target_include_directories(camera PUBLIC ${PROJECT_SOURCE_DIR})

if (APPLE)
    set(apple_includes
        ${CMAKE_CURRENT_SOURCE_DIR}/platform/macos/
    )

    file(GLOB_RECURSE apple_srcs
        ${CMAKE_CURRENT_SOURCE_DIR}/platform/macos/*.mm
    )

    target_sources(camera PUBLIC ${apple_srcs})
    target_include_directories(camera PUBLIC ${apple_includes})
endif()

target_link_libraries(camera
    bnb_effect_player
)
