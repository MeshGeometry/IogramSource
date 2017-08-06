./Urho3D/cmake_generic.sh ./Urho3D/Build/gcc -DURHO3D_OPENGL=1 -DURHO3D_64BIT=1 -DURHO3D_LUA=0 -DURHO3D_SAMPLES=0 -DURHO3D_C++11=1 &&
cd ./Urho3D/Build/gcc && make -j4 &&
cd ../../.. &&
cmake -BBuild/gcc -H. -DURHO3D_HOME="./Urho3D/Build/gcc" -DURHO3D_OPENGL=1 -DURHO3D_64BIT=1 -DURHO3D_LUA=0 -DURHO3D_C++11=1 &&
cd Build/gcc && make -j4
