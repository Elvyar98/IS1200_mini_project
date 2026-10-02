# IS1200 Mini Project

##  Objectives & Requirements

The objective of this project is to develop a double laser tripwire system.
Using two sets of lasers and receivers, the system should be able to calculate
the speed with which either an object or a human interrupts set 1 and set 2.
The system should also work in both directions, meaning both sets should be
able to stop and start the timer depending on whether they were interrupted
first or second. If the second set is not activated within a certain time of
the first set being activated, the timer should stop and reset.
The sensors and the transmitters should be able to be placed far apart
in order to allow a human to safely run in between them in order to get
their speed measured.

## Solutions
We intend to develop on the DTEK-V board and use I/O with two pho-
totransistors. The phototransistors will be in separate housings and light
tunnel in order to avoid other lights to trigger them than the lasers. The two
lasers will be on a separate platform aimed with its own battery power, the
platform will then be pointed towards the corresponding phototransistors.
The distance between the two wires is static and is planned to be around
25 cm. When the phototransistor loses the light signal the wire is cut and
the timer starts. The timer will end when the second wire is cut. We will
use the DTEK’s screen to display the calculated speed based on the known
distance and the measured time.
A small sound from an added speaker can be used in order to indicate
and confirm that the wire is cut, different sounds for the two wires would
be beneficial for later verification.

## Extra Requirements
- Store the measurements and let the user browse statistics (e.g., max/min/average of the last N runs), instead of only showing the latest speed;
- Let the user switch between units (m/s, km/h, mph), e.g., with the toggle switches;
- Make the distance between the two beams a parameter the user can configure from the board (a small menu), rather than a hardcoded constant;
- Handle the edge cases explicitly: a beam blocked for too long, or an object that turns back before reaching the second beam.


