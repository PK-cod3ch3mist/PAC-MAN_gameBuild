# PAC-MAN_gameBuild
A simple version of Pac-Man written in C++. Needs no graphics support. Entirely based on ASCII art/characters!!

The file pac.cpp contains code that will work on any macOS machine.
The file Pac_linux.cpp contains code that will work on any linux machine.

If the Pac_linux file does not work correctly, note the following:

  1.The program uses ANSI escape codes in which I used \u001b as the ESC character; try using \033 instead.
  
  2.The program requires C++11 standard.
  
  3.The program uses usleep method.
  
The program has been tested on machines running macOS.
