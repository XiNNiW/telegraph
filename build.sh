#!/bin/bash
if [ ! -d build ]; then
    mkdir build;
else
    rm -r build;
    mkdir build;
fi

cmake -DCMAKE_BUILD_TYPE=Release ../;

make;