# specs
A re-writing of the specs pipeline stage from CMS, only changed quite a bit

"specs" is a command line utility for parsing and re-arranging text
input. It allows re-alignment of fields, some format conversion, and
re-formatting multiple lines into single lines or vice versa. Input
comes from standard input, and output flows to standard output.

The command line format is as follows:

  specs [switches] [spec-units]
  
switches
========
  -ws word-separator-chars
    determines what characters count as word separators. The default is
    tab, space, newline and carriage-return. In the argument, "\t" can 
    be used for tab, "\n" can be used for the newline character, "\r"
    can be used for the carriage return, "\w" can be used for all
    three, and "\W" for all three plus space (the defualt).
    
  -spc space-char
    determines what character will be used as the space character when
    padding output and separating output words. The default is space.
    
  -d
    Allows debug output to standard error
    
  -dd
    Allows a lot of debug output to standard error.
    
Spec Units
==========
Each spec unit specifies an action to be taken by the program. The spec
unit may span from one to four command line arguments.

The ordinary spec unit consists of four arguments, two of which may be
omitted:
  source [conversion] dest [alignment]
  
The "source" argument may be any of the following:
  - A range of characters, such as 5, 3-7, or 5.8, the last one 
    indicating 8 characters starting in the 5th position. Note that the
    indexing of characters is 1- rather than 0-based.
  - A range of words, such as w5 or w5-7, and again the word indexing
    is 1-based.
  - timestamp - an OS formatted timestamp, giving the exact time when
    the record was written.
  - timediff - an 8-char decimal number indicating the number of 
    seconds since the invocation of the program.
  - a string literal, optionally enclosed by single quotes. Note that
    to include the single quotes on the command line required you to 
    enclose them in double quotes.
    
The "dest" argument specifies where to put the source:
  - absolute position (such as "1")
  - range (such as 1-5 or 1.5)
  - "n" (for "next") for placing the output immediately after the 
    previous output
  - "nw" (for "next word") for placing the output following a space 
    character after the previous output
    
The alignment argument can be 'l', 'c', or 'r', for "left", "center",
and "right" respectively.

The conversion argument can specify any of the following conversions:
 - rot13 - encrypts the bytes using the ROT-13 cipher
 - x2d - converts hex data to decimal
 - d2x - converts decimal data to hex
 - b2x - converts binary data to hex
 - x2b - converts hex data to binary
 - x2bt - converts hex data to binary, padded with zeros to include all
   nibbles.
 - ucase - converts text to uppercase
 - lcase - converts text to lowercase
 
There are also three special spec units, that may be used:
  "read" - causes the program to read the next line of input
  "write"- causes the program to write to output and reset the output 
      line.
  "redo" - causes the program to use the line of output as the new 
      input, and reset the output line.
      
Example:
  "ls -l" yields this:
total 352
-rw-r--r--@ 1 ynir  admin    574 Aug 25  2009 Makefile
-rw-r--r--@ 1 ynir  admin   3542 Nov 23 00:21 README
-rw-r--r--@ 1 ynir  admin    362 Nov 19 08:31 conversion.h
-rw-r--r--  1 ynir  admin    984 Nov 11 17:45 ls.txt
-rw-r--r--@ 1 ynir  admin   2233 Nov 23 00:03 main.cc
-rw-r--r--  1 ynir  admin   9412 Nov 23 00:11 main.o
-rw-r--r--@ 1 ynir  admin   6567 Nov 23 00:09 spec_build.cc
-rw-r--r--  1 ynir  admin  16776 Nov 23 00:11 spec_build.o
-rw-r--r--@ 1 ynir  admin   5494 Nov 19 08:30 spec_convert.cc
-rw-r--r--  1 ynir  admin  17004 Nov 23 00:11 spec_convert.o
-rw-r--r--@ 1 ynir  admin  11419 Nov 23 00:10 spec_params.cc
-rw-r--r--  1 ynir  admin  21080 Nov 23 00:11 spec_params.o
-rw-r--r--@ 1 ynir  admin    375 Nov 11 09:29 spec_vars.cc
-rw-r--r--  1 ynir  admin   4800 Nov 23 00:11 spec_vars.o
-rwxr-xr-x  1 ynir  admin  36740 Nov 23 00:11 specs
-rw-r--r--@ 1 ynir  admin   1547 Nov 23 00:10 specs.h

Let's run it though a spec:
  ls -l | specs 12-x 1 redo w2 1 w4 d2x 8.8 r w8 17
The first spec unit converts it to this:
 1 ynir  admin    574 Aug 25  2009 Makefile
 1 ynir  admin   3542 Nov 23 00:21 README
 1 ynir  admin    362 Nov 19 08:31 conversion.h
 1 ynir  admin    984 Nov 11 17:45 ls.txt
 1 ynir  admin   2233 Nov 23 00:03 main.cc
 1 ynir  admin   9412 Nov 23 00:11 main.o
 1 ynir  admin   6567 Nov 23 00:09 spec_build.cc
 1 ynir  admin  16776 Nov 23 00:11 spec_build.o
 1 ynir  admin   5494 Nov 19 08:30 spec_convert.cc
 1 ynir  admin  17004 Nov 23 00:11 spec_convert.o
 1 ynir  admin  11419 Nov 23 00:10 spec_params.cc
 1 ynir  admin  21080 Nov 23 00:11 spec_params.o
 1 ynir  admin    375 Nov 11 09:29 spec_vars.cc
 1 ynir  admin   4800 Nov 23 00:11 spec_vars.o
 1 ynir  admin  36740 Nov 23 00:11 specs
 1 ynir  admin   1547 Nov 23 00:10 specs.h

Then after the redo, we get this:
ynir        23e Makefile
ynir        dd6 README
ynir        16a conversion.h
ynir        3d8 ls.txt
ynir        8b9 main.cc
ynir       24c4 main.o
ynir       19a7 spec_build.cc
ynir       4188 spec_build.o
ynir       1576 spec_convert.cc
ynir       426c spec_convert.o
ynir       2c9b spec_params.cc
ynir       5258 spec_params.o
ynir        eae spec_vars.cc
ynir       12c0 spec_vars.o
ynir       8f84 specs
ynir        60b specs.h

      
Alternatively, let's arrange this on multiple lines:
  ls -l | specs w9 1 write "Owner:" 3 w3 10 write "Size:" 3 w5 10-20 r
Makefile
  Owner: ynir
  Size:          574
README
  Owner: ynir
  Size:         5834
conversion.h
  Owner: ynir
  Size:          362
list.txt
  Owner: ynir
  Size:          978
ls.txt
  Owner: ynir
  Size:          984
main.cc
  Owner: ynir
  Size:         2233
main.o
  Owner: ynir
  Size:         9412

Finally, let's make our own version of the multi-column display:
  ls -l | specs w9 1 read w9 26 read w9 51
                         Makefile                 README
conversion.h             main.cc                  main.o
spec_build.cc            spec_build.o             spec_convert.cc
spec_convert.o           spec_params.cc           spec_params.o
spec_vars.cc             spec_vars.o              specs

Note that the output is truncated. The last two entries did not appear, 
becuase the spec unit "read" failed.
