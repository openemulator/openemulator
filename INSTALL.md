# OpenEmulator installation instructions

These are developer instructions for building OpenEmulator. Users should download a pre-packaged [release](https://github.com/openemulator/openemulator/releases) that is ready to use.

## Mac OS X
We aim to be portable, but currently the only offically supported and tested platform is macOS.

To compile OpenEmulator for Mac OS X you need Xcode and several external libraries not available by default. The simplest way to install them is Homebrew. 

### Dependencies

Install the dependencies for libemulation as described [here](https://github.com/openemulator/libemulation/blob/master/INSTALL.md). When the dependencies are installed, build libemulation:

	cmake -Hmodules/libemulation -Bmodules/libemulation/build -DCMAKE_BUILD_TYPE=Release
	cmake --build modules/libemulation/build --config Release

### Build OpenEmulator

Open OpenEmulator in XCode to build, or run the command

	xcodebuild