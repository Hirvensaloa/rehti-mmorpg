#!/bin/bash
cd ./client
conan install . --output-folder=build --build=missing -s build_type=Debug
cmake -S . -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build ./build