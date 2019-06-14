spkrd
=====

spkrd (Speaker Design) is an update to the GSpeakers and GSpeakers-Redux application written originally by Daniel Sundberg and originally hosted at [SourceForge](https://sourceforge.net/projects/gspeakers/).

The purpose of this repository is to bring a high quality loudspeaker design software to Linux.  Extensive work is underway to cleanup the deprecated features in GTK2 and to use GTK3.  The old and dusty code has been cleaned up to align with modern C++ and we require C++14 as the working standard.  The build system has been ported to CMake for ease of compilation.

Requests to add and improve features, bug reports and collaboration are strongly encouraged.  I welcome your forks and pull requests!

## Compiling spkrd

The cross-platform nature of `Cmake` allows mostly the same compilation steps across platforms.  Generally you want to create a separate build directory and run

```
cmake ..
```

and once configured run the appropriate compilation program.

### Linux

```
mkdir build && cd build
cmake ..
make all
```

### Windows

The best thing you can do if you want to run spkrd on Windows is
to download the binary package.

* Download and install the gtk+ runtime environment
  (http://gladewin32.sourceforge.net/)

* Download and unzip the GSpeakers zip file, double click on gspeakers.exe
  to start GSpeakers

If you want to build GSpeakers things get a little bit more complicated. First
you need a compiler and some kind of GNU system. I use Mingw and Msys and some
additional tools from the gnuwin32 project. Here are some step by step
instructions that you can use. However, be prepared to do some tweaking on
your own, setting up the build envorinment might be quite tricky. Also, I
have not got the cygwin environment to work so I just use msys instead.

If you use msys you may have to get some additional packages from
the gnuwin project (http://gnuwin32.sourceforge.net/). Also you have to
add the pkg-config definition file for libxml2 since that file has been
missing in the win32 binary distribution. This file is attached at the bottom
of this file.

### Mac OS X
by Emil Hessman

Make sure you have the following software installed before an attempt
to install GSpeakers:

- The Developer tools (X11, xTools etc.)
- gtkmm-2
- libxml2

The Developer Tools ( http://www.apple.com/macosx/developertools/ ) are
included on the software install [CDs/DVDs] provided by Apple, they can
also be found in Apple's  download section at their website
(http://www.apple.com/downloads/macosx/) - consult Apple's website
(http://www.apple.com/) for further information.

gtkmm and libxml are, for an example, available in the DarwinPorts
collection - consult its documentation at
http://darwinports.opendarwin.org/ for installation instructions.

Download the latest source code for GSpeakers.

Uncompress the source code by performing the following command in the
Terminal:

tar zxvf gspeakers-0.xx.tar.gz

.. where xx is the version of the file you downloaded.

Change directory to the directory containing GSpeakers' source code:

cd gspeakers-0.xx/

.. where xx is the version of the file you previously uncompressed.

Configure GSpeakers for your system:

./configure

Compile GSpeakers:

make

Install GSpeakers:

```sh
sudo make install
```

How to run GSpeakers;

Fire up the X Window System - X11 (being found in /Applications/Utilities/),
open up a Terminal from X11's menu and run '/usr/local/bin/gspeakers'
(/usr/local/bin is where GSpeakers is installed by default).

## Release Criteria

* Check that the xml files refers to frf-files in installation path (use vifa1.xml.in in /xml)


* Compile from source and check that everything works
(Check list goes here)


* Prepare release
make distclean
cp -R gspeakers2 gspeakers-{VERSION}
cd gspeakers-{VERSION}
rm -rf doc
rm -rf autom4te.cache
rm -rf `find -name CVS`
rm -rf `find -name *~`

* Make tar balls:

tar xjf gspeakers-{VERSION}.tar.bz2 gspeakers-{VERSION}
tar xzf gspeakers-{VERSION}.tar.bz2 gspeakers-{VERSION}


* Make rpms:

cp speakers-{VERSION}.tar.bz2 /home/sumpan/code/rpmbuild/SOURCES

cd gspeakers2
./configure
cd gspeakers2
rpmbuild -ba gspeakers.spec


* Windows binary

New checkout
cd gspeakers2
mkdir win32-bin
cp pixmaps/* win32-bin
cp xml/* win32-bin
./configure --enable-win32

Copy gtkmm runtime files
Copy gnucap executable
Test
Check xml file frf paths
Zip the file


* Sourceforge

Upload source tar balls, rpms and windows binary
Release on sourceforge

* Update web page

New features

* Freshmeat

* Gnomefiles

* http://footnews.org
