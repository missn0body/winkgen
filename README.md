# winkgen

A sly little key generator, *wink*.
First written by anson in June 2020.

This terminal application allows you to generate
OEM and CD keys of a specific variety, reminiscent
of a large software company that is owned by a 
quite notable bill.

Command-line arguments are as follows:
> winkgen -v (--verbose) -a (--ansi) -o (--oem) -c (--cd) -h (--help)

These arguments can be put in any order, alternated
between their long and short forms, and their short
forms can be globbed, such as "-vao", giving winkgen
the instruction to set verbose mode and generate an
OEM key with ANSI escape codes providing color in the
terminal.

However if the '-h' flag is used with any other
option, then those other options will be nullified,
as the help flag is evaluated with higher priority
than the others.

Please read here for more information on the key
generation itself:
https://gurney.dev/posts/mod7/


### v.1.0.0 (First release)

(June 2020)
Run this application at the command line and it
will generate one legal OEM key and one legal CD key,
free of charge.

### v.1.0.2

(September 21, 2023)
Added command-line arguments, and selection between
generating OEM and CD keys. However this program will
not operate without command-line arguments.
