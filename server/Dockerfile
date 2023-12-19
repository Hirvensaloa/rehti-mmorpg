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
RUN wget https://github.com/Kitware/CMake/releases/download/v3.27.0/cmake-3.27.0-linux-x86_64.sh && \
    chmod +x cmake-3.27.0-linux-x86_64.sh && \
    ./cmake-3.27.0-linux-x86_64.sh --skip-license --prefix=/usr/local

RUN pip3 install conan

RUN conan profile detect --force

WORKDIR /app

COPY server/conanfile.txt ./server/conanfile.txt

RUN cd server && conan install . --output-folder=build --build=missing -s build_type=Debug -s compiler.cppstd=20

COPY third_party/rapidjson/include/ ./third_party/rapidjson/include/
COPY third_party/bcrypt ./third_party/bcrypt/


COPY rehtiLib/utils/ ./rehtiLib/utils
# Generate map assets
COPY rehtiLib/assets/ ./rehtiLib/assets
COPY scripts/generate_assets.sh ./scripts/generate_assets.sh
COPY assets/ ./assets
RUN ./scripts/generate_assets.sh

COPY rehtiLib/network/ ./rehtiLib/network
COPY server/CMakeLists.txt ./server
COPY server/src/ ./server/src/

RUN cd server && cmake -S . -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -B build -DCMAKE_BUILD_TYPE=Debug && \
    cmake --build ./build

CMD [ "./server/build/src/server"]