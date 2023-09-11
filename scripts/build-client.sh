#!/bin/bash
cd ./client
conan install . --output-folder=build --build=missing
cmake -S . -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -B build
cmake --build ./build