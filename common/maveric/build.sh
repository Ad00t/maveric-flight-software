rm -rf build
cmake -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -G "MinGW Makefiles" 
cmake --build build
