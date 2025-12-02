FROM gcc:15 as builder

RUN apt-get update && apt-get install -y \
    cmake ninja-build git pkg-config openssl libssl-dev

WORKDIR /build
COPY . /build

RUN cmake -Bbuild -H. -DCMAKE_BUILD_TYPE=Release
RUN cmake --build build -j$(nproc)

FROM debian:stable-slim
RUN useradd -m appuser
USER appuser
WORKDIR /app

COPY --from=builder /build/build/ws-server .

CMD ["./ws-server"]
