@echo off

::run cmake on Urho3D Source
call Urho3D/cmake_vs2015.bat ./Urho3D/Build/Win64 -DURHO3D_C++11=1 -DURHO3D_SAMPLES=0 -DURHO3D_LUA=0 ^
-DURHO3D_OPENGL=1 -DURHO3D_64BIT=1 -G "Visual Studio 14 2015 Win64"

::build Urho
start /wait "" "C:\Program Files (x86)\MSBuild\14.0\Bin\msbuild.exe" .\Urho3D\Build\Win64\Urho3D.sln /p:Configuration=Release /maxcpucount:4
start /wait "" "C:\Program Files (x86)\MSBuild\14.0\Bin\msbuild.exe" .\Urho3D\Build\Win64\Urho3D.sln /p:Configuration=Debug /maxcpucount:4

::run cmake on IOGRAM Source
call cmake -BBuild/Win64 -H. -DURHO3D_HOME="./Urho3D/Build/Win64" -DURHO3D_C++11=1 ^
-DURHO3D_OPENGL=1 -DURHO3D_64BIT=1 -G "Visual Studio 14 2015 Win64" -DCMAKE_BUILD_TYPE=RELEASE ^
-DCMAKE_INSTALL_PREFIX="./"

::build
start "" "C:\Program Files (x86)\MSBuild\14.0\Bin\msbuild.exe" .\Build\Win64\IogramSource.sln /p:Configuration=Release /maxcpucount:4
start "" "C:\Program Files (x86)\MSBuild\14.0\Bin\msbuild.exe" .\Build\Win64\IogramSource.sln /p:Configuration=Debug /maxcpucount:4    
@echo on