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

# Install required build toolchain + SSL
RUN apt-get update && apt-get install -y \
        cmake git ninja-build pkg-config openssl libssl-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /build

# Copy project
COPY . .

# Configure + build
RUN cmake -Bbuild -H. -DCMAKE_BUILD_TYPE=Release -G "Unix Makefiles"
RUN cmake --build build -j$(nproc)


##
## Stage 2 — Runtime (rootless)
##
FROM debian:stable-slim

# Create non-root runtime user
RUN useradd -m appuser
USER appuser

WORKDIR /app

# Where certificates are mounted via docker-compose
ENV CERT_PATH=/certificates

# Mount point for host certs
VOLUME ["/certificates"]

# Copy final executable from builder stage
COPY --from=builder /build/build/drone-ws-target-tracking .

# Run the application
CMD ["./drone-ws-target-tracking"]
