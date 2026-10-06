/* Code for IS1200 Mini project
 * Authors:
 *  Blomquist, Gustaf
 *  Yarquah, Elvis
 *
 * Created: 2026/10/02
 * Last modified: 2026/10/05
 */
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

/*Timer registers*/
#define TMR1_CTRL ((volatile unsigned short*) 0x04000024)
#define TMR1_PERLO ((volatile unsigned short*) 0x04000028)
#define TMR1_PERHI ((volatile unsigned short*) 0x0400002c)
/*Settings for Timer and GPIO detections*/
#define maxTime_seconds ((unsigned int) 10)
#define detect_required ((unsigned int) 10)

/*User inputs*/
int distance; //in cm
int units; //km/h, mph, m/s
int runs;

/*Timer settings by Blomquist, Gustaf. Modified from Lab 3*/
void start_timer(void){

//divided by 1000 in order to get ms 29999999/1000
/*Counts down from 1 seconds*/
  *(TMR1_PERLO) = ((maxTime_seconds * 30000000) - 1) & 0xFFFF; 
  *(TMR1_PERHI) = ((maxTime_seconds * 30000000) - 1) >> 16; 
  *(TMR1_CTRL) = 0x6; //due to 0111 instead of 0110, ITO bit set to 1. 0x6 is TMR Start and Control

  //enable_interrupt(); //calls boot.S method for enabling interrupts
}

void stop_timer(void);
  

void handle_interrupt(unsigned int cause) {}
void handle_exception(unsigned int cause) {}

/*GPIO methods by Blomquist, Gustaf*/
void start_GPIO_input(void) {
    /* Set pins for GPIO#0 and GPIO#1 to input mode (0 = input) */
    *GPIO1_DIR = 0x3;
}

/*debouncing method,bitmask is the pin identifier, by Blomquist, Gustaf*/
int debouncing (int bitmask){
  int count = 0;

  for (int i = 0; i < detect_required; i++){
    if ((*GPIO1_DATA & bitmask) == 0){
      count++;
    }
  }
  
return (count == detect_required);
}

/*gate helping methods with built-in debouncing, by Blomquist Gustaf*/
int read_GPIO_zero (void) {
  return debouncing (0x1); //3 is bit 0 and 1. 1 or 2 for each pin.
}

int read_GPIO_one (void) {
  return debouncing (0x2); //3 is bit 0 and 1. 1 or 2 for each pin.
}


/*Measuring time between gates. By Blomquist, Gustaf*/
int sprint (void){
  start_GPIO_input();
  int runTime_Ms = 0;
  int passed_first_gate = 0;
  int last_gate = -1;
  int first_gate = -1;

  while (!passed_first_gate) {
    if (read_GPIO_zero()){
      first_gate = 0;
      last_gate = 1;
      passed_first_gate = 1;
    }
    if (read_GPIO_one()){
      first_gate = 1;
      last_gate = 0;
      passed_first_gate = 1;
    }
  }
  
  start_timer();

  while()

   
      
        //runTimeMs =
    
  
   
  //if timer hits 10s then runTimeMs = 0; ERROR
  return runTime_Ms;
}


/*Function to get the input from switches*/
int get_sw(void){
    volatile int *switches = (volatile int *) 0x4000010;
    return *switches & 0x3FF;
}

int get_btn(void){
    volatile int *sec_btn = (volatile int*) 0x40000d0;
    return *sec_btn & 0x1;
}



/*Segmentdisplay_number corresponds to the display the user wants to set; value
corresponds to the value to display.
Return value: nothing. */
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

void show_prompt(int label) {
  set_displays(6, label);
  set_displays(5, SEG_DASH);
  set_displays(4, SEG_OFF);
  set_displays(3, SEG_OFF);
  set_displays(2, SEG_OFF);
  set_displays(1, SEG_OFF);

}

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

void show_speed(int speed){
    if (speed == 9999999) {
    
      set_displays(6, SEG_E);
      set_displays(5, SEG_R);
      set_displays(4, SEG_R);
      set_displays(3, SEG_O);
      set_displays(2, SEG_R);
      set_displays(1, SEG_OFF);
      
      //restart run();
    }

    int hun_thou_digit = speed / 100000;
    int ten_thou_digit = (speed % 100000) / 10000;
    int thou_digit = (speed % 10000) / 1000;
    int hun_digit = (speed % 1000) / 100;
    int ten_digit = (speed % 100) / 10;
    int one_digit = (speed % 10);
  
    if (hun_thou_digit == 0 && ten_thou_digit != 0) {
    
    set_displays(6, convert_to_segm7(ten_thou_digit));
    set_displays(5, convert_to_segm7(thou_digit));
    set_displays(4, (convert_to_segm7(hun_digit) & 0x7F));
    set_displays(3, convert_to_segm7(ten_digit));
     set_displays(2, convert_to_segm7(one_digit));
    set_displays(1, SEG_OFF);

    } else if ((hun_thou_digit == 0 && ten_thou_digit == 0) && thou_digit != 0){

    set_displays(6, convert_to_segm7(thou_digit));
    set_displays(5, (convert_to_segm7(hun_digit) & 0x7F));
    set_displays(4, convert_to_segm7(ten_digit));
    set_displays(3, convert_to_segm7(one_digit));
  
    set_displays(2, SEG_OFF);
    set_displays(1, SEG_OFF);

  } else if ((hun_thou_digit == 0 && ten_thou_digit == 0) && (thou_digit == 0 && (hun_digit != 0 || ten_digit != 0)))  {

    set_displays(6, (convert_to_segm7(hun_digit) & 0x7F));
    set_displays(5, convert_to_segm7(ten_digit));
    set_displays(4, convert_to_segm7(one_digit));
    set_displays(3, SEG_OFF);
  
    set_displays(2, SEG_OFF);
    set_displays(1, SEG_OFF);

  } else {
    set_displays(6, convert_to_segm7(hun_thou_digit));
    set_displays(5, convert_to_segm7(ten_thou_digit));
    set_displays(4, convert_to_segm7(thou_digit));
    set_displays(3, (convert_to_segm7(hun_digit) & 0x7F));
  
    set_displays(2, convert_to_segm7(ten_digit));
    set_displays(1, convert_to_segm7(one_digit));
  }
}

int calculate_speed(int time){
  if(time == 0){
      return 9999999;
  }
  
  int speed;

  switch(units){
    case 1:
      speed =  (distance * 1000)/time;
      break;
    case 2:
      speed = (distance * 3600)/time;
      break;
    case 3:
      speed = (distance * 2237) / time;
      break;
  }

  return speed;
 
}

void start_run(){

}

void main() {
  promt_user();
}

