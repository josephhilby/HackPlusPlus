# Single-container build: C server serves ./web and handles /ws
FROM debian:bookworm-slim

# make may not be needed
RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential make \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Copy everything needed to build + run
COPY core ./core
COPY web ./web

# Build using your core/Makefile
RUN make -C core

EXPOSE 8080

# Ensure relative paths like "./web" resolve correctly
CMD ["./core/hackemu"]

# Build image
# docker build -t hack-webemu-static -f Dockerfile .

# Run container
# docker run --name hack-webemu --rm -p 8080:8080 hack-webemu-static

# Stop container
# docker stop hack-webemu

# Restart container
# docker start hack-webemu

# View logs
# docker logs -f hack-webemu

# UI: http://localhost:8080/
# WS: ws://localhost:8080/ws