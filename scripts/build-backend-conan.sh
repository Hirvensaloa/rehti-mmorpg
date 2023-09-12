#!/bin/bash
cd ./server
conan install . --output-folder=build --build=missing
cmake -S . -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -B build -CMAKE_BUILD_TYPE=Debug