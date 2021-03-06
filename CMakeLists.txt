cmake_minimum_required (VERSION 3.14)
project(BachelorProject)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

# Ensure build dir exists.
if(NOT EXISTS "${PROJECT_SOURCE_DIR}/build/")
	message(FATAL_ERROR "Please specify an out-of-source directory 'build/' in the project's root directory.")
endif()

# Generate AudioEngine static lib.
file(GLOB_RECURSE AudioEngineStatic_include ${PROJECT_SOURCE_DIR}/AudioEngineStatic/include/*.h)
file(GLOB_RECURSE AudioEngineStatic_src ${PROJECT_SOURCE_DIR}/AudioEngineStatic/src/*.cpp)
add_library(AudioEngineStatic STATIC ${AudioEngineStatic_include} ${AudioEngineStatic_src})
target_include_directories(AudioEngineStatic PUBLIC
	${PROJECT_SOURCE_DIR}/AudioEngineStatic/include/
	${PROJECT_SOURCE_DIR}/thirdparty/3dti/include/
	${PROJECT_SOURCE_DIR}/thirdparty/cereal/
	${PROJECT_SOURCE_DIR}/thirdparty/libsofa/include/
	${PROJECT_SOURCE_DIR}/thirdparty/portaudio/include/
	${PROJECT_SOURCE_DIR}/thirdparty/easy_profiler/include/
	${PROJECT_SOURCE_DIR}/thirdparty/openvr/include/
	${PROJECT_SOURCE_DIR}/thirdparty/dr_wav/include/
	${PROJECT_SOURCE_DIR}/thirdparty/sdl/include/
	${PROJECT_SOURCE_DIR}/thirdparty/fmod/include/
	)
target_link_libraries(AudioEngineStatic PRIVATE
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
	general ${PROJECT_SOURCE_DIR}/thirdparty/sdl/lib/SDL2.lib
	general ${PROJECT_SOURCE_DIR}/thirdparty/sdl/lib/SDL2main.lib
	general ${PROJECT_SOURCE_DIR}/thirdparty/sdl/lib/SDL2test.lib
	general ${PROJECT_SOURCE_DIR}/thirdparty/fmod/lib/fmod_vc.lib
	general ${PROJECT_SOURCE_DIR}/thirdparty/fmod/lib/fmodL_vc.lib
	)
set_target_properties(AudioEngineStatic PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/build/AudioEngineStatic/lib"
    LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/build/AudioEngineStatic/lib"
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/build/AudioEngineStatic/bin"
)

# Generate experiment app executable.
file(GLOB_RECURSE ExperimentApp_include ${PROJECT_SOURCE_DIR}/ExperimentApp/include/*.h)
file(GLOB_RECURSE ExperimentApp_src ${PROJECT_SOURCE_DIR}/ExperimentApp/src/*.cpp)
add_executable(ExperimentApp ${ExperimentApp_include} ${ExperimentApp_src})
target_include_directories(ExperimentApp PUBLIC
	${PROJECT_SOURCE_DIR}/ExperimentApp/include/
	${PROJECT_SOURCE_DIR}/AudioEngineStatic/include/
	${PROJECT_SOURCE_DIR}/thirdparty/spdlog/include/
	)
target_link_libraries(ExperimentApp PRIVATE
	general AudioEngineStatic
	optimized ${PROJECT_SOURCE_DIR}/thirdparty/spdlog/lib/spdlog.lib
	debug ${PROJECT_SOURCE_DIR}/thirdparty/spdlog/lib/spdlogd.lib
	)
set_target_properties(ExperimentApp PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/build/ExperimentApp/lib"
    LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/build/ExperimentApp/lib"
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/build/ExperimentApp/bin"
)

# Create folder for holding easy_profiler's profiling data.
file(MAKE_DIRECTORY ${PROJECT_SOURCE_DIR}/build/profilingData)
# Create folder for holding experiment's results.
file(MAKE_DIRECTORY ${PROJECT_SOURCE_DIR}/build/experimentData)

# Add options for the user to configure
set(SEED 0 CACHE INT "Seed used for random positions. A seed of 0 results in a random seed.")
set(SIMULATE_REVERB OFF CACHE BOOL "Whether the spatializers should simulate reverb or not.")
set(USE_EASY_PROFILER OFF CACHE BOOL "Enable profiling of spatialization time. Generates .prof files under build/profilingData/")
set(RUN_WITHOUT_VR OFF CACHE BOOL "Whether to use the headset VR or not, disable to run the program without a VR headset connected.")
if(SIMULATE_REVERB)
	add_compile_definitions(BUILD_WITH_REVERB)
endif()
if (USE_EASY_PROFILER)
	add_compile_definitions(BUILD_WITH_EASY_PROFILER)
endif()
if (NOT RUN_WITHOUT_VR)
	add_compile_definitions(BUILD_WITH_OPENVR)
endif()
add_compile_definitions(BSEXP_SEED=${SEED})