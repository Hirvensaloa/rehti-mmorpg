########################################################################################################################
# Development Dockerfile
# Use this file in development. It is faster to rebuild on changes to source code.
########################################################################################################################


FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    wget \
    libssl-dev \
    python3-pip \
    && rm -rf /var/lib/apt/lists/*

# Install cmake from the binary as the apt-get version is too old.
RUN wget https://github.com/Kitware/CMake/releases/download/v3.26.4/cmake-3.26.4-linux-x86_64.sh && \
    chmod +x cmake-3.26.4-linux-x86_64.sh && \
    ./cmake-3.26.4-linux-x86_64.sh --skip-license --prefix=/usr/local

RUN pip3 install conan

RUN conan profile detect --force

WORKDIR /server 

COPY server/conanfile.txt .

RUN conan install . --output-folder=build --build=missing -s build_type=Debug 

COPY server/CMakeLists.txt .
COPY server/src/ ./src/

RUN cmake -S . -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -B build -DCMAKE_BUILD_TYPE=Debug && \
    cmake --build ./build

CMD [ "./build/src/server"]