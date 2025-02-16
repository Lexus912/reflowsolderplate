#include <avr/io.h>
#include <avr/eeprom.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#define MOSFET_PIN PIN3_bm   // Exempel, PA3
#define BUTTON_PIN PIN7_bm   // Exempel, PA7
#define TEMP_SENSOR_PIN 6    // Exempel, AIN6

void setup() {
    // Initiera MOSFET och knappen
    PORTA.DIRSET = MOSFET_PIN; // Sätt MOSFET som output
    PORTA.OUTCLR = MOSFET_PIN; // Börja med MOSFET avstängd

    PORTA.DIRCLR = BUTTON_PIN; // Sätt knappen som input
    PORTA.PIN7CTRL = PORT_PULLUPEN_bm; // Aktivera pull-up resistor

    // Initiera PWM på TCA0
    TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV1_gc | TCA_SINGLE_ENABLE_bm;
    TCA0.SINGLE.CTRLB = TCA_SINGLE_WGMODE_DSBOTTOM_gc | TCA_SINGLE_CMP0EN_bm;
    TCA0.SINGLE.PER = 255; // 8-bit PWM

    // Initiera I2C för OLED-skärmen
    Wire.begin();
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    display.clearDisplay();
    display.display();
}

void loop() {
    // Läs knappstatus
    bool buttonPressed = !(PORTA.IN & BUTTON_PIN);

    if (buttonPressed) {
        PORTA.OUTSET = MOSFET_PIN; // Slå på MOSFET
    } else {
        PORTA.OUTCLR = MOSFET_PIN; // Stäng av MOSFET
    }

    // Läs temperatur från ADC
    uint16_t adcValue;
    ADC0.CTRLA = ADC_ENABLE_bm;
    ADC0.MUXPOS = ADC_MUXPOS_AIN6_gc;
    ADC0.COMMAND = ADC_STCONV_bm;
    while (!(ADC0.INTFLAGS & ADC_RESRDY_bm));
    adcValue = ADC0.RES;
    ADC0.INTFLAGS = ADC_RESRDY_bm;

    // Uppdatera OLED-display
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.print("ADC: ");
    display.print(adcValue);
    display.display();

    delay(100); // Liten fördröjning
}
