// NO WARANTYS THIS CODE MIGHT DESTORY YOUR THINGS..
// USE ONLY IF YOU HAVE A SAFE PLACE TO USE THIS..
// This is only the driver of the heater mossfet NO PID or graphics on the display

// Origin of the basecode is Chris Halsall 
// There are a few challnges with this code and pcb design so please be carful.
// Chat gtp code is where chapt gtp did the code form the old version the PIN ASSIGNMNET IS WRONG FOR V3.0!!

#include <avr/io.h>  // Chat gtp code
#include <avr/eeprom.h> // Chat gtp code
#include <Wire.h> // Chat gtp code
#include <Adafruit_GFX.h> // Chat gtp code
#include <Adafruit_SSD1306.h> // Chat gtp code

//Version Definitions
 static const PROGMEM float hw = 3.0;
 static const PROGMEM float sw = 0.1;

#define SCREEN_WIDTH 128 // Chat gtp code
#define SCREEN_HEIGHT 64 // Chat gtp code
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1); // Chat gtp code

#define MOSFET_PIN PIN3_bm   // CHECK YOUR PINS, PA3 Heater mosfet pin // Chat gtp code
#define BUTTON_PIN_UP PIN7_bm   // CHECK YOUR PINS, PA7 Button 1 // Chat gtp code
#define BUTTON_PIN_DOWN PIN6_BM // CHECK YOUR PINS, 
#define TEMP_SENSOR_PIN 6    // CHECK YOUR PINS, AIN6 ON PCB temp sensor // Chat gtp code
#define VCC_SENSOR_PIN ADC0 // CHECK YOUR PINS. This is for adc to check voltage

//Temperature Info
 byte maxTempArray[] = { 140, 150, 160, 170, 180 };
 byte maxTempIndex = 0;
 byte tempIndexAddr = 1;

//Voltage Measurement Info
 float vConvert = 52.00;
 float vMin = 10.50;

//Solder Reflow Plate Logo
 static const uint8_t PROGMEM logo[] = { 
   0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
   0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 
   0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x1f, 0xc0, 0x00, 0x31, 0x80, 0x00, 0x00, 0x00, 0x00, 
   0x1f, 0xe0, 0x03, 0x01, 0x80, 0x00, 0x00, 0x30, 0x70, 0x00, 0x21, 0x80, 0x00, 0x00, 0x00, 0x00, 
   0x10, 0x20, 0x03, 0x00, 0xc7, 0x80, 0x00, 0x20, 0x18, 0xf0, 0x61, 0x80, 0x00, 0x00, 0x00, 0x00, 
   0x18, 0x00, 0x03, 0x3e, 0xcc, 0xc0, 0xc0, 0x04, 0x19, 0x98, 0x61, 0x80, 0x00, 0x00, 0x00, 0x00, 
   0x1c, 0x01, 0xf3, 0x77, 0xd8, 0xc7, 0xe0, 0x06, 0x33, 0x18, 0x61, 0x8f, 0x88, 0x00, 0x00, 0x00, 
   0x06, 0x03, 0x3b, 0x61, 0xd0, 0xc6, 0x00, 0x07, 0xe2, 0x18, 0x61, 0x98, 0xd8, 0x04, 0x00, 0x00, 
   0x01, 0xc6, 0x0b, 0x60, 0xd9, 0x86, 0x00, 0x06, 0x03, 0x30, 0xff, 0xb0, 0x78, 0x66, 0x00, 0x00, 
   0x40, 0xe4, 0x0f, 0x60, 0xdf, 0x06, 0x00, 0x07, 0x03, 0xe0, 0x31, 0xe0, 0x78, 0x62, 0x00, 0x00, 
   0x40, 0x3c, 0x0f, 0x61, 0xd8, 0x06, 0x00, 0x07, 0x83, 0x00, 0x31, 0xe0, 0x78, 0x63, 0x00, 0x00, 
   0x60, 0x36, 0x1b, 0x63, 0xc8, 0x02, 0x00, 0x02, 0xc1, 0x00, 0x18, 0xb0, 0xcc, 0xe2, 0x00, 0x00, 
   0x30, 0x33, 0x3b, 0x36, 0x4e, 0x03, 0x00, 0x02, 0x61, 0xc0, 0x0c, 0x99, 0xcd, 0xfe, 0x00, 0x00, 
   0x0f, 0xe1, 0xe1, 0x3c, 0x03, 0xf3, 0x00, 0x02, 0x38, 0x7e, 0x0c, 0x8f, 0x07, 0x9c, 0x00, 0x00, 
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
   0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
   0x7f, 0x84, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
   0xc0, 0xe4, 0x00, 0x18, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
   0x04, 0x3c, 0x3c, 0x18, 0x6c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
   0x04, 0x1e, 0x06, 0x7f, 0xc6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
   0x04, 0x3e, 0x03, 0x18, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
   0x04, 0x36, 0x7f, 0x19, 0x8c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
   0x07, 0xe6, 0xc7, 0x19, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
   0x06, 0x07, 0x83, 0x18, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
   0x06, 0x07, 0x81, 0x18, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
   0x06, 0x06, 0xc3, 0x98, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
   0x02, 0x04, 0x7e, 0x08, 0x3f, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
 };
 static const uint8_t logo_width = 128;
 static const uint8_t logo_height = 27;

//Heating Animation
 static const uint8_t PROGMEM heat_animate[] = {
   0b00000001, 0b00000000,
   0b00000001, 0b10000000,
   0b00000001, 0b10000000,
   0b00000001, 0b01000000,
   0b00000010, 0b01000000,
   0b00100010, 0b01000100,
   0b00100100, 0b00100100,
   0b01010101, 0b00100110,
   0b01001001, 0b10010110,
   0b10000010, 0b10001001,
   0b10100100, 0b01000001,
   0b10011000, 0b01010010,
   0b01000100, 0b01100010,
   0b00100011, 0b10000100,
   0b00011000, 0b00011000,
   0b00000111, 0b11100000
 };
 static const uint8_t heat_animate_width = 16;
 static const uint8_t heat_animate_height = 16;

//Tick
 static const uint8_t PROGMEM tick[] = {
   0b00000000, 0b00000100,
   0b00000000, 0b00001010,
   0b00000000, 0b00010101,
   0b00000000, 0b00101010,
   0b00000000, 0b01010100,
   0b00000000, 0b10101000,
   0b00000001, 0b01010000,
   0b00100010, 0b10100000,
   0b01010101, 0b01000000,
   0b10101010, 0b10000000,
   0b01010101, 0b00000000,
   0b00101010, 0b00000000,
   0b00010100, 0b00000000,
   0b00001000, 0b00000000,
   0b01111111, 0b11100000
 };
 static const uint8_t tick_width = 16;
 static const uint8_t tick_height = 15;

void setup() {
    // setup button and Mosfet // Chat gtp code
    PORTA.DIRSET = MOSFET_PIN; // sett Heater Mosfet pin // Chat gtp code
    PORTA.OUTCLR = MOSFET_PIN; // set Heater Mosfet off // Chat gtp code

    PORTA.DIRCLR = BUTTON_PIN_UP; //  pin for buttons input  // Chat gtp code
    PORTA.PIN7CTRL = PORT_PULLUPEN_bm; // Use pull-up resistor // Chat gtp code
    PORTA.DIRCLR = BUTTON_PIN_DOWN; //  pin for buttons input  
    PORTA.PIN7CTRL = PORT_PULLUPEN_bm; // Use pull-up resistor 
       
 //Pull saved values from EEPROM
    maxTempIndex = EEPROM.read(tempIndexAddr) % sizeof(maxTempArray);
  
    // setup  PWM on TCA0 // Chat gtp code
    TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV1_gc | TCA_SINGLE_ENABLE_bm; // Chat gtp code
    TCA0.SINGLE.CTRLB = TCA_SINGLE_WGMODE_DSBOTTOM_gc | TCA_SINGLE_CMP0EN_bm; // Chat gtp code
    TCA0.SINGLE.PER = 255; // 8-bit PWM // Chat gtp code

    // Setup  I2C to OLED-screen // Chat gtp code
    Wire.begin(); // Chat gtp code
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Chat gtp code
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0,0);
    display.drawBitmap(0, 0, logo, logo_width, logo_height, SSD1306_WHITE);
    display.setCursor(80,16);
    display.print(F("S/W V"));
    display.print(sw, 1);
    display.setCursor(80,24);
    display.print(F("H/W V"));
    display.print(hw, 1);
    display.display();
    delay(3000);
    
}

void loop() {
    // Read Button // Chat gtp code
    bool buttonPressed = !(PORTA.IN & BUTTON_PIN);

    if (buttonPressed) {
        PORTA.OUTSET = MOSFET_PIN; // Turn on heater MOSFET // Chat gtp code
    } else {
        PORTA.OUTCLR = MOSFET_PIN; // Turnb off Heater MOSFET // Chat gtp code
    }

    // Read temp from  ADC // Chat gtp code
    uint16_t adcValue;
    ADC0.CTRLA = ADC_ENABLE_bm;
    ADC0.MUXPOS = ADC_MUXPOS_AIN6_gc;
    ADC0.COMMAND = ADC_STCONV_bm;
    while (!(ADC0.INTFLAGS & ADC_RESRDY_bm));
    adcValue = ADC0.RES;
    ADC0.INTFLAGS = ADC_RESRDY_bm;

    // Refresh OLED-display // Chat gtp code
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.print("ADC: ");
    display.print(adcValue);
    display.display();

    delay(100); // Have a short break  // Chat gtp code
}
