##
## Stage 0 — Base (non-root user)
##
FROM alpine AS base
RUN adduser -D appuser
USER appuser

##
## Stage 1 — Builder
##
FROM gcc:15 AS builder

RUN apt-get update && apt-get install -y \
    cmake git ninja-build pkg-config openssl libssl-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /build
COPY . .

RUN cmake -Bbuild -H. -DCMAKE_BUILD_TYPE=Release -G "Unix Makefiles"
RUN cmake --build build -j$(nproc)

##
## Stage 2 — Runtime (non-root)
##
FROM debian:stable-slim

RUN useradd -m appuser
USER appuser

WORKDIR /app

# Path where compose will mount TLS material
ENV CERT_PATH=/certificates
VOLUME ["/certificates"]

COPY --from=builder /build/build/drone-ws-target-tracking .

CMD ["./drone-ws-target-tracking"]
