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
	${PROJECT_SOURCE_DIR}/thirdparty/steamaudio/include/
	${PROJECT_SOURCE_DIR}/thirdparty/fmod/include/
	${PROJECT_SOURCE_DIR}/thirdparty/dr_wav/include/
	${PROJECT_SOURCE_DIR}/thirdparty/sdl/include/
	)
target_link_libraries(AudioEngine PRIVATE
	optimized ${PROJECT_SOURCE_DIR}/thirdparty/3dti/lib/3DTI_AudioToolkit_release.lib
	optimized ${PROJECT_SOURCE_DIR}/thirdparty/3dti/lib/3DTI_ResourceManager_release.lib
	debug ${PROJECT_SOURCE_DIR}/thirdparty/3dti/lib/3DTI_AudioToolkit_debug.lib
	debug ${PROJECT_SOURCE_DIR}/thirdparty/3dti/lib/3DTI_ResourceManager_debug.lib
	general ${PROJECT_SOURCE_DIR}/thirdparty/libsofa/lib/libcurl_imp_x64.lib
	general ${PROJECT_SOURCE_DIR}/thirdparty/libsofa/lib/libhdf5_hl_x64.lib
	general ${PROJECT_SOURCE_DIR}/thirdparty/libsofa/lib/libhdf5_x64.lib
	general ${PROJECT_SOURCE_DIR}/thirdparty/libsofa/lib/netcdf_x64.lib
	general ${PROJECT_SOURCE_DIR}/thirdparty/libsofa/lib/zlib_x64.lib
	optimized ${PROJECT_SOURCE_DIR}/thirdparty/libsofa/lib/libsofa_release_x64.lib
	debug ${PROJECT_SOURCE_DIR}/thirdparty/libsofa/lib/libsofa_debug_x64.lib
	general ${PROJECT_SOURCE_DIR}/thirdparty/portaudio/lib/portaudio_static_x64.lib
	general ${PROJECT_SOURCE_DIR}/thirdparty/easy_profiler/lib/easy_profiler.lib
	general ${PROJECT_SOURCE_DIR}/thirdparty/openvr/lib/openvr_api.lib
	general ${PROJECT_SOURCE_DIR}/thirdparty/steamaudio/lib/phonon.lib
	general ${PROJECT_SOURCE_DIR}/thirdparty/fmod/lib/fmod_vc.lib
	general ${PROJECT_SOURCE_DIR}/thirdparty/fmod/lib/fmodL_vc.lib
	general ${PROJECT_SOURCE_DIR}/thirdparty/sdl/lib/SDL2.lib
	general ${PROJECT_SOURCE_DIR}/thirdparty/sdl/lib/SDL2main.lib
	general ${PROJECT_SOURCE_DIR}/thirdparty/sdl/lib/SDL2test.lib
	)
set_target_properties(AudioEngine PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/build/AudioEngineStatic/lib"
    LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/build/AudioEngineStatic/lib"
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/build/AudioEngineStatic/bin"
)

# Generate profiling app executable.
add_executable(ProfilingApp ${PROJECT_SOURCE_DIR}/ProfilingApp/src/main.cpp)
target_link_libraries(ProfilingApp PRIVATE AudioEngine)
set_target_properties(ProfilingApp PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/build/ProfilingApp/lib"
    LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/build/ProfilingApp/lib"
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/build/ProfilingApp/bin"
)

# Create folder for holding easy_profiler's profiling data.
file(MAKE_DIRECTORY ${PROJECT_SOURCE_DIR}/build/profilingData)

# Add options for the user to configure
set(USE_EASY_PROFILER ON CACHE BOOL "Enable profiling of spatialization time. Generates .prof files under build/profilingData/")
set(USE_DUMMY_INPUTS ON CACHE BOOL "Test out the experiment without using the VR headset.")
if (USE_EASY_PROFILER)
	add_compile_definitions(USE_EASY_PROFILER)
endif()
if (USE_DUMMY_INPUTS)
	add_compile_definitions(USE_DUMMY_INPUTS)
endif()