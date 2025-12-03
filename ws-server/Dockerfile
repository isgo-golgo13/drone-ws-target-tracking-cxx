FROM gcc:15 AS builder

RUN apt-get update && apt-get install -y \
        cmake git ninja-build pkg-config openssl libssl-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /build
COPY . .
RUN cmake -Bbuild -H. -DCMAKE_BUILD_TYPE=Release
RUN cmake --build build -j$(nproc)

FROM debian:stable-slim
RUN useradd -m appuser
USER appuser
WORKDIR /app

ENV CERT_PATH=/certificates
VOLUME ["/certificates"]

COPY --from=builder /build/build/ws-server .

CMD ["./ws-server"]
