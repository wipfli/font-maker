FROM ubuntu:22.04

RUN apt-get update && \
    DEBIAN_FRONTEND=noninteractive apt-get install -y libboost-all-dev cmake clang libfreetype6-dev libraqm-dev g++ python3-pip git && \
    rm -rf /var/lib/apt/lists/*

RUN pip3 install fonttools git+https://gitlab.com/ldo/pybidi.git

WORKDIR /root
