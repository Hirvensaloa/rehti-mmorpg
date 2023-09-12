#!/bin/bash
cd ./server
conan install . --output-folder=build --build=missing -s build_type=Debug -s compiler.cppstd=17
cmake -S . -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build ./build