#!/bin/bash
cd ./client
conan install . --output-folder=build --build=missing --settings=build_type=Debug
cmake -S . -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -B build -CMAKE_BUILD_TYPE=Debug
cmake --build ./build