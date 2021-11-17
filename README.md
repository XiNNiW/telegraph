# telegraph
a chaotic resonance synthesizer

telegraph makes ringing oscillations that hiss, warble, and stutter. It can make beautiful distorted tones that sound a bit like badly treated cassette tape. It is also capable of wild, out of control noise. I hope you enjoy it!

# build

## linux:

1. install gtest and cmake

```
sudo apt-get install cmake libgtest-dev libx11-dev libfreetype6-dev libxcb-util-dev libxcb-cursor-dev libxcb-xkb-dev libxkbcommon-dev libxkbcommon-x11-dev libgtkmm-3.0-dev libsqlite3-dev libasound2-dev libjack-jackd2-dev
cd /usr/src/gtest
sudo cmake CMakeLists.txt
sudo make
sudo cp *.a /usr/lib
```
2. checkout all submodules
`git submodule update --recursive --init`
3. run the build script
`./build.sh`
The `.so` file will be in the build folder and an alias to it should be placed in `~/.vst3`.
If the alias is missing you can manually copy it over.
4. run tests
`./test.sh`
## mac os
1. you will need [xcode](https://developer.apple.com/support/xcode/)... I'm sorry...
1. wait 20m while the 20 odd gigabytes of unrelated apple stuff downloads
1. check to see if you have `git` by running `git --version`. If its missing follow instructions for downloading dev tools.
1. you will need [homebrew](https://brew.sh/)
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
1. checkout all submodules
`git submodule update --recursive --init`
1. run the build script
`./build-mac.sh`
1. The last command should have created an XCode project for you to open. Open it and begin configuring build settings according to your needs. (ex. set the lowest targeted operating system)
1. select and run the `BUILD ALL` target.
1. Apple has helpfully hidden the resulting `.vst3` files deep in your  system Library/Development/XCode folder. [Happy hunting](https://discussions.apple.com/thread/3019694).
1. run tests
`./test.sh`
Alternatively, you may run the test executable from the XCode project.

### other OS (note: Untested!)
1. please follow GTest's instructions for installing google-test binaries
1. please follow cmake's instructions for installing cmake
1. checkout all submodules
`git submodule update --recursive --init`
1. run the build script
`./build.sh`
1. run tests
`./test.sh`
