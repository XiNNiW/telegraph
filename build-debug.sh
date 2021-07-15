#!/bin/bash
if [ ! -d build ]; then
    mkdir build;
else
    rm -r build;
    mkdir build;
fi

cd build && cmake -DCMAKE_BUILD_TYPE=Debug ../;

make;