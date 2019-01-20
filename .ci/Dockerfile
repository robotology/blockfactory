ARG from=ubuntu:bionic
FROM ${from}
MAINTAINER Diego Ferigo <diego.ferigo@iit.it>

RUN apt-get update &&\
    apt-get install -y \
        lsb-release \
        wget \
        gnupg \
        git \
        build-essential \
        cmake \
        ninja-build \
        clang \
        ccache \
        valgrind \
        &&\
    rm -rf /var/lib/apt/lists/*

# Enable CCACHE by default
ENV PATH=/usr/lib/ccache:$PATH
RUN mkdir $HOME/.ccache &&\
    echo "max_size = 2.0G" > $HOME/.ccache/ccache.conf
