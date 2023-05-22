                           ESTEREL Version v6_01
                           ---------------------

                                HOW TO USE
                              (Remote Linux)


This is release v6_01 of the Esterel V6 compiler by INRIA / CMA.
You have the choice of working in Remote Linux where the compiler has
been set up for you, or install the compiler for use on your own
computer. This document provides the instruction for working in
Remote Linux. If you wish to install the compiler on your computer,
download esterelv6_01.run from Cecil.

Download Makefile, ctype.c, cell.strl from Cecil. Assuming you
downloaded them to a folder named "example" on your desktop. To
compile the example, open up a terminal, then type the following
commands:

$ cd ~/Desktop/example
$ make cell.xes
$ ./example.xes

To verify the example, open up a terminal, then type the following
commands:

$ cd ~/Desktop/example
$ make example.xev
$ ./example.xev

To check for presence of signals, click the red box on the signal
outputs column, then click on the signals to be checked. Alternatively,
click on the blue box to check for absence of signals.

If for whatever reason, the make says the file is already up to date,
do

$ make clean

then make the xes/xev file again.

