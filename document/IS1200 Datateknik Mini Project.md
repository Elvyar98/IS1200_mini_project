---
id: "IS1200 Datateknik Mini Project"
aliases: []
tags:
  - notes
  - vocation
  - project
course: "[[IS1200 Datorteknik]]"
created: 19700101{{time:HHmm}}
project: "[[Högskoleingenjör i Datateknik|TIDAB]]"
---

### IS1200 Datateknik Mini Project

#### Objectives & Requirements

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

#### Solutions
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

#### Extra Requirements
- Store the measurements and let the user browse statistics (e.g., max/min/average of the last N runs), instead of only showing the latest speed;
- Let the user switch between units (m/s, km/h, mph), e.g., with the toggle switches;
- Make the distance between the two beams a parameter the user can configure from the board (a small menu), rather than a hardcoded constant;
- Handle the edge cases explicitly: a beam blocked for too long, or an object that turns back before reaching the second beam.


#### Software Architecture
The program will be written i C with inline assembly if needed. We can reuse a lot of the code written for labb3. 

##### High level flow
The user selects distance, units and the amounts of tries they want to do and gets ready to run. 
The leds shows a waiting phase by blinking in sequence. As soon as the user passes Set A, the timer starts. When they pass Set B the timer stops.
Show speed on 7 segment display.
The displays switch back to the waiting phase to indicate the start of the next run. This continues until all runs are finished. The users can then toggle through a menu on the 7 segment displays to see there fastest, slowest and average speed between the N runs.

##### Methods.

###### Polling
- Kolla om GIPO har ändrat state. 
- Hålla koll på Set A och Set B
- Skickar vidare informationen till timer
###### Timer
- Startas när Set A blir 0.
- Medans Set A är 0 bryr sig inte om Set A förrän Set B är också noll.
- Om set B inte blir noll inom ett par sekunder kan skicka error.
-  
###### Speed calculations
- Ta in avstånd och tiden och räknar ut hastighet.
- Räknar ut hastighet i KM/H , mph
- Skickar till Store data.
###### Store data
- Store data har antal N runs some körs.
- Räknar fastest, average and slowest.
- Skickar till menu.
###### Menu
- Får en struct gällande user och skickar till display
- lägger fastest på 01
- Slowest på 10.
- Average på  11
###### Display
- Prompt user to choose distance, units and N amounts of run.  Send distance and unit to speed calculations. N to store data.
- led lights  waiting phase
- Display speed after run. 
- Display n of time 
- Display stats on menu after completed run based on switches. 

