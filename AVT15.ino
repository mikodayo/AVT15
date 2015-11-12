/* AVTokyo2015
TokyoHackerSpace & @miko_dayo
*/

#include <TimerOne.h>

// common var
// leds
int leds[] = {6, 7, 8, 9, 10, 11, 12, 13};
#define NUMBER_OF_LEDS (sizeof(leds)/sizeof(int))
#define MENU_1_LASOR 1
#define MENU_2_POV   2
#define MENU_3_NOTB  3
// menu
volatile unsigned int menu = 1;
// switch
int SWITCH_SOCKET = 2;
int ACTION_SWITCH_SOCKET = 3;
unsigned long led = 0;
// wait
unsigned long start = 0;
unsigned long now = 0;
unsigned long elapsed = 0;
// pov
unsigned char pos = 0;
unsigned char i = 0;
// binary
unsigned char pos_bin_start = 0;
unsigned char pos_bin = 0;
volatile unsigned int correct = 0;
// score
unsigned long correct_answer = 0;
unsigned long question = 0;
unsigned long score = 0;
// led
#define TIMER_US 100                       // ScanRate (Duration of one LED period) 100mS = uS 100000                        
//unsigned long frames = 0;                 // ++ on every 'frame' of the LED bar
volatile bool LSTATE[8];                  // Desired state for each LED (can use 0 / 1 or LOW / HIGH)
volatile int LDRAW = 0;                   // Which LED are we 'drawing' right now?
//const long interval = 1000;           // interval at which to blink (milliseconds)

void setup() {
  // LED 6,7,8,9,10,11,12,13
  for (led=0; led<NUMBER_OF_LEDS; led++) {
    pinMode(leds[led], OUTPUT);
  }
  // switch
  pinMode(SWITCH_SOCKET,INPUT); // interruput button
  pinMode(ACTION_SWITCH_SOCKET,INPUT); // action button
  // interruption
  attachInterrupt(0, pressed, RISING);
  attachInterrupt(1, pressed_action, RISING);
  // led
  Timer1.initialize(TIMER_US);                  // Initialise timer 1
  Timer1.attachInterrupt( timerIsr );           // attach the ISR routine here
}

// --------------------------
// timerIsr() 1x milli second interrupt ISR()
// Called every time the hardware timer 1 times out.
// --------------------------
void timerIsr()
{
    noInterrupts();                       // temporarilly disable interrupts
    digitalWrite(leds[LDRAW], LOW);   // turn the current LED off
     LDRAW++;                              // move to the next..  
    if (LDRAW >= NUMBER_OF_LEDS){                 // reset to zero if necessary
      LDRAW = 0;
      //frames++;
      LSTATE[7] = LOW;
    }
    digitalWrite( leds[LDRAW], LSTATE[LDRAW]);   // set new current LED state
    interrupts();                         // restart interrupts
}       // get out of the ISR ASAP

void loop(){
  
  switch (menu) {
    case MENU_1_LASOR:// action fine
      // sum score
      if ( correct_answer != 0 && question != 0 ) {
        score = (double)correct_answer / (double)question  * 8;
      }
      for(;;){
        if(menu != 1) break;     
        led_off();
        led_lasor_1();
        // display score = current_answer/question * 8
        led_score(score);       
      }
      break;     
    case MENU_2_POV: //pov
        led_intro();
        myDelay(100);
        
        for(;;) {
          if(menu != 2 ) break;
          pov();
        }
        break;
    case MENU_3_NOTB: // not binary
        led_intro();
        game_notb();
        pos_bin = 0;
        for(;;){
          if(menu != 3 ) break;  
           not_binary();
        }
        break;
    default:
        menu = 1;
        break;
  }

}
// interrput when press the button
void pressed () {
  // change menu value
  if ( digitalRead( SWITCH_SOCKET) == 1 ) {
     menu = menu + 1;
  }
}
void pressed_action(){
  if ( digitalRead( ACTION_SWITCH_SOCKET) == 1 ) {
     correct = 1;
  }
}

// !!notice!! mydelay can not user inside interrput.
void myDelay(unsigned long ms) {              // ms: duration
  // start: timestamp
  start = millis();           
  for (;;) {
    // now: timestamp
    now = millis();
    // elapsed: duration
    elapsed = now - start;
    if (elapsed >= ms)
      return;
  }
}

// led
//-----------------------------
// all off
void led_off() {
  byte bitmap_led_off[] = {
  0b00000000  
  };
  led_flash_by_order_arry(bitmap_led_off, sizeof(bitmap_led_off)/sizeof(byte),400);
}
// led
void led_flash_by_order_arry(byte *array_order, int bitmap_size, unsigned long ms) {

    // first byte big array
    for(i=0; i<bitmap_size; i++){

          start = millis();
          for(;;) {
              now = millis();
            for(led=0;led<NUMBER_OF_LEDS;led++) {      
              LSTATE[led] = (array_order[i]>>led)&0b00000001 ? 1 : 0;
            }
          // check system timer
          elapsed = now - start;
          if (elapsed >= ms)
            break;

          }
      
  }  
}

//-------- Intro
void led_intro(){
led_yes();
led_yes();
}
// lasor
void led_lasor_1(){
  byte bitmap_lasor[] = {
  0b00000001,
  0b00000010,
  0b00000100,
  0b00001000,
  0b00010000,
  0b00100000,
  0b01000000,
  0b10000000,  
  };
  led_flash_by_order_arry(bitmap_lasor, sizeof(bitmap_lasor)/sizeof(byte),70);
}
void led_score(unsigned long score){
  byte bitmap_score_0[] = {
  0b00000000
  };
  byte bitmap_score_1[] = {
  0b00000001
  };
  byte bitmap_score_2[] = {
  0b00000011
  };
  byte bitmap_score_3[] = {
  0b00000111
  };
  byte bitmap_score_4[] = {
  0b00001111
  };
  byte bitmap_score_5[] = {
  0b00011111
  };
  byte bitmap_score_6[] = {
  0b00111111
  };
  byte bitmap_score_7[] = {
  0b01111111
  };
  byte bitmap_score_8[] = {
  0b11111111
  };
  switch (score) {
    case 0:
      led_flash_by_order_arry(bitmap_score_0, sizeof(bitmap_score_0)/sizeof(byte),500);
      break;
    case 1:
      led_flash_by_order_arry(bitmap_score_1, sizeof(bitmap_score_1)/sizeof(byte),500);
      break;
    case 2:
      led_flash_by_order_arry(bitmap_score_2, sizeof(bitmap_score_2)/sizeof(byte),500);
      break;
    case 3:
      led_flash_by_order_arry(bitmap_score_3, sizeof(bitmap_score_3)/sizeof(byte),500);
      break;
    case 4:
      led_flash_by_order_arry(bitmap_score_4, sizeof(bitmap_score_4)/sizeof(byte),500);
      break;
    case 5:
      led_flash_by_order_arry(bitmap_score_5, sizeof(bitmap_score_5)/sizeof(byte),500);
      break;
    case 6:
      led_flash_by_order_arry(bitmap_score_6, sizeof(bitmap_score_6)/sizeof(byte),500);
      break;
    case 7:
      led_flash_by_order_arry(bitmap_score_7, sizeof(bitmap_score_7)/sizeof(byte),500);
      break;
    case 8:
      led_flash_by_order_arry(bitmap_score_8, sizeof(bitmap_score_8)/sizeof(byte),500);
      break;
    default :
      break;
  }
}
//-------- condition
// Yes
void led_yes(){
  byte bitmap_led_yes[] = {
  0b10000001,
  0b11000011,
  0b11100111,
  0b11111111
  };
  led_flash_by_order_arry(bitmap_led_yes, sizeof(bitmap_led_yes)/sizeof(byte),100);
}

void led_no(){
  byte bitmap_led_no[] = {
  0b10000000,
  0b11000000,
  0b11100000,
  0b11110000,
  0b11111000,
  0b11111100,
  0b11111110,
  0b11111111,
  0b11111110,
  0b11111100,
  0b11111000,
  0b11110000,
  0b11100000,
  0b11000000,
  0b10000000,
  0b11000000,
  0b11100000,
  0b11110000,
  0b11111000,
  0b11111100,
  0b11111110,
  0b11111111
  };
  led_flash_by_order_arry(bitmap_led_no, sizeof(bitmap_led_no)/sizeof(byte),100);
}

//-------- game
void pov(){
  // pov
  byte bitmap_pov[] = {
    0b11111000,
    0b00010110,
    0b00010001,
    0b00010110,
    0b11111000,
    0b00000000,
    0b00011111,
    0b01100000,
    0b10000000,
    0b01100000,
    0b00011111,
    0b00000000,
    0b00000001,
    0b00000001,
    0b11111111,
    0b00000001,
    0b00000001
  };
  led_flash_by_order_arry(bitmap_pov, sizeof(bitmap_pov)/sizeof(byte),5);
}
// Not B begining
void game_notb(){
  // not begin
  byte bitmap_bin_start[] =  {
  0b00000001,
  0b00000010,
  0b00000011,
  0b00000100,
  0b00000111,
  0b00000000,
  0b00000111,
  0b00000000,
  0b00000111
  };
  
  led_flash_by_order_arry(bitmap_bin_start, sizeof(bitmap_bin_start)/sizeof(byte),500);
  led_no();
  led_off();
}
// not b
void not_binary(){
  
byte error_binary_pos[] = {5,10,18,25,35,48,55,63,75,83,95,103,117,122,133,149,154,161,170,183,195,203,214,225,239,248,251};
byte error_binary[] = {
  0b00000001,
  0b00000010,
  0b00000011,
  0b11111111,
  0b00000101,
  0b00000110,
  0b00000111,
  0b00001000,
  0b00001111,
  0b00001010,
  0b00001011,
  0b00001100,
  0b00001101,
  0b00001110,
  0b00001111,
  0b00010000,
  0b00110001,
  0b00010010,
  0b00010011,
  0b00010100,
  0b00010101,
  0b00010110,
  0b00010111,
  0b10011000,
  0b00011001,
  0b00011010,
  0b00011011,
  0b00011100,
  0b00011101,
  0b00011110,
  0b00011111,
  0b00100000,
  0b00100001,
  0b00110011,
  0b00100011,
  0b00100100,
  0b00100101,
  0b00100110,
  0b00100111,
  0b00101000,
  0b00101001,
  0b00101010,
  0b00101011,
  0b00101100,
  0b00101101,
  0b00101110,
  0b01101111,
  0b00110000,
  0b00110001,
  0b00110010,
  0b00110011,
  0b00110100,
  0b00110101,
  0b00111110,
  0b00110111,
  0b00111000,
  0b00111001,
  0b00111010,
  0b00111011,
  0b00111100,
  0b00111101,
  0b10111110,
  0b00111111,
  0b01000000,
  0b01000001,
  0b01000010,
  0b01000011,
  0b01000100,
  0b01000101,
  0b01000110,
  0b01000111,
  0b01001000,
  0b01001001,
  0b01011011,
  0b01001011,
  0b01001100,
  0b01001101,
  0b01001110,
  0b01001111,
  0b01010000,
  0b01010001,
  0b01010111,
  0b01010011,
  0b01010100,
  0b01010101,
  0b01010110,
  0b01010111,
  0b01011000,
  0b01011001,
  0b01011010,
  0b01011011,
  0b01011100,
  0b01011101,
  0b01010110,
  0b01011111,
  0b01100000,
  0b01100001,
  0b01100010,
  0b01100011,
  0b01100100,
  0b01100101,
  0b11100100,
  0b01100111,
  0b01101000,
  0b01101001,
  0b01101010,
  0b01101011,
  0b01101100,
  0b01101101,
  0b01101110,
  0b01101111,
  0b01110000,
  0b01110001,
  0b01110010,
  0b01110011,
  0b01010100,
  0b01110101,
  0b01110110,
  0b01110111,
  0b01111000,
  0b00011001,
  0b01111010,
  0b01111011,
  0b01111100,
  0b01111101,
  0b01111110,
  0b01111111,
  0b10000000,
  0b10000001,
  0b10000010,
  0b10000011,
  0b11100110,
  0b10000101,
  0b10000110,
  0b10000111,
  0b10001000,
  0b10001001,
  0b10001010,
  0b10001011,
  0b10001100,
  0b10001101,
  0b10001110,
  0b10001111,
  0b10010000,
  0b10010001,
  0b10010010,
  0b10010011,
  0b00010100,
  0b10010101,
  0b10010110,
  0b10010111,
  0b10011000,
  0b00010000,
  0b10011010,
  0b10011011,
  0b10011100,
  0b10011101,
  0b10011110,
  0b10011111,
  0b10100100,
  0b10100001,
  0b10100010,
  0b10100011,
  0b10100100,
  0b10100101,
  0b10100110,
  0b10100111,
  0b10101000,
  0b11111011,
  0b10101010,
  0b10101011,
  0b10101100,
  0b10101101,
  0b10101110,
  0b10101111,
  0b10110000,
  0b10110001,
  0b10110010,
  0b10110011,
  0b10110100,
  0b10110101,
  0b10110111,
  0b10110111,
  0b10111000,
  0b10111001,
  0b10111010,
  0b10111011,
  0b10111100,
  0b10111101,
  0b10111110,
  0b10111111,
  0b11000000,
  0b11000001,
  0b11010010,
  0b11000011,
  0b11000100,
  0b11000101,
  0b11000110,
  0b11000111,
  0b11001000,
  0b11001001,
  0b11001011,
  0b11001011,
  0b11001100,
  0b11001101,
  0b11001110,
  0b11001111,
  0b11010000,
  0b11010001,
  0b11010010,
  0b11010011,
  0b11010100,
  0b11011111,
  0b11010110,
  0b11010111,
  0b11011000,
  0b11011001,
  0b11011010,
  0b11011011,
  0b11011100,
  0b11011101,
  0b11011110,
  0b11011111,
  0b11100001,
  0b11100001,
  0b11100010,
  0b11100011,
  0b11100100,
  0b11100101,
  0b11100110,
  0b11100111,
  0b11101000,
  0b11101001,
  0b11101010,
  0b11101011,
  0b11101100,
  0b11101101,
  0b11111110,
  0b11101111,
  0b11110000,
  0b11110001,
  0b11110010,
  0b11110011,
  0b11110100,
  0b11110101,
  0b11110110,
  0b11110101,
  0b11111000,
  0b11111001,
  0b11110010,
  0b11111011,
  0b11111100,
  0b11111101,
  0b11111110,
  0b11111111
};
byte bitmap_bin[] =  {
  0b00000001,
  0b00000010,
  0b00000011,
  0b00000100,
  0b00000101,
  0b00000110,
  0b00000111,
  0b00001000,
  0b00001001,
  0b00001010,
  0b00001011,
  0b00001100,
  0b00001101,
  0b00001110,
  0b00001111,
  0b00010000,
  0b00010001,
  0b00010010,
  0b00010011,
  0b00010100,
  0b00010101,
  0b00010110,
  0b00010111,
  0b00011000,
  0b00011001,
  0b00011010,
  0b00011011,
  0b00011100,
  0b00011101,
  0b00011110,
  0b00011111,
  0b00100000,
  0b00100001,
  0b00100010,
  0b00100011,
  0b00100100,
  0b00100101,
  0b00100110,
  0b00100111,
  0b00101000,
  0b00101001,
  0b00101010,
  0b00101011,
  0b00101100,
  0b00101101,
  0b00101110,
  0b00101111,
  0b00110000,
  0b00110001,
  0b00110010,
  0b00110011,
  0b00110100,
  0b00110101,
  0b00110110,
  0b00110111,
  0b00111000,
  0b00111001,
  0b00111010,
  0b00111011,
  0b00111100,
  0b00111101,
  0b00111110,
  0b00111111,
  0b01000000,
  0b01000001,
  0b01000010,
  0b01000011,
  0b01000100,
  0b01000101,
  0b01000110,
  0b01000111,
  0b01001000,
  0b01001001,
  0b01001010,
  0b01001011,
  0b01001100,
  0b01001101,
  0b01001110,
  0b01001111,
  0b01010000,
  0b01010001,
  0b01010010,
  0b01010011,
  0b01010100,
  0b01010101,
  0b01010110,
  0b01010111,
  0b01011000,
  0b01011001,
  0b01011010,
  0b01011011,
  0b01011100,
  0b01011101,
  0b01011110,
  0b01011111,
  0b01100000,
  0b01100001,
  0b01100010,
  0b01100011,
  0b01100100,
  0b01100101,
  0b01100110,
  0b01100111,
  0b01101000,
  0b01101001,
  0b01101010,
  0b01101011,
  0b01101100,
  0b01101101,
  0b01101110,
  0b01101111,
  0b01110000,
  0b01110001,
  0b01110010,
  0b01110011,
  0b01110100,
  0b01110101,
  0b01110110,
  0b01110111,
  0b01111000,
  0b01111001,
  0b01111010,
  0b01111011,
  0b01111100,
  0b01111101,
  0b01111110,
  0b01111111,
  0b10000000,
  0b10000001,
  0b10000010,
  0b10000011,
  0b10000100,
  0b10000101,
  0b10000110,
  0b10000111,
  0b10001000,
  0b10001001,
  0b10001010,
  0b10001011,
  0b10001100,
  0b10001101,
  0b10001110,
  0b10001111,
  0b10010000,
  0b10010001,
  0b10010010,
  0b10010011,
  0b10010100,
  0b10010101,
  0b10010110,
  0b10010111,
  0b10011000,
  0b10011001,
  0b10011010,
  0b10011011,
  0b10011100,
  0b10011101,
  0b10011110,
  0b10011111,
  0b10100000,
  0b10100001,
  0b10100010,
  0b10100011,
  0b10100100,
  0b10100101,
  0b10100110,
  0b10100111,
  0b10101000,
  0b10101001,
  0b10101010,
  0b10101011,
  0b10101100,
  0b10101101,
  0b10101110,
  0b10101111,
  0b10110000,
  0b10110001,
  0b10110010,
  0b10110011,
  0b10110100,
  0b10110101,
  0b10110110,
  0b10110111,
  0b10111000,
  0b10111001,
  0b10111010,
  0b10111011,
  0b10111100,
  0b10111101,
  0b10111110,
  0b10111111,
  0b11000000,
  0b11000001,
  0b11000010,
  0b11000011,
  0b11000100,
  0b11000101,
  0b11000110,
  0b11000111,
  0b11001000,
  0b11001001,
  0b11001010,
  0b11001011,
  0b11001100,
  0b11001101,
  0b11001110,
  0b11001111,
  0b11010000,
  0b11010001,
  0b11010010,
  0b11010011,
  0b11010100,
  0b11010101,
  0b11010110,
  0b11010111,
  0b11011000,
  0b11011001,
  0b11011010,
  0b11011011,
  0b11011100,
  0b11011101,
  0b11011110,
  0b11011111,
  0b11100000,
  0b11100001,
  0b11100010,
  0b11100011,
  0b11100100,
  0b11100101,
  0b11100110,
  0b11100111,
  0b11101000,
  0b11101001,
  0b11101010,
  0b11101011,
  0b11101100,
  0b11101101,
  0b11101110,
  0b11101111,
  0b11110000,
  0b11110001,
  0b11110010,
  0b11110011,
  0b11110100,
  0b11110101,
  0b11110110,
  0b11110111,
  0b11111000,
  0b11111001,
  0b11111010,
  0b11111011,
  0b11111100,
  0b11111101,
  0b11111110,
  0b11111111
};  
  
  // errory binary
  for(i=0; i<sizeof(error_binary_pos)/sizeof(byte); i++){
    if ( pos_bin == error_binary_pos[i] ) {
      bitmap_bin[pos_bin] = error_binary[i];
      ++question;
    }
  }
  correct = 0;
  start = millis();
  for(;;){
    now = millis();
    for(led=0;led<NUMBER_OF_LEDS;led++) {
      LSTATE[led] = (bitmap_bin[pos_bin]>>led)&0b00000001 ? 1 : 0;
    }
    // check system timer
    elapsed = now - start;
    if (elapsed >= 1000)
      break;
  }

  for(i=0; i<sizeof(error_binary_pos)/sizeof(byte); i++){
    if ( pos_bin == error_binary_pos[i] ) {
      if ( correct == 1){
        led_yes();led_off();
        ++correct_answer; 
      } else {
        led_no(); led_off();
      }
      break;
    }
  }
  if(pos_bin++ == sizeof(bitmap_bin)/sizeof(byte)) pos_bin = 0;    

}

