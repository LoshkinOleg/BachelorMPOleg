cmake_minimum_required (VERSION 3.14)
project(BachelorProject)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE AudioEngine_include ${PROJECT_SOURCE_DIR}/AudioEngineStatic/include/*.h)
file(GLOB_RECURSE AudioEngine_src ${PROJECT_SOURCE_DIR}/AudioEngineStatic/src/*.cpp)
add_library(AudioEngine STATIC ${AudioEngine_include} ${AudioEngine_src})
target_include_directories(AudioEngine PUBLIC
	${PROJECT_SOURCE_DIR}/AudioEngineStatic/include/
	${PROJECT_SOURCE_DIR}/thirdparty/3dti/include/
	${PROJECT_SOURCE_DIR}/thirdparty/cereal/
	${PROJECT_SOURCE_DIR}/thirdparty/libsofa/include/
	${PROJECT_SOURCE_DIR}/thirdparty/portaudio/include/)
target_link_libraries(AudioEngine PRIVATE
	${PROJECT_SOURCE_DIR}/thirdparty/3dti/lib/3DTI_AudioToolkit.lib
	${PROJECT_SOURCE_DIR}/thirdparty/3dti/lib/3DTI_ResourceManager.lib
	${PROJECT_SOURCE_DIR}/thirdparty/libsofa/lib/libcurl_imp_x64.lib
	${PROJECT_SOURCE_DIR}/thirdparty/libsofa/lib/libhdf5_hl_x64.lib
	${PROJECT_SOURCE_DIR}/thirdparty/libsofa/lib/libhdf5_x64.lib
	${PROJECT_SOURCE_DIR}/thirdparty/libsofa/lib/netcdf_x64.lib
	${PROJECT_SOURCE_DIR}/thirdparty/libsofa/lib/zlib_x64.lib
	${PROJECT_SOURCE_DIR}/thirdparty/libsofa/lib/libsofa_x64.lib
	${PROJECT_SOURCE_DIR}/thirdparty/portaudio/lib/portaudio_static_x64.lib)
set_target_properties(AudioEngine PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/AudioEngineStatic/lib"
    LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/AudioEngineStatic/lib"
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/AudioEngineStatic/bin"
)

add_executable(ExampleApp ${PROJECT_SOURCE_DIR}/ExampleApp/src/main.cpp)
target_link_libraries(ExampleApp PRIVATE AudioEngine)
set_target_properties(ExampleApp PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/ExampleApp/lib"
    LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/ExampleApp/lib"
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/ExampleApp/bin"
)