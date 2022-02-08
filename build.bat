if not exist "build/" mkdir "build/"
cd build
:: https://cmake.org/cmake/help/git-stage/generator/Visual%20Studio%2016%202019.html
cmake .. -G "Visual Studio 17 2022" -A x64
PAUSE