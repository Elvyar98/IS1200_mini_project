/* Code for IS1200 Mini project
 * Authors:
 *  Blomquist, Gustaf
 *  Yarquah, Elvis
 *
 * Created: 2026/10/02
 * Last modified: 2026/10/02
 */
#define SEG_OFF   255
#define SEG_D     161 //d
#define SEG_U     193 
#define SEG_N     200//n 
#define SEG_DASH  191

/*User inputs*/
int distance; //in cm
int units; //km/h, mph, m/s
int runs;

void handle_interrupt(unsigned int cause) {}
void handle_exception(unsigned int cause) {}

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

void main() {
  promt_user();
}

