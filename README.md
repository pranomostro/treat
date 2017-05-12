Running different fields through different filters
==================================================

treat is a unix utility for running different fields of the input
through different filters. The nth field is run through the nth filter
specified, cat is used for the rest of the line.

	                                       +-----> filter1 (1st input field)+----+
	                                       |                                     |
	+-------+     +---------------------+  +                                     |   +-----+    +------+
	| input |  |  |treat filter1 filter2|--|+-----> filter2 (2nd input field)+---|---|paste| |  |output|
	+-------+     +---------------------+  +                                     |   +-----+    +------+
	                                       |                                     |
	                                       +-----> cat (rest of the line)+-------+

Examples
---------

Emulate `du -h`:

	du -b | treat human

Emulate `du -abh | sort -h`:

	du -ab | sort -n | tr '\t' ' ' | treat human

Run calculations submitted by users from a colon-separated file.

	treat -i : cat 'bc -l' <calcs

Installation
-------------

	make install

for installation and

	make uninstall

for deinstallation.

Requirements
------------

* A make implementation
* A C compiler
* A POSIX system, needed for execl(), fork(), getline(), mkdtemp(), mkfifo() and wait()

License
=======

The [MIT/X11 license](./LICENSE).
