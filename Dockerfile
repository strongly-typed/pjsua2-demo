FROM ubuntu:16.04

RUN apt-get update && apt-get install -y \
    libasound2-dev subversion cmake build-essential pkg-config \
    && rm -rf /var/lib/apt/lists/*

RUN svn checkout http://svn.pjsip.org/repos/pjproject/tags/2.7.2 pjproject-2.7.2 \
    && cd pjproject-2.7.2 \
    && ./configure \
    && make dep \
    && make \
    && make install

WORKDIR /app
