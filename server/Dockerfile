########################################################################################################################
# Development Dockerfile
# Use this file in development. It is faster to rebuild on changes to source code.
########################################################################################################################


FROM alpine:3.17.0

RUN apk update && \
    apk add \
    build-base \
    cmake \
    libstdc++ \
    libpq-dev \
    pip

RUN pip install conan

WORKDIR /server

COPY server/src/ ./src/
COPY server/CMakeLists.txt .

RUN cmake -DCMAKE_BUILD_TYPE=Debug && \
    cmake --build . --parallel 8

RUN addgroup -S shs && adduser -S shs -G shs
USER shs

CMD [ "./src/server"]