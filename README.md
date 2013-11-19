OpenEmulator
------------

This is OpenEmulator, a cross-platform emulator of many legacy computer
systems.

For more information, take a look at the included documentation.

### Linux

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

You'll also need CMake to build the emulation libraries. Enter the following
in the terminal:

	cd /path/to/build/directory

followed by:

	cmake /path/to/openemulator/source/files

followed by:

	make

### Mac OS X

To compile OpenEmulator for Mac OS X you need Xcode and several
external libraries not available by default. The easiest way to install
them is through MacPorts:

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

### Windows

Not yet available.