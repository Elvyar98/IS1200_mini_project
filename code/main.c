/* Code for IS1200 Mini project
 * Authors:
 *  Blomquist, Gustaf
 *  Yarquah, Elvis
 *
 * Created: 2026/10/02
 * Last modified: 2026/10/05
 */


/*Values for seven segment display
 * By Claude AI*/
#define SEG_OFF   255  // 0xFF
#define SEG_D     161  // 0xA1 (d)
#define SEG_U     193  // 0xC1
#define SEG_N     200  // 0xC8
#define SEG_DASH  191  // 0xBF
#define SEG_E     134  // 0x86
#define SEG_R     175  // 0xAF  (r)
#define SEG_O     163  // 0xA3  (o)

/*GPIO addresses*/
/*Adress for data for GPIO*/
#define GPIO1_DATA  ((volatile int *) 0x040000E0)
/*Adress for changing between Input and Output for GPIO*/
#define GPIO1_DIR   ((volatile int *) 0x040000E4)

/*Timer registers
*By Blomquist, Gustaf*/
#define TMR1_CTRL ((volatile unsigned short*) 0x04000024)
#define TMR1_PERLO ((volatile unsigned short*) 0x04000028)
#define TMR1_PERHI ((volatile unsigned short*) 0x0400002c)
#define TMR1_STAT ((volatile unsigned short *) 0x04000020)
#define TMR1_SNAPL ((volatile unsigned short *) 0x04000030)
#define TMR1_SNAPH ((volatile unsigned short *) 0x04000034)
/*Settings for Timer and GPIO detections*/
#define maxTime_seconds ((unsigned int) 10) //in seconds
#define detect_required ((unsigned int) 10) //in clock cycles

/*User inputs*/
int distance; //in cm
int units; //km/h, mph, m/s
int runs;

<<<<<<< HEAD
=======

>>>>>>> upstream/main
/*Global_values*/
int global_time = 0;


<<<<<<< HEAD
/*Timer setup. 
* Modified by Blomquist, Gustaf. Origins from Lab 3*/
=======

/*Timer settings by Blomquist, Gustaf. Modified from Lab 3*/
>>>>>>> upstream/main
void start_timer(void){
/*Counts down from maxTime_seconds*/
  *(TMR1_PERLO) = ((maxTime_seconds * 30000000) - 1) & 0xFFFF; 
  *(TMR1_PERHI) = ((maxTime_seconds * 30000000) - 1) >> 16; 
  *(TMR1_CTRL) = 0x6; //due to 0111 instead of 0110, ITO bit set to 1. 0x6 is TMR Start and Control
}

/*To pause or stop timer
* By Gemini AI*/
void stop_timer(void){
  *TMR1_CTRL = (1 << 3);
}

/*Timer test for time-out.
* Modified by Blomquist, Gustaf. Origins from Lab 3*/
int timertest(void)
  {
  if((*TMR1_STAT) & 0x1){ /* Test time-out event flag */
    return(1);
  } else{
    return(0);
  }
} 

/* For saving the current timer value in order to measure time.
The result is then inverted to avoid getting time left and not time passed.
Time is calculated based on remaining clock cycles and converted into Milliseconds.
The unsigned Int is then casted into Int for the return value.
*By Gemini AI, modified by Blomquist Gustaf*/
int get_timer_snapshot(void) {
    *TMR1_SNAPL = 0x0;

    unsigned short low = *TMR1_SNAPL;
    unsigned short high = *TMR1_SNAPH;

    unsigned int current_timer = ((unsigned int)high << 16) | (unsigned int)low;
    
    unsigned int cycles = 300000000U - current_timer;

    unsigned int time_in_Ms = cycles /30000U;

    //Explicit cast to regular int
    return (int)time_in_Ms;
}

void handle_interrupt(unsigned int cause) {}
void handle_exception(unsigned int cause) {}

/*GPIO start of input.
* By Blomquist, Gustaf*/
void start_GPIO_input(void) {
    /* Set pins for GPIO#0 and GPIO#1 to input mode, 0 = input */
    *GPIO1_DIR = 0x0;
}


/*Debouncing method,bitmask is the pin identifier. 
In order to avoid interference causing a false positive.
This also converts the lack of input, IE a tripped laserwire, into a positive value.
* By Blomquist, Gustaf*/
int debouncing (int bitmask){
  int count = 0;

  for (int i = 0; i < detect_required; i++){
    if ((*GPIO1_DATA & bitmask) == 0){
      count++;
    }
  }
  
return (count == detect_required);
}

/*GPIO reading functions with debouncing.
* By Blomquist Gustaf*/
int read_GPIO_zero (void) {
  return debouncing (0x1); //3 is bit 0 and 1. 1 or 2 for each pin.
}

int read_GPIO_one (void) {
  return debouncing (0x2); //3 is bit 0 and 1. 1 or 2 for each pin.
}

/*Measuring time between gates.
Starts inputs from GPIOs.
Checks that both gates are clear.
Then detects which of the two gates are passed first and starts timer.
Stops timer either when last gate is passed or maxTime-seconds have passed.
Converts time into ms, then returns the runTime in Ms
* By Blomquist, Gustaf, modified by Gemini AI*/
int sprint (void){
  start_GPIO_input();

  while (read_GPIO_zero() || read_GPIO_one()){
  }

  int runTime_Ms = 0;
  int last_gate = -1;
  int first_gate = -1;

  while (1) {
    if (read_GPIO_zero()){
      first_gate = 0;
      last_gate = 1;
      break;
    } else if (read_GPIO_one()){
      first_gate = 1;
      last_gate = 0;
      break;
    }
  }
  
  start_timer();

  while(1){
    if (last_gate == 0 && read_GPIO_zero()){
      runTime_Ms = get_timer_snapshot();
      break;
    }

    if (last_gate == 1 && read_GPIO_one()){
      runTime_Ms = get_timer_snapshot();
      break;
    }

    if (timertest()){
      runTime_Ms = 0;
      break;
    }
  }
  stop_timer();
   
  //if timer hits 10s then runTimeMs = 0; ERROR will be Notify User
  return runTime_Ms;
}


/*Function to get the input from switches
 * By Gustaf Blomquist*/
int get_sw(void){
    volatile int *switches = (volatile int *) 0x4000010;
    return *switches & 0x3FF;
}

/*Gets input from second button
 * By Gustaf Blomquist*/
int get_btn(void){
    volatile int *sec_btn = (volatile int*) 0x40000d0;
    return *sec_btn & 0x1;
}

/*Sets the leds through a mask.
 * By Gustaf Blomquist, Elvis Yarquah*/
void set_leds(int led_mask) {
    volatile int *leds = (volatile int*) 0x4000000;
    *leds = led_mask;
}

/*Segmentdisplay_number corresponds to the display the user wants to set; value
corresponds to the value to display.
Return value: nothing.
By Gustaf Blomquist, Elvis Yarquah*/
void set_displays(int display_number, int value) {
    volatile int *display;

    switch(display_number) {
        case 1:
            display = (volatile int*) 0x4000050;
            *display = value;
            break;
        case 2:
            display = (volatile int*) 0x4000060;
            *display = value;
            break;
        case 3:
            display = (volatile int*) 0x4000070;
            *display = value;
            break;
        case 4:
            display = (volatile int*) 0x4000080;
            *display = value;
            break;
        case 5:
            display = (volatile int*) 0x4000090;
            *display = value;
            break;
        case 6:
            display = (volatile int*) 0x40000a0;
            *display = value;
            break;
    }
}

/*convert_to_segm7 converts the ints to an active low binary code for the displays.
 * Returns a masked number to put bit 7 on 1 to deactivate the dots
 * By Elvis Yarquah, Gustaf Blomquist*/
int convert_to_segm7(int value) {
    int number;

    switch(value) {
        case 0:
          number =  64;
          break;
        case 1:
          number =  121;
          break;
        case 2:
          number =  36;
          break;
        case 3: 
          number =  48;
          break;
        case 4: 
          number =  25;
          break;
        case 5: 
          number =  18;
          break;
        case 6: 
          number =  2;
          break;
        case 7: 
          number =  120;
          break;
        case 8: 
          number =  0;
          break;
        case 9: 
          number =  16;
          break;
        default:
          number =  0;
          break;
    }

  return number | 0x80;
}

/*Displays the users chosen numbers on the seven segment display.
 * Shows number from left to right
 * By Elvis Yarquah*/

void show_number(int n){

    int thou_digit;
    int hun_digit;
    int ten_digit;
    int one_digit;

  if(n > 999){
     thou_digit = n / 1000;
     hun_digit = (n % 1000) / 100;
     ten_digit = (n  % 100) / 10;
     one_digit = (n % 10);

    set_displays(4, convert_to_segm7(thou_digit));
    set_displays(3, convert_to_segm7(hun_digit));
    set_displays(2, convert_to_segm7(ten_digit));
    set_displays(1, convert_to_segm7(one_digit));

  }else if(n > 99){
     hun_digit = n / 100;
     ten_digit = (n % 100) / 10;
     one_digit = (n % 10);

    set_displays(4, convert_to_segm7(hun_digit));
    set_displays(3, convert_to_segm7(ten_digit));
    set_displays(2, convert_to_segm7(one_digit));
    set_displays(1, SEG_OFF);

  } else if(n > 9) {
      ten_digit = n / 10;
      one_digit = n % 10;

    set_displays(4, convert_to_segm7(ten_digit));
    set_displays(3, convert_to_segm7(one_digit));
    set_displays(2, SEG_OFF);
    set_displays(1, SEG_OFF);

  } else {
    set_displays(4, convert_to_segm7(n));
    set_displays(3, SEG_OFF);
    set_displays(2, SEG_OFF);
    set_displays(1, SEG_OFF);

  }
}

/*Displays intial prompt for distance, units, amount of runs
 * Displays from left to right
 * By Elvis Yarquah*/
void show_prompt(int label) {
  set_displays(6, label);
  set_displays(5, SEG_DASH);
  set_displays(4, SEG_OFF);
  set_displays(3, SEG_OFF);
  set_displays(2, SEG_OFF);
  set_displays(1, SEG_OFF);

}

/*Reads and stores users choices from prompt
 * Gets input by polling switches
 * Displays input when button is pressed.
 * Accepts input when button is released
 * By Elvis Yarquah*/

int read_prompt(int label){
  while(!get_btn()){
    show_prompt(label);
  }

  int user_choice = get_sw();
  while(get_btn()){
    user_choice = get_sw();
    show_number(user_choice);
  }

  return user_choice;

}

/*Prompts user to input distance, unit of speed and amount of runs wanted.
 * distance has to be greater than 0.
 * units from 0 - 3.
 * runs from 0 - 10.
 * By Elvis Yarquah*/
void promt_user(){
  distance = 0;
  units = 0;
  runs = 0;

  while(!distance){
    distance = read_prompt(SEG_D);
  }

  while(!(units && units < 4)){
    units = read_prompt(SEG_U);
  }

  while(!(runs && runs < 11) ){
    runs = read_prompt(SEG_N);
  }
}

/*show_speed displays the speed in the hundreths.
 * It displays from left to right.
 * returns 1 if sucessful, 0 if not.
 * By Elvis Yarquah*/
void show_speed(int speed){

    int hun_thou_digit = speed / 100000;
    int ten_thou_digit = (speed % 100000) / 10000;
    int thou_digit = (speed % 10000) / 1000;
    int hun_digit = (speed % 1000) / 100;
    int ten_digit = (speed % 100) / 10;
    int one_digit = (speed % 10);
    
    
   if (speed >= 100000)  {
    set_displays(6, convert_to_segm7(hun_thou_digit));
    set_displays(5, convert_to_segm7(ten_thou_digit));
    set_displays(4, convert_to_segm7(thou_digit));
    set_displays(3, (convert_to_segm7(hun_digit) & 0x7F));
  
    set_displays(2, convert_to_segm7(ten_digit));
    set_displays(1, convert_to_segm7(one_digit));
  } else if (speed >= 10000) {
    
    set_displays(6, convert_to_segm7(ten_thou_digit));
    set_displays(5, convert_to_segm7(thou_digit));
    set_displays(4, (convert_to_segm7(hun_digit) & 0x7F));
    set_displays(3, convert_to_segm7(ten_digit));
     set_displays(2, convert_to_segm7(one_digit));
    set_displays(1, SEG_OFF);

    } else if (speed >=1000){

    set_displays(6, convert_to_segm7(thou_digit));
    set_displays(5, (convert_to_segm7(hun_digit) & 0x7F));
    set_displays(4, convert_to_segm7(ten_digit));
    set_displays(3, convert_to_segm7(one_digit));
    set_displays(2, SEG_OFF);
    set_displays(1, SEG_OFF);

  } else {

    set_displays(6, (convert_to_segm7(hun_digit) & 0x7F));
    set_displays(5, convert_to_segm7(ten_digit));
    set_displays(4, convert_to_segm7(one_digit));
    set_displays(3, SEG_OFF);
    set_displays(2, SEG_OFF);
    set_displays(1, SEG_OFF);
  }
}
/*calculate_speed takes in time(ms) in the form of an int.
 *Converts to ms/s, km/h and mi/h if units is 1,2 or 3.
 By Elvis Yarquah*/
int calculate_speed(){
  int speed;

  switch(units){
    case 1:
      speed =  (distance * 1000)/global_time;
      break;
    case 2:
      speed = (distance * 3600)/global_time;
      break;
    case 3:
      speed = (distance * 2237)/global_time;
      break;
  }

  return speed;
 
}


void show_error() {

    while(!get_btn()){
      
     set_displays(6, SEG_E);
     set_displays(5, SEG_R);
     set_displays(4, SEG_R);
     set_displays(3, SEG_O);
     set_displays(2, SEG_R);
     set_displays(1, SEG_OFF);
    }
}

void show_time(){
//show_speed
  //display 2 SEG_OFF
  //display 1 5
}

/*Check lasers constantly polls the GPIOs for a 1 and counts from 1-4. 
 * If it gets a zero during the count it restarts.
 * If it never reads a 1 then it "Throws" an error
 * By Elvis Yarquah*/

void check_gpios() {
  int counter = 0;
  while((read_GPIO_one() && read_GPIO_zero()) && counter < 9){
    if (counter == 0) {
      counter++;
    }
    set_leds(counter);
    counter += counter;
    maxTime_seconds = 1;
    start_timer();
  }

  if (counter == 0) {
    show_error();
  }

  if (counter < 9) {
    check_gpios();
  }

  set_leds(0);
}


/*Start run intialize a check to make sure the lasers are connected. 
 * "Throws" an error and restarts run  if time is 0.
 * returns speed.
 * By Elvis Yarquah
 * */
int start_run(){
  int speed = 0;
  check_gpios();
  global_time  = sprint();

   if (global_time == 0) {

     while(!get_btn) show_error();
     
    return start_run();
  }


  speed = calculate_speed();
  
  while(!get_btn()){
    show_speed(speed);
  }

  show_time();
  
  while(get_btn()){
  
  }


  return speed;
}

void main() {
  promt_user();
  start_run();
}

