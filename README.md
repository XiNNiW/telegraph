# telegraph
a chaotic resonance synthesizer


# build

## install gtest and cmake
### assuming linux:
```
sudo apt-get install cmake libgtest-dev
cd /usr/src/gtest
sudo cmake CMakeLists.txt
sudo make
sudo cp *.a /usr/lib
```
### other OS
please follow GTest's instructions for installing google-test binaries
please follow cmake's instructions for installing cmake
## checkout all submodules
`git submodule update --recursive --init`
## run the build script
`./build.sh`
## run tests
`./test.sh`
