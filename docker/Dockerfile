# Single-container build: C server serves ./web and handles /ws
FROM debian:bookworm-slim

# make may not be needed
RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential make \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Copy everything needed to build + run
COPY src ./src
COPY web ./web

# Build using your src/Makefile
RUN make -C src

EXPOSE 8080

# Ensure relative paths like "./web" resolve correctly in your server
CMD ["./src/hackemu"]

# docker build -t hack-webemu-static -f docker/Dockerfile .
# docker run --rm -p 8080:8080 hack-webemu-static
# UI: http://localhost:8080/
# WS: ws://localhost:8080/ws