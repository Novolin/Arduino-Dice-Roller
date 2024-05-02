/* This version uses a 4 digit, common cathode, 7 segment display */

#include <LiquidCrystal.h>
#define SRCK 0
#define D0 1
#define E0 2
#define D1 3
#define E1 4
#define D2 5
#define E2 6


#define ROLL_BTN A4
#define CURS_BTN A5


LiquidCrystal lcd(8,9,10,11,12,13);

uint8_t last_dice = 0;
uint8_t roll;
uint8_t selected_dice = 6;
char lineOne[] = " Press To Roll  ";
char lineTwo[] = "Roll D   Last:  ";
char digit_names[][4]= {"D20","100", "D12", "D10", "D6 ", "D4 ", "D2 "};
const uint8_t dice_vals[] = {21,101,13,11,7,5,3};
const uint8_t cursor_locs[] = {0,3,6,9,11,13,15};
bool debounce_roll = true;
bool debounce_curs = true;
// 7 Segment exclusive variables
const uint8_t led_nums[] = {0xFC, 0x60, 0xDA, 0xF2, 0x66, 0xB6, 0xBE, 0xE0, 0xFE, 0xE6};
const uint8_t disp_patterns[] = {4,8,16,32,64,128,256};
const uint8_t enable_pins[] = {E0,E1,E2};
uint8_t current_digit = 0;

long rippleTime;

void writeLED(int data_0, int data_1 = -1, int data_2 = -1, int data_3 = -1){
  // Write the data to the given shift register
    for (int i = 0; i < 9; i++){
        digitalWrite(D0, data_0 & 1);
        data_0 = data_0 >> 1;
        if (data_1 >= 0){
            digitalWrite(D1, data_1 & 1);
            data_1 = data_1 >> 1;
        } 
        if (data_2 >= 0){
            digitalWrite(D2, data_2 & 1);
            data_2 = data_2 >> 1;
        }
        digitalWrite(SRCK, HIGH);
        delay(2); // let it travel down the lines
        digitalWrite(SRCK, LOW);
    }
}




void writeDigits(int num){
    // Outputs the digit to the display
    /* 7 segment version!!! */
    uint8_t ones = num % 10;
    uint8_t tens = num / 10;
    if (tens >= 10 ){
        tens = tens % 10; //ten TEN TEN TEN TEN!!!! TEN TEN !!!!!!!!!!
    } 
    int hundreds = num / 100;

    // Output to each decimal place:
    writeLED(led_nums[ones], led_nums[tens], led_nums[hundreds]);
}


int rollDice(int dval){
	// The roll is one line, 
  	//everything else is about making things pretty
  	last_dice = roll;
  	lcd.setCursor(13,1);
    lcd.print("   ");
    lcd.setCursor(13,1);
    lcd.print(":"+ String(last_dice));
  	roll = random(1,dval);
    rollAnimation(2);
  	writeDigits(roll); 
    
  	return roll;
    
}

void rollAnimation(int duration){
  long target_time = millis() + (duration * 1000);
  uint8_t anim_step = 0;
  while (target_time > millis()){
    anim_step++;
    if (anim_step > 7){
      anim_step = 0;
    }
    writeLED(disp_patterns[anim_step], disp_patterns[anim_step], disp_patterns[anim_step]);
    rippleDisp();
    delay(100);
  }
}

void selectDice(){
  lcd.setCursor(0,1);
  lcd.print(lineTwo);
  selected_dice++;
  if (selected_dice > 5){
    selected_dice = 0;
  }
  lcd.setCursor(5,1);
  lcd.print(digit_names[selected_dice]);
  lcd.setCursor(13,1);
  lcd.print(":"+ String(last_dice));
}

void rippleDisp(){
	digitalWrite(E0,HIGH);
    digitalWrite(E1,HIGH);
    digitalWrite(E2, HIGH);
    current_digit ++;
    if (current_digit > 2){
        current_digit = 0;
    }
	digitalWrite(enable_pins[current_digit], LOW);
}

void setup()
{
	lcd.begin(16,2);
	lcd.print("Loading...");
  	randomSeed(analogRead(A0));
  	pinMode(SRCK, OUTPUT);
  	pinMode(E0,OUTPUT);
  	pinMode(D0,OUTPUT);
    pinMode(E1,OUTPUT);
    pinMode(D1,OUTPUT);
  	pinMode(E2,OUTPUT);
    pinMode(D2,OUTPUT);
	digitalWrite(E0, HIGH);
    digitalWrite(E1, HIGH);
    digitalWrite(E2, HIGH);
  	pinMode(ROLL_BTN, INPUT_PULLUP);
  	pinMode(CURS_BTN, INPUT_PULLUP);
  	rippleTime = millis() + 200;
    lcd.setCursor(0,0);
  	lcd.print(lineOne);
  	lcd.setCursor(0,1);
  	lcd.print(lineTwo);
    selectDice();


}

void loop()
{
    if (!digitalRead(ROLL_BTN)){
        if (debounce_roll){
        rollDice(dice_vals[selected_dice]);
        debounce_roll = false;
        }
    } else {
        debounce_roll = true;
    }
    if (!digitalRead(CURS_BTN)){
        if (debounce_curs){
            selectDice();
            debounce_curs = false;
        }
    } else {
        debounce_curs = true;
    }
  /* 7 Segment ripple code*/
    if (rippleTime < millis()){
        rippleDisp();
        rippleTime += 100;
    }    

  
}