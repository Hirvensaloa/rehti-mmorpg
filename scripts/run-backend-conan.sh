#!/bin/bash

./scripts/generate_assets.sh

if [[ "$(expr substr $(uname -s) 1 5)" == "Linux" ]]; then
    # Linux
    EXECUTABLE_PATH="./server/build/src/server"
elif [[ "$(expr substr $(uname -s) 1 10)" == "MINGW32_NT" || "$(expr substr $(uname -s) 1 10)" == "MINGW64_NT" ]]; then
    # Windows (MINGW32_NT or MINGW64_NT)
    EXECUTABLE_PATH="./server/build/Debug/src/server"
else
    echo "Unsupported operating system"
    exit 1
fi

./scripts/build-backend-conan.sh

if [ -f "$EXECUTABLE_PATH" ]; then
    echo "Executing $EXECUTABLE_PATH"
    "$EXECUTABLE_PATH"
else
    echo "Executable not found at $EXECUTABLE_PATH"
    exit 1
fi
