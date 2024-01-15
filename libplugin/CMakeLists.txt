add_library(libplugin SHARED libplugin.cpp)
target_compile_definitions(libplugin PRIVATE BUILD_LIBPLUGIN)
target_include_directories(libplugin PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(libplugin PROPERTIES PREFIX "")  # Avoid extra 'lib' prefix
if(WIN32)
    # link against .lib explicitly to prevent direct link against .exe
    target_link_libraries(libplugin PUBLIC ${ProjectId}.lib)
endif()