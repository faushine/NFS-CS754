FROM ubuntu:latest
RUN apt-get update \
    && apt-get install -y \
                        python3 python3-pip python3-setuptools python3-wheel \
                        ninja-build \
                        vim \
                        udev \
                        pkg-config \
                        wget \
                        cmake \
                        autoconf automake libtool curl make g++ unzip \
                        git \
    && cd /usr/local/bin \
    && ln -s /usr/bin/python3 python \
    && pip3 install --upgrade pip
RUN pip3 install meson
RUN pip3 install pytest
WORKDIR /fuse
RUN wget https://github.com/libfuse/libfuse/archive/fuse-3.7.0.tar.gz \
    && tar xvzf fuse-3.7.0.tar.gz \
    && mkdir libfuse-fuse-3.7.0/build
WORKDIR /fuse/libfuse-fuse-3.7.0/build
RUN meson .. \
    && ninja \
    && ninja install
# install protobuf
WORKDIR /protobuf
RUN wget https://github.com/protocolbuffers/protobuf/releases/download/v3.10.0/protobuf-cpp-3.10.0.tar.gz \
    && tar xvzf protobuf-cpp-3.10.0.tar.gz
WORKDIR /protobuf/protobuf-3.10.0
RUN ./configure \
    && make -j4 \
    && make check -j4 \
    && make install -j4 \
    && ldconfig # refresh shared library cache.
# install gRPC
WORKDIR /
RUN git clone -b $(curl -L https://grpc.io/release) https://github.com/grpc/grpc
WORKDIR /grpc
RUN git submodule update --init \
    && make && make install
# install json
WORKDIR /
RUN git clone https://github.com/nlohmann/json.git
WORKDIR /json
RUN cmake . \
    && make
WORKDIR /fuse-nfs
