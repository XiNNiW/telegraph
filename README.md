# telegraph
a chaotic resonance synthesizer


# build

## install gtest and cmake
### linux:
```
sudo apt-get install cmake libgtest-dev libx11-dev libfreetype6-dev libxcb-util-dev libxcb-cursor-dev libxcb-xkb-dev libxkbcommon-dev libxkbcommon-x11-dev libgtkmm-3.0-dev libsqlite3-dev libasound2-dev libjack-jackd2-dev
cd /usr/src/gtest
sudo cmake CMakeLists.txt
sudo make
sudo cp *.a /usr/lib
```
### mac os
1. you will need xcode... I'm sorry...
1. you will need homebrew
1. run the following to get cmake:
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
1. later, if you get a build error about xcode version, you may need 
```sudo xcode-select -s /Applications/Xcode.app/Contents/Developer```
### other OS
please follow GTest's instructions for installing google-test binaries
please follow cmake's instructions for installing cmake
## checkout all submodules
`git submodule update --recursive --init`
## run the build script
`./build.sh`
## run tests
`./test.sh`
