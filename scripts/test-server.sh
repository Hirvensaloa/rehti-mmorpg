#!/bin/bash

if [[ "$(expr substr $(uname -s) 1 5)" == "Linux" ]]; then
    # Linux
    EXECUTABLE_PATH="./build/mmorpg-server-test"
elif [[ "$(expr substr $(uname -s) 1 10)" == "MINGW32_NT" || "$(expr substr $(uname -s) 1 10)" == "MINGW64_NT" ]]; then
    # Windows (MINGW32_NT or MINGW64_NT)
    EXECUTABLE_PATH="./build/Debug/mmorpg-server-test"
else
    echo "Unsupported operating system"
    exit 1
fi

cd server/test
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build ./build

if [ -f "$EXECUTABLE_PATH" ]; then
    echo "Executing $EXECUTABLE_PATH"
    "$EXECUTABLE_PATH"
else
    echo "Executable not found at $EXECUTABLE_PATH"
    exit 1
fi
