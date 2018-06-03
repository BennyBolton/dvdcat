DVDCAT
======

Version 0.1.1

dvdcat is a tool for piping dvds using libdvdread.

libdvdread is required to build and use dvdcat, and libdvdcss is optionally
required to decrypt dvds.

BUILDING
--------

Run `make` to build dvdcat. This will compile all c sources, and produce the
dvdcat binary.

INSTALLING
----------

Run `make install` to install dvdcat, this will copy the dvdcat binary to
`/usr/bin`.

USAGE
-----

dvdcat will write the dvd contents to standard output, which can then be piped
to a file, or piped to ffmpeg in order to encode on the fly. Run
`dvdcat --help` to see the available options.