#!/usr/bin/env bash

rm -rf build
mkdir build
cd build
cmake --clean-first ..
make
./pjsua2-demo
