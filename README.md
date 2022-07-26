# BachelorMP

Generate a Visual Studio using the CMakeLists.txt provided and make an out-of-source build of the project in a new folder named "build". This will ensure that moveDlls.bat can copy the .dll's necessary for the ExperimentApp to run into the right location.
Use moveDlls.bat to automatically move all the necessary dll's from thirdparty subdirectories into "ExperimentApp/bin/Release/" .
If launching the ProfilingApp still throws a missing dll error, all dll's necessary for it can be found under "thirdparty/.../bin/". And dll's contained within subdirectories of "bin/" folders are not required and only used for other applications, such as the easy_profiler's "profiler_gui.exe".
