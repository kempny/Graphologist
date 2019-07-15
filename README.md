# Graphologist
RaspberryPI Touch screen signature network server.

The drawing part of the code based on the "Drawing in response to input" example
from developer.gnome.org

The start.c is server part (on RaspberryPI with touch screen).<br>
The wyslij.c is example client part, sending signing requests to the 
server and collecting the png image with signature.

Principle of operation described in file [operating.md](/screenshots/operating.md)

The program works in three language versions, english, german and polish (my native language).<br>
Instructions for adding new languages in file [locale.txt](/locale.txt)

You can test this on any Linux computer, in the absence of a touch screen you can sign witch a mouse.


