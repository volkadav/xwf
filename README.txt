xwf -- simple command line xml well-formedness checker

m. jackson, Jan 2005. p3rlm0nk@yahoo.com. public domain.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

To build this, you will need James Clark's expat library,
which is commonly available on many platforms.  If you
happen to be running debian linux (on which this was 
developed and tested), running "apt-get install libexpat1-dev"
as root will install the required packages.

installation:
	0. ensure you have the right libs and such
	1. "make"
	2. "make install"

(default is a stripped, mildly optimized "xwf" in /usr/local/bin)

usage:
	xwf -h | [-mv] filename
		-h: this help message
		-m: move file to file.INVALID if it is invalid XML
		-v: verbosely report file status information
			(otherwise, exists EXIT_SUCCESS if file OK,
			and EXIT_FAILURE if file is not OK)
