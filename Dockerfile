# Use Ubuntu 22.04 as the base image
FROM ubuntu:22.04

# Update the package list and install libboost-all-dev
RUN apt-get update && \
    DEBIAN_FRONTEND=noninteractive apt-get install -y libboost-all-dev cmake clang libfreetype6-dev && \
    rm -rf /var/lib/apt/lists/*

# Set the working directory
WORKDIR /app

# Your additional Dockerfile instructions
