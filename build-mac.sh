#!/bin/bash
if [ ! -d build ]; then
    mkdir build;
else
    rm -r build;
    mkdir build;
fi

cd build && cmake -GXcode -Bbuild -DCMAKE_BUILD_TYPE=Release ../;

# make;

xcodebuild build -configuration Release -project build/telegraph.xcodeproj -target telegraph