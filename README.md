
## Build prerequisites

IogramSource's primary dependency is [Urho3D](https://urho3d.github.io). To
build IogramSource you will have to build Urho3D first on your machine.

Here are instructions to
[build Urho3D](https://urho3d.github.io/documentation/1.6/_building.html) on
your platform. To allow IogramSource to link to Urho3D, build Urho3D with the
following build options

* -DURHO3D_C++11=1
* -DURHO3D_OPENGL=1
* -DURHO3D_64BIT=1

If you have any problems building Urho3D we'll be happy to try to help you out
here as well.

## IogramSource build instructions

### Visual Studio 2015 on Windows 10

Find the file "cmake_vs2015.bat.txt" at the top level of IogramSource and modify

* -DURHO3D_HOME="path/to/Urho3D/build"

to the location in which you have compiled Urho3D on your machine. Change this
file's name to "cmake_vs2015.bat" and run the script. The folder Build/Win64/
will be created in IogramSource, with a Visual Studio 2015 solution. Use this to
compile IogramSource with Visual Studio 2015.

### Linux

Find the file "cmake_generic.sh.txt" at the top level of IogramSource and modify

* -DURHO3D_HOME="path/to/Urho3D/build"

to the location in which you have compiled Urho3D on your machine. Change this
file's name to "cmake_generic.sh" and do
```
chmod +x cmake_generic.sh
```
on the command line to make it executable. Then do
```
./cmake_generic.sh
cd build
make
```
The IogramSource library will be compiled in the newly created build/ folder.