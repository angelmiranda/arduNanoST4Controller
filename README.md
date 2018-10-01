# arduNanoST4Controller
Control manually or by ST4 protocol a telescope mount using an Arduino Nano.

The speed selector is done with two swithes in the next way;

- One swith to select over 3 speeds, x1, x8 and x16 times the sidereal rate.
- One "turbo" switch that multiplies the speed per 50 if it is on, so we can reach speeds of x50, x400 and x800
  
It allows communication with a PC using the USB port of Arduino. The communication is done using the ST4 protocol, so it adds the GOTO function using software like Cartes du Ciel or guiding with PHD. 

It extends the project developped by Kevin Ferrare for Arduino ST4 telescope control.

https://hackaday.io/project/4386-arduino-st4-telescope-control

The instructions to make it run are the same;

- Install the ASCOM plattform.
- Install the ASCOM driver (can be downloaded from Kevin Ferrare <a href="https://drive.google.com/file/d/0B04IJvbw0nqmeHJLQS13eVotQ3M/viewB">page</A>).
- Install the Arduno code in the hand controller.
