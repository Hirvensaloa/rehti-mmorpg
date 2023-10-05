#!/bin/bash

if [[ "$(expr substr $(uname -s) 1 5)" == "Linux" ]]; then
    # Linux
    EXECUTABLE_PATH="./client/build/Client"
elif [[ "$(expr substr $(uname -s) 1 10)" == "MINGW32_NT" || "$(expr substr $(uname -s) 1 10)" == "MINGW64_NT" ]]; then
    # Windows (MINGW32_NT or MINGW64_NT)
    EXECUTABLE_PATH="./client/build/Debug/Client"
else
    echo "Unsupported operating system"
    exit 1
fi

./scripts/build-client.sh 

if [ -f "$EXECUTABLE_PATH" ]; then
    echo "Executing $EXECUTABLE_PATH"
    "$EXECUTABLE_PATH" 127.0.0.1
else
    echo "Executable not found at $EXECUTABLE_PATH"
    exit 1
fi
