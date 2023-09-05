########################################################################################################################
# Development Dockerfile
# Use this file in development. It is faster to rebuild on changes to source code.
########################################################################################################################


FROM alpine:3.17.0

RUN apk update && \
    apk add \
    build-base \
    cmake \
    boost1.80-dev=1.80.0-r3 \
    libstdc++ \
    boost1.80-program_options=1.80.0-r3 \
    libpq-dev

WORKDIR /server

COPY dependencies/libpqxx ./dependencies/libpqxx

RUN cmake --build ./dependencies/libpqxx --parallel 8 && \
    cmake --install .

COPY server/src/ ./src/
COPY server/CMakeLists.txt .

RUN cmake -DCMAKE_BUILD_TYPE=Debug && \
    cmake --build . --parallel 8

RUN addgroup -S shs && adduser -S shs -G shs
USER shs

CMD [ "./src/server"]