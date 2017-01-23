Running different fields through different filters
==================================================

I'm not gonna read all of this
------------------------------

requires the plan 9 shell [rc](http://git.suckless.org/9base), lua (not terra, fuck you github)
and a posixy environment, `make install` for installing and `make uninstall` for uninstalling,
more information in the man-page.

An image says more than a thousand words
----------------------------------------

```
                                     +-----> filter1 (1st input field)+----+
                                     |                                     |
+-------+     +----------------------+                                    ++----+
| input |  |  |treat filter1 filter2 +-----> filter2 (2nd input field)+---+paste|
+-------+     +----------------------+                                    ++----+
                                     |                                     |
                                     +-----> cat (rest of the line)+-------+
```

The shameless self-promotion
----------------------------

Do you know du(1)?
It prints the directory tree and the corresponding sizes. Pretty simple. And - much more interesting in this case -
du(1) has an option -h, which means it prints the file sizes in human-readable formats.

What if I told you that this option is unneeded?

You can emulate it with the following command
(assuming you have z3bras awesome utility [human(1)](http://git.z3bra.org/human/log.html)
installed, but you can also use numfmt):

	du -b | treat human

This is neat, huh? At least, I hope it is.
And unlike du -h, you can have the whole thing sorted as well (with all files):

	du -ab | sort -n | treat human

This is much harder with du -h. If your sort(1) has an option to
sort human-readable numbers, it is possible, put that is a hell of an
assumption to make.

treat(1) is exactly for that - running different fields of the input through different text filters, in parallel.

It runs the nth field of the input through the nth filter specified (cat(1) for the fields without a specified filter).

More examples:

	#file calcs with two fields: 1. username, 2. bc code, separated by colons
	treat -i : cat bc <calcs

I also hope this encourages the programming of more specific filters,
such as the existing factor(1) and human(1), like for example a program that
just prints the hashsum of it's line or something like that.
I don't know. Think for yourself one single time.

Okay, looks quite - well, acceptable, how do I install it?
--------------------------------------------------

	make install

At the moment, you need the plan 9 shell [rc](http://git.suckless.org/9base) and lua for this.
You probably have got mktemp(1), seq(1), mkfifo(1), paste(1), cat(1),
rm(1) and rmdir(1) already installed.

No way that I am going to run this on my machine, how do I get rid of it?
-------------------------------------------------------------------------

	make uninstall

License
=======

MIT license, more details in ./LICENSE.
