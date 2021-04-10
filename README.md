# telegraph
a chaotic resonance synthesizer


# build

## install gtest and cmake
### linux:
```
sudo apt-get install cmake libgtest-dev
cd /usr/src/gtest
sudo cmake CMakeLists.txt
sudo make
sudo cp *.a /usr/lib
```
### mac os
1. you will need xcode... I'm sorry...
1. you will need homebrew
3. run the following to get cmake:
```
brew install cmake
```
1. run the following to get gtest:
```
git clone https://github.com/google/googletest
cd googletest
mkdir build
cd build
cmake ..
make
make install
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
