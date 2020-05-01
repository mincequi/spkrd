spkrd
=====

spkrd (Speaker Design) is an update to the GSpeakers and GSpeakers-Redux application written originally by Daniel Sundberg and originally hosted at [SourceForge](https://sourceforge.net/projects/gspeakers/).

The purpose of this repository is to bring a high quality loudspeaker design software to Linux. Extensive work is underway to cleanup the deprecated features in GTK2 and to use GTK3. The old and dusty code has been cleaned up to align with modern C++ and we require C++14 as the working standard. The build system has been ported to CMake for ease of compilation.

Requests to add and improve features, bug reports and collaboration are strongly encouraged. I welcome your forks and pull requests!

## Compiling and installing spkrd with CMake

The cross-platform nature of `CMake` allows mostly the same compilation steps across platforms.  Generally you want to create a separate build directory and run

```
cmake .. && cmake --build .
```
from inside the `build` directory.

You can simply install using
```
sudo cmake --install .
```

## Features

Spkrd aims to be a high quality, open source and extendable loudspeaker design program written using GTK and modern C++. The current features are:

- Modern compilation toolchain with CMake
- Graphical interface written in GTK3
- Sealed enclosure design including frequency response for different alignments
- First to fourth order crossover design with SPICE simulation
- Ability to provide user defined driver frequency response and impedance curves
- Test routines to ensure correctness of results

If you think any features are missing or have any suggestions for improvements, no matter how small, please open an issue and start contributing!

## Code contributions

Code contributions in the form of additional design equations, user interface improvements or documentation advice are welcome. Please follow the prevealing code style (snake_case, descriptive names and heavy use of the standard library) and provide tests alongside code contributions.

Translations are welcome and assistance setting up the framework would be greatly appreciated.

Help compiling on Windows and MacOSX is also required.

## Code of conduct

We aim to provide a positive and welcoming community and we therefore do not discriminate on any basis.
