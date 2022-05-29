cmake_minimum_required (VERSION 3.14)
project(BachelorProject)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

# Ensure build dir exists.
if(NOT EXISTS "${PROJECT_SOURCE_DIR}/build/")
	message(FATAL_ERROR "Please specify an out-of-source directory 'build/' in the project's root directory.")
endif()

# Generate AudioEngine static lib.
file(GLOB_RECURSE AudioEngine_include ${PROJECT_SOURCE_DIR}/AudioEngineStatic/include/*.h)
file(GLOB_RECURSE AudioEngine_src ${PROJECT_SOURCE_DIR}/AudioEngineStatic/src/*.cpp)
add_library(AudioEngine STATIC ${AudioEngine_include} ${AudioEngine_src})
target_include_directories(AudioEngine PUBLIC
	${PROJECT_SOURCE_DIR}/AudioEngineStatic/include/
	${PROJECT_SOURCE_DIR}/thirdparty/3dti/include/
	${PROJECT_SOURCE_DIR}/thirdparty/cereal/
	${PROJECT_SOURCE_DIR}/thirdparty/libsofa/include/
	${PROJECT_SOURCE_DIR}/thirdparty/portaudio/include/
	${PROJECT_SOURCE_DIR}/thirdparty/easy_profiler/include/
	${PROJECT_SOURCE_DIR}/thirdparty/openvr/include/
	${PROJECT_SOURCE_DIR}/thirdparty/ovraudio/include/
	${PROJECT_SOURCE_DIR}/thirdparty/steamaudio/include/)
target_link_libraries(AudioEngine PRIVATE
	${PROJECT_SOURCE_DIR}/thirdparty/3dti/lib/3DTI_AudioToolkit.lib
	${PROJECT_SOURCE_DIR}/thirdparty/3dti/lib/3DTI_ResourceManager.lib
	${PROJECT_SOURCE_DIR}/thirdparty/libsofa/lib/libcurl_imp_x64.lib
	${PROJECT_SOURCE_DIR}/thirdparty/libsofa/lib/libhdf5_hl_x64.lib
	${PROJECT_SOURCE_DIR}/thirdparty/libsofa/lib/libhdf5_x64.lib
	${PROJECT_SOURCE_DIR}/thirdparty/libsofa/lib/netcdf_x64.lib
	${PROJECT_SOURCE_DIR}/thirdparty/libsofa/lib/zlib_x64.lib
	${PROJECT_SOURCE_DIR}/thirdparty/libsofa/lib/libsofa_x64.lib
	${PROJECT_SOURCE_DIR}/thirdparty/portaudio/lib/portaudio_static_x64.lib
	${PROJECT_SOURCE_DIR}/thirdparty/easy_profiler/lib/easy_profiler.lib
	${PROJECT_SOURCE_DIR}/thirdparty/openvr/lib/openvr_api.lib
	${PROJECT_SOURCE_DIR}/thirdparty/ovraudio/lib/ovraudio64.lib
	${PROJECT_SOURCE_DIR}/thirdparty/steamaudio/lib/phonon.lib)
set_target_properties(AudioEngine PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/AudioEngineStatic/lib"
    LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/AudioEngineStatic/lib"
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/AudioEngineStatic/bin"
)

# Generate profiling app executable.
add_executable(ProfilingApp ${PROJECT_SOURCE_DIR}/ProfilingApp/src/main.cpp)
target_link_libraries(ProfilingApp PRIVATE AudioEngine)
set_target_properties(ProfilingApp PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/ProfilingApp/lib"
    LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/ProfilingApp/lib"
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/ProfilingApp/bin"
)

# Create folder for holding easy_profiler's profiling data.
file(MAKE_DIRECTORY ${PROJECT_SOURCE_DIR}/build/profilingData)

# Add options for the user to configure
set(USE_EASY_PROFILER ON CACHE BOOL "Enable profiling of spatialization time. Generates .prof files under build/profilingData/")
if (USE_EASY_PROFILER)
	add_compile_definitions(USE_EASY_PROFILER)
endif()