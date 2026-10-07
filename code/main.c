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
#define SEG_P         140  // 0x8C 
#define SEG_H         137  // 0x89
#define SEG_H_LOW     139  // 0x8B
#define SEG_K         148  // 0x94 
#define SEG_OFF       255  // 0xFF
#define SEG_D_LOW     161  // 0xA1 (d)
#define SEG_U         193  // 0xC1
#define SEG_N         200  // 0xC8
#define SEG_DASH      191  // 0xBF
#define SEG_E         134  // 0x86
#define SEG_R_LOW     175  // 0xAF  (r)
#define SEG_O_LOW     163  // 0xA3  (o)
#define SEG_S         146  // 0x92 
#define SEG_F         142  // 0x8E
#define SEG_A         136  // 0x88
#define SEG_E_LOW     132  // 0x84 (e)
#define SEG_N_LOW     171  // 0xAB (n)
#define SEG_M_RIGHT   248  // 0xF8 (7)
#define SEG_DOT       0x7F 

/*Strut for user_data. Stores time, speed, fastest speed and time,
 * slowest speed and time, average speed and time.
 * By Elvis Yarquah*/

struct user_data {
  int speed[10];
  int time[10];
  int fastest;
  int slowest;
  int average_speed;
  int average_time;
};


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
#define detect_required ((unsigned int) 10) //in clock cycles

/*User inputs*/
int distance; //in cm
int units; //km/h, mph, m/s
int runs;

/*Global_values*/
int global_time = 0;
unsigned int maxTime_seconds = 10; //in seconds

/*Timer setup. 
* Modified by Blomquist, Gustaf. Origins from Lab 3*/
void start_timer(void){
/*Counts down from maxTime_seconds*/
  *(TMR1_PERLO) = ((maxTime_seconds * 30000000) - 1) & 0xFFFF; 
  *(TMR1_PERHI) = ((maxTime_seconds * 30000000) - 1) >> 16; 
  *(TMR1_CTRL) = 0x4; //Startbit only. 0111 instead of 0110, ITO bit set to 1. 0x6 is TMR Start and Control
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
*By Gemini AI, Blomquist Gustaf*/
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

  while (read_GPIO_zero() || read_GPIO_one()){
  }

  int runTime_Ms = 0;
  int last_gate = -1;

  while (1) {
    if (read_GPIO_zero()){
      last_gate = 1;
      break;
    } else if (read_GPIO_one()){
      last_gate = 0;
      break;
    }
  }
  maxTime_seconds = 10; 
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
 * By Elvis Yarquah */
void show_prompt(int label) {
  set_displays(6, label);
  set_displays(5, SEG_DASH);
  set_displays(4, SEG_OFF);
  set_displays(3, SEG_OFF);
  set_displays(2, SEG_OFF);
  set_displays(1, SEG_OFF);
}

/*Displays the units
ms/s, km/h and mp/h if units is 1,2 or 3.
* By Gustaf Blomquist*/
void show_units(int label){
  if(label == 1){
    set_displays(3, SEG_N);
    set_displays(2, SEG_M_RIGHT & SEG_DOT);
    set_displays(1, SEG_S);
  } else if(label == 2){
      set_displays(4, SEG_H);
      set_displays(3, SEG_N);
      set_displays(2, SEG_M_RIGHT & SEG_DOT);
      set_displays(1, SEG_H_LOW);
  } else if(label == 3){
      set_displays(4, SEG_N);
      set_displays(3, SEG_M_RIGHT & SEG_DOT);
      set_displays(2, SEG_P & SEG_DOT);
      set_displays(1, SEG_H_LOW);
  }
}

/* Shows specific max or min values depending on label
* By Gustaf Blomquist */
void show_minmax(int label){
  set_displays(6, label);
  set_displays(5, SEG_OFF);
  set_displays(4, SEG_OFF);

  if(label == SEG_D_LOW){
    set_displays(4, convert_to_segm7(1));
    set_displays(3, SEG_DASH);
    set_displays(2, convert_to_segm7(9));
    set_displays(1, convert_to_segm7(9));
  } else if(label == SEG_U){
      set_displays(3, convert_to_segm7(1));
      set_displays(2, SEG_DASH);
      set_displays(1, convert_to_segm7(3));
  } else if(label == SEG_N){
      set_displays(3, convert_to_segm7(1));
      set_displays(2, SEG_DASH);
      set_displays(1, convert_to_segm7(9));
  }

}
/*Reads and stores users choices from prompt
 * Gets input by polling switches
 * Displays input
 * Accepts input when button is released
 * By Elvis Yarquah, Gustaf Blomquist*/

int read_prompt(int label){
  int user_choice = 0;

  show_minmax(label);
 
  while(!get_btn()){} 
  
  while(get_btn()){}  

  show_prompt(label);
  
  if (label == SEG_U){
    while(!get_btn()){
      user_choice = get_sw();
      show_units(user_choice);
    }
    while(get_btn()){
      user_choice = get_sw();
      show_units(user_choice);
    }
      return user_choice;
  }
  while(!get_btn()){
    user_choice = get_sw();
    show_number(user_choice);
  }
  
  while(get_btn()){
    user_choice = get_sw();
    show_number(user_choice);
  }

  return user_choice;

}

/*Prompts user to input distance, unit of speed and amount of runs wanted.
 * distance has to be greater than 0 and less than 100.
 * units from 0 - 3.
 * runs from 0 - 9.
 * By Elvis Yarquah*/
void prompt_user(){
  distance = 0;
  units = 0;
  runs = 0;

  while(!distance && distance < 100){
    distance = read_prompt(SEG_D_LOW);
  }

  while(!(units && units < 4)){
    units = read_prompt(SEG_U);
  }

  while(!(runs && runs < 10) ){
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
    set_displays(3, (convert_to_segm7(hun_digit) & SEG_DOT));
  
    set_displays(2, convert_to_segm7(ten_digit));
    set_displays(1, convert_to_segm7(one_digit));
  } else if (speed >= 10000) {
    
    set_displays(6, convert_to_segm7(ten_thou_digit));
    set_displays(5, convert_to_segm7(thou_digit));
    set_displays(4, (convert_to_segm7(hun_digit) & SEG_DOT));
    set_displays(3, convert_to_segm7(ten_digit));
     set_displays(2, convert_to_segm7(one_digit));
    set_displays(1, SEG_OFF);

    } else if (speed >=1000){

    set_displays(6, convert_to_segm7(thou_digit));
    set_displays(5, (convert_to_segm7(hun_digit) & SEG_DOT));
    set_displays(4, convert_to_segm7(ten_digit));
    set_displays(3, convert_to_segm7(one_digit));
    set_displays(2, SEG_OFF);
    set_displays(1, SEG_OFF);

  } else {

    set_displays(6, (convert_to_segm7(hun_digit) & SEG_DOT));
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
  int speed = 0;

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
     set_displays(5, SEG_R_LOW);
     set_displays(4, SEG_R_LOW);
     set_displays(3, SEG_O);
     set_displays(2, SEG_R_LOW);
     set_displays(1, SEG_OFF);
    }
}

/*show_time displays global_time (ms) as seconds with three decimals, e.g. 0.234.
 * By Elvis Yarquah*/
void show_time(int t){

  if (t > 9999) t = 9999;
  if (t < 0) t = 0;

  int thou_digit = t / 1000;
  int hun_digit  = (t % 1000) / 100;
  int ten_digit  = (t % 100) / 10;
  int one_digit  = t % 10;

  set_displays(6, convert_to_segm7(thou_digit) & SEG_DOT);
  set_displays(5, convert_to_segm7(hun_digit));
  set_displays(4, convert_to_segm7(ten_digit));
  set_displays(3, convert_to_segm7(one_digit));
  set_displays(2, SEG_OFF);
  set_displays(1, SEG_S);
}

/*Delays by n seconds on the hardware level as not to interrupt software level.
 * returns 1 if sucessfully timed out. 
 * By Elvis Yarquah*/

int delay(int seconds){
  maxTime_seconds = seconds;
  *TMR1_STAT = 0;
  start_timer();
  while(1){
    if (read_GPIO_one() || read_GPIO_zero()) {
      stop_timer();
      return 0;
    }

    if (timertest()) {
      *TMR1_STAT = 0;
      stop_timer();
      return 1;
    }
  }
}


/*Displays the current run number.
 * Elvis Yarquah*/
void show_run(int run_number){
  set_displays(6, convert_to_segm7(run_number));
  set_displays(5, SEG_OFF);
  set_displays(4, SEG_O);
  set_displays(3, SEG_F);
  set_displays(2, SEG_OFF);
  set_displays(1, convert_to_segm7(runs));
}


/*Check GPIOS constantly polls the GPIOs and counts from 1-4.
 * accepts run_number
 * If it gets a zero during the count it restarts.
 * If it never reads a 1 then it "Throws" an error
 * By Elvis Yarquah*/

void check_gpios(int run_number) {
  start_GPIO_input();
  show_run(run_number);

  int counter = 0;
  while((!read_GPIO_one() && !read_GPIO_zero()) && counter < 9){
    if (counter == 0) {
      counter++;
    }

    if (!delay(1)) {
      set_leds(0);
      break;
    }

    set_leds(counter);
    counter += counter;
  }

  if (counter == 0) {
    show_error();
  }

  if (counter < 9) {
    set_leds(0);
    check_gpios(run_number);
  }

  set_leds(0);
}



/*Start run intialize a check to make sure the lasers are connected.
 * accepts run_number
 * "Throws" an error and restarts run  if time is 0.
 * returns speed.
 * By Elvis Yarquah
 * */
int start_run(int run_number){
  int speed = 0;
  check_gpios(run_number);
  global_time  = sprint();

   if (global_time == 0) {

     while(!get_btn()) show_error();
     
    return start_run(run_number);
  }


  speed = calculate_speed();
  
  while(!get_btn()){
    show_speed(speed);
  }

  show_time(global_time);
  
  while(get_btn()){
  
  }


  return speed;
}

/*Starts n amount of runs. Stores values in a struct through pointers.
 * By Elvis Yarquah*/

void start_runs(struct user_data *u){
  int fastest_time = 999999999;
  int slowest_time = 0;
  int total_time = 0;
  int total_speed = 0;


  for (int i = 1; i <= runs; i++ ) {
    int speed = start_run(i);
    u->speed[i] = speed;
    u->time[i] = global_time;

    total_time += global_time;
    total_speed += speed;

    if(global_time < fastest_time){
      fastest_time = global_time;
      u->fastest = i;
    }
    if(global_time > slowest_time){
      slowest_time = global_time;
      u->slowest = i;
    }
  }

  u->average_speed = total_speed / runs; 
  u->average_time = total_time / runs; 
}


/*Delay for show_user_stats
 * By Elvis Yarquah*/

void stat_delay(int seconds){
  maxTime_seconds = seconds;
  *TMR1_STAT = 0;
  start_timer();
  while(1){

    if (timertest()) {
      *TMR1_STAT = 0;
      stop_timer();
      break;
    }
  }
}

/*Clears the seven segment displays*
 * By Elvis Yarquah */
void clear_displays(){
  
    set_displays(6, SEG_OFF);
    set_displays(4, SEG_OFF);
    set_displays(5, SEG_OFF);
    set_displays(3, SEG_OFF);
    set_displays(2, SEG_OFF);
    set_displays(1, SEG_OFF);
}
/*Shows stats from struct on the seven segment displays.
 * Loops fastest to average
 * By Elvis Yarquah, modified by Gustaf Blomquist*/

void show_user_stats(struct user_data *u){
  while(!get_btn()){
    set_displays(6, SEG_OFF);
    set_displays(5, SEG_D_LOW);
    set_displays(4, SEG_O_LOW);
    set_displays(3, SEG_N_LOW);
    set_displays(2, SEG_E_LOW);
    set_displays(1, SEG_OFF);
  }

  while(get_btn()){}

  int fastest = u->fastest;
  int slowest = u->slowest;
  
  while(!get_btn()){
    set_displays(6, SEG_F);
    set_displays(5, SEG_DASH);
    set_displays(4, convert_to_segm7(fastest));
    set_displays(3, SEG_OFF);
    set_displays(2, SEG_OFF);
    set_displays(1, SEG_OFF);

    stat_delay(3);
    if (get_btn()) break;

    clear_displays();
    show_speed(u->speed[fastest]);
    stat_delay(3);
    clear_displays();
    show_time(u->time[fastest]);
    
    stat_delay(3);
    if (get_btn()) break;

    clear_displays();
    set_displays(6, SEG_S);
    set_displays(5, SEG_DASH);
    set_displays(4, convert_to_segm7(slowest));
    set_displays(3, SEG_OFF);
    set_displays(2, SEG_OFF);
    set_displays(1, SEG_OFF);
    

    stat_delay(3); 
    if (get_btn()) break;

    clear_displays();
    show_speed(u->speed[slowest]);
    stat_delay(3); 
    clear_displays();
    show_time(u->time[slowest]);

    stat_delay(3); 
    if (get_btn()) break;

    clear_displays();
    set_displays(6, SEG_A);
    stat_delay(3); 
    clear_displays();
    show_speed(u->average_speed);
    stat_delay(3); 
    clear_displays();
    show_time(u->average_time);
    stat_delay(3);
    if (get_btn()) break;

    clear_displays();

    
  }
  while(get_btn()){}
  clear_displays();
}

/* Resets all values in the user_data struct to 0
* Allowing the program to be looped 
* By Gemini AI*/
void reset_user_data(struct user_data *u) {
  u->fastest = 0;
  u->slowest = 0;
  u->average_speed = 0;
  u->average_time = 0;
  
  for (int i = 0; i < 10; i++) {
    u->speed[i] = 0;
    u->time[i] = 0;
  }
}

void main() {
  struct user_data user;

  while(1){
    reset_user_data(&user);
    prompt_user();
    start_runs(&user);        //pointer to created user_data struct
    show_user_stats(&user);
  }
}
