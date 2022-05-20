# BachelorMP

Generate a Visual Studio using the CMakeLists.txt provided.

Once everything is built, don't forget to move the *.dll's from various "bin/" folders from third party dependancies located in "thirdparty/*/bin/" to "ExampleApp/bin/" for the ExampleApp to work.

The ExampleApp needs to be built in Release x64 mode, else the build will fail due to modes mismatch.