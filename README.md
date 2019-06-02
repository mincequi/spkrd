GSpeakers Redux
===============

GSpeakers Redux is an update to the GSpeakers application written by Daniel Sundberg and originally hosted at [SourceForge](https://sourceforge.net/projects/gspeakers/).

The purpose of this update is to bring GSpeakers up to date so that it will compile without problems on modern Linuxes, to add/improve features, to encourage collaboration, and perhaps to eventually rewrite the application.  I welcome your forks and pull requests!


Original README
===============

GSpeakers is a loudspeaker enclosure and crossovernetwork designer for the gtk+/gnome desktop.

However, you can build GSpeakers on both Microsoft Windows and MacOS X. See README.win32 and README.macosx for further details.

## GSpeakers on Windows

The best thing you can do if you want to run GSpeakers on Windows is
to download the binary package.

* Download and install the gtk+ runtime environment
  (http://gladewin32.sourceforge.net/)

* Download and unzip the GSpeakers zip file, double click on gspeakers.exe
  to start GSpeakers


Building GSpeakers
---------------

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

INSTALLATION INSTRUCTIONS FOR GTK+ AND GTKMM USING CYGWIN AND MINGW.
--------------------------------------------------------------------

1. Download and install the latest mingw (I used MinGW-3.1.0-1.exe). Install
   to default folder, c:\mingw.

2. Download and install the latest cygwin. You probably want to add the following
   to the default cygwin setup:

	Admin->cygrunsrv (so that you can run sshd)
	Archive->zip
	Archive->unzip
	Devel->autoconf
	Devel->automake
	Devel->cvs
	Devel->libtool
	Devel->make
	Editors->nano
	Graphics->libpng2
	Net->Openssh
	Net->setup
	Web->wget

   At least autoconf, automake, make and libtool is required to setup a standard
   gtkmm build system.

3. Cygwins path needs to be modified, because we want to use mingws gcc.
   Edit /etc/profile to add /cygdrive/c/mingw/bin in front of the current PATH.

4. Download and install the GTK+2 development environment[0]. Install into
   c:\cygwin.

5. Download and install the GTK+2 runtime environment[0]. Install to standard
   location.

6. Download and install the gtkmm development environment[1]. Install to
   c:\cygwin. Answer yes to add it to the path.

7. You can now test whether GTK works by typing:
       pkg-config.exe gtk+-2.0 --modversion
   You can test gtkmm by a similar command:
       pkg-config.exe gtkmm-2.0 --modversion

   If they return something sane, you should be on your way.

8. "Hello World" can be compiled like this:
   GTK+:
       gcc foo.c `pkg-config.exe gtk+-2.0 --libs --cflags` -mno-cygwin -mms-bitfields
   gtkmm:
       g++ foo.cpp `pkg-config.exe gtkmm-2.0 --libs --cflags` -mno-cygwin -mms-bitfields

   The standard gtkmm examples is in /demo or /examples.

Now you should be able to compile and run the examples. Also you will find the gtkmm
standard demo in the start menu. Now it is time for the more tricky parts. We need to
have GNU Gettext and libxml2 working. Get them from the gnuwin32 project.

9. In order to compile a regular gtkmm application with autoconf, automake,
   libtool and gettext,  the following needs to be installed as well:

  - autoconf from cygwin setup
  - automake from cygwin setup
  - make from cygwin setup
  - libtool from cygwin setup
  - libiconv from www.mingw.org (install to standard location)
  - gettext from www.mingw.org (install to standard location)




[0] http://www2.arnes.si/~sopjsimo/gimp/stable.html
[1] http://www.pcpm.ucl.ac.be/~gustin/win32_ports/


Install mingw

Install msys

Install msys DTK

Install gtk+

Install gtkmm

Install emacs

Add paths

aclocal -I /mingw/share/aclocal

autoconf

automake -a

autoheader

./configure --enable-win32


=============
libmls-2.0.pc
=============

prefix=/target
exec_prefix=${prefix}
libdir=${exec_prefix}/libdir
includedir=${prefix}/include


Name: libXML
Version: 2.5.11
Description: libXML library version2.
Requires:
Libs: -L${libdir} -lxml2  -lm
Cflags: -I${includedir}/libxml2 -I${includedir}/libxml2


## Installation instructions for GSpeakers on Mac OS X
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
