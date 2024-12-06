# Arduino-led-blink
C++/Arduino code to blink 4 LEDs via an Adafruit Metro

This is code I wrote for a project (to ultimately make a PCB with four LEDs) in Arduino with several patterns for four LEDs.
I haven't coded C++ in forever, and don't usually code with event loops, but it all works (at least with the breadboard setup I have and the Adafruit Metro).
YMMV.
My design has the pattern change based on a button press, the button-press code may not be the best but it works.

13 patterns, they are:
1. Blink.
2. Pulse -- uses fade-in and fade-out via PWM.
3. Rotate.
4. Alternate. 
5. Snake -- the start is a bit messed up but it works.
6. Rave, my favorite.
7. Random.
8. Random path -- randoms but only to the two adjacent LEDs.
9. Bounce.
10. Side-side.
11. Fire! (I like this one.)
12. Fireworks - my second favorite.
13. Count - complex, was a pain to code.
