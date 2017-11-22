# OpenEmulator installation instructions

These are the building and installation instructions for OpenEmulator, a cross-platform emulator of many legacy computer
systems.

## Mac OS X

To compile OpenEmulator for Mac OS X you need Xcode and several
external libraries not available by default. The simplest way to install them is through MacPorts or Homebrew.

### MacPorts

	http://www.macports.org/

Download and install MacPorts.

To produce binaries that work with older versions of Mac OS X, enter the
following command at the terminal:

	sudo nano /opt/local/etc/macports/macports.conf

Add at the end of this file (if you installed an older Xcode,
change the path of MacOSX10.6.sdk accordingly):

	universal_target        10.6
	universal_sysroot       /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.6.sdk

Then try the following commands:

	sudo port install libzip +universal
	sudo port install libpng +universal
	sudo port install portaudio +universal
	sudo port install libsndfile +universal
	sudo port install libsamplerate +universal

If you can't compile because of this error: "'zipconf.h' file not found", try:

	sudo ln -s /opt/local/lib/libzip/include/zipconf.h /opt/local/include/zipconf.h

### Homebrew

	http://brew.sh/

#### Install dependencies

**Note:** if I missed any, follow my procedure: try to build, see which header file or library is
missing, and search for it on Google to find out what package to install.

_Interesting note: my Homebrew config was initially out-of-date, so it installed libpng 1.5.
This caused the headers to be found, but the build failed because of the `-lpng16` library param.
`brew update && brew upgrade libpng` fixed it._

	brew update
	brew install libpng libsamplerate libsndfile libzip portaudio

**Note:** If you're installing Homebrew into a non-standard location, you'll need to configure
Xcode to find its headers and libraries:
- Type command-shift-o then type xcodeproj to open the file `OpenEmulator.xcodeproj`.
- At the top, make sure the OpenEmulator project, not a target, is selected.
- Add the "include" dir to Header Search Paths (eg. `/Users/joe/homebrew/include`).
- Add the "lib" dir to Library Search Paths (eg. `/Users/joe/homebrew/lib`).

## Linux

Important: the user-interface has not yet been implemented. For now
you can only build the libemulation library.

Installation of libraries depends on the Linux distribution.

If you have a Debian-compatible distribution, try the following
commands:

	sudo apt-get install libxml2-dev
	sudo apt-get install libzip-dev
	sudo apt-get install libpng12-dev
	sudo apt-get install portaudio19-dev
	sudo apt-get install libsndfile1-dev
	sudo apt-get install libsamplerate-dev
	sudo apt-get install freeglut3-dev
	sudo apt-get install libwxgtk2.8-dev
	sudo apt-get install libxmu-dev
	sudo apt-get install libxi-dev

If you have a Fedora-compatible distribution, try the following
commands:

sudo yum install libxml2-devel
sudo yum install libzip-devel
sudo yum install libpng12-devel
sudo yum install portaudio-devel
sudo yum install libsndfile-devel
sudo yum install libsamplerate-devel
sudo yum install freeglut-devel
sudo yum install wxGTK-devel
sudo yum install libXmu-devel
sudo yum install libXi-devel

You'll also need CMake to build the emulation libraries. Enter the following
in the terminal:

	cd /path/to/build/directory

followed by:

	cmake /path/to/openemulator/source/files

followed by:

	make

## Windows

Not yet available.
