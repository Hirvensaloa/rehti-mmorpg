#!/bin/bash
cd ./client
mkdir build
cmake -S . -B build
make -C build