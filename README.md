Running different fields through different filters
==================================================

Do you know du(1)?
It prints the directory tree and the corresponding sizes.
Pretty simple. And-much more interesting in this case-
du(1) has an option -h, which means it prints the file sizes
in human-readable formats.

What if I told you that this option is unneeded?

You can emulate it with the following command (assuming you have z3bras awesome utility human(1) installed):

    du -b | treat 'human'

This is neat, huh? At least, I hope it is.
And unlike du -h, you can have the whole thing sorted as well:

    du -b | sort -n | treat human

This is much harder with du -h. If your sort(1) has an option to
sort human-readable numbers, it is possible, put that is a hell of an
assumption to make (HAHA! He said 'ass'!).

treat(1) is exactly for that-running different fields of the input
through different text filters, in parallel.

It runs the nth field of the input through the nth filter specified
(cat(1) for the fields bigger than n).

But there is more: treat can also make the -h option for df(1) obsolete.
Here is how I do that:

    df | sed '1d' | tr -s '\t ' ' ' | treat cat human human human | column -t

Nobody needed that stupid first line anyway.
Also note that this command has haiku like qualities.

More examples:

    #file calcs with two fields: 1. username, 2. bc code
    treat 'cat' 'bc' <calcs

I also hope this encourages the programming of more specific filters,
such as the existing factor(1) and human(1), like for example a program that
just prints the hashsum of it's line or something like that.
I don't know. Think for yourself one single time.

Okay, looks quite-acceptable, how do I install it?
--------------------------------------------------

    make install

At the moment, you need the plan 9 shell rc and lua for this.
You probably have got mktemp(1), seq(1), mkfifo(1), paste(1), cat(1),
rm(1) and rmdir(1) already installed.

No way that I am going to run this on my machine, how do I get rid of it?
-------------------------------------------------------------------------

    make uninstall

License
=======

MIT license, more details in ./COPYING.
