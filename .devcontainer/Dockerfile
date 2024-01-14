FROM ubuntu:jammy

RUN apt-get update \
    && DEBIAN_FRONTEND=noninteractive apt-get install -qq --no-install-recommends \
        build-essential \
        clang-format \
        cmake \
        gdb \
        git \
        gstreamer1.0 \
        jackd2 \
        libfmt-dev \
        libjack-jackd2-dev \
        libfftw3-dev \
        libncurses-dev \
        libsndfile-dev \
        locales \
    && locale-gen en_US.UTF-8

ENV LANG en_US.UTF-8
ENV LANGUAGE en_US.UTF-8
ENV LC_ALL en_US.UTF-8
