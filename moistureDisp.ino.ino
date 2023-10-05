#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//Thanks to Adafruit :)

//PIEZO
const int buzzer = 9; //buzzer to arduino pin 9

//DISP
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//SOILMOIST
const int AirValue = 715;   //you need to replace this value with Value_1
const int WaterValue = 390;  //you need to replace this value with Value_2
int soilMoistureValue = 0;
int soilmoisturepercent=0;

#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16

static const unsigned char PROGMEM bar_bmp[] =
{ 0b10101010, 0b01010101,
  0b10000000, 0b00000001,
  0b10000000, 0b00000001,
  0b10000000, 0b00000001,
  0b10000000, 0b00000001,
  0b10000000, 0b10000001,
  0b10000000, 0b10000001,
  0b10000001, 0b11000001,
  0b10000011, 0b11100001,
  0b10000111, 0b11110001,
  0b10000111, 0b11110001,
  0b10000111, 0b11110001, 
  0b10000011, 0b11100001,
  0b10000001, 0b11000001,
  0b10000000, 0b00000001,
  0b10000000, 0b00000001,};

static const unsigned char PROGMEM toomuch_bmp[] =
{ 0b10101010, 0b01010101,
  0b10000000, 0b00000001,
  0b10000000, 0b00000001,
  0b10000000, 0b00000001,
  0b10000000, 0b00000001,
  0b10000001, 0b10000001,
  0b10000011, 0b11000001,
  0b10000111, 0b11100001,
  0b10001111, 0b11110001,
  0b10011001, 0b10011001,
  0b10011011, 0b11011001,
  0b10011110, 0b01111001, 
  0b10001001, 0b10010001,
  0b10000111, 0b11100001,
  0b10000000, 0b00000001,
  0b10000000, 0b00000001,};

void setup() {
  Serial.begin(9600);

  pinMode(buzzer, OUTPUT); // Set buzzer - pin 9 as an output

  tone(buzzer, 1000);
  delay(100);
  tone(buzzer, 2000);
  delay(100);
  tone(buzzer, 2500);
  delay(100);
  tone(buzzer, 2000);
  delay(500);
  tone(buzzer, 800);
  noTone(buzzer);     // Stop sound...

  addDisplay();
  // Clear the buffer
  display.clearDisplay();
  display.setTextColor(WHITE);

  display.setTextSize(2);
  display.setCursor(0, 0);
  display.println("Wet");
  display.println("or");
  display.println("not :O");
  delay(2000); // Pause for 2 seconds

  // Show the display buffer on the screen. You MUST call display() after
  // drawing commands to make them visible on screen!
  display.display();
  delay(2000);

  // Invert and restore display, pausing in-between
  display.invertDisplay(true);
  delay(1000);
  display.invertDisplay(false);
  delay(1000);

  looper(bar_bmp, toomuch_bmp, LOGO_WIDTH, LOGO_HEIGHT);
}

void loop(){

}

void addDisplay(){
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  delay(2000);
}

int xpos = 0;
void looper(const uint8_t *bitmap, const uint8_t *toomuch, uint8_t w, uint8_t h){
  for(;;) { // Loop forever...
    
    soilMoistureValue = analogRead(A0);  //put Sensor insert into soil
    soilmoisturepercent = map(soilMoistureValue, AirValue, WaterValue, 0, 100);

    if(xpos+w<display.width()){
      xpos++;
    }else{
      xpos = 0;
    }

    display.clearDisplay(); // Clear the display buffer

    if(soilmoisturepercent<84){
        tone(buzzer, 1000); // Send 1KHz sound signal...
        delay(100);        // ...for 1 sec
        noTone(buzzer);     // Stop sound...
        delay(1000);        // ...for 1sec
    }

    if(soilmoisturepercent>=85){
      display.drawBitmap(xpos, 0, bitmap, w, h, SSD1306_WHITE);
    }
    if(soilmoisturepercent>90){
    display.drawBitmap(xpos, h, bitmap, w, h, SSD1306_WHITE);
    }
    if(soilmoisturepercent>110){
    display.drawBitmap(xpos, h*2, bitmap, w, h, SSD1306_WHITE);
    }
    if(soilmoisturepercent>130){ //163 on vesilasissa
      if(soilmoisturepercent>160){
        display.drawBitmap(xpos, h*3, toomuch, w, h, SSD1306_WHITE);
      }else{
        display.drawBitmap(xpos, h*3, bitmap, w, h, SSD1306_WHITE);
      }
    }
    
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println(String(soilmoisturepercent));

    display.display(); // Show the display buffer on the screen
    delay(200);        // Pause 
  }
}
