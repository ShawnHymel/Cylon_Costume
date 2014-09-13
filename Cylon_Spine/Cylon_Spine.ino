/**
 * Cylon Eyes
 *
 * Author: Shawn Hymel
 * Date: September 11, 2014
 * License: As long as you retain this notice you can do whatever
 * you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.
 */

#include <avr/sleep.h>
#include <avr/power.h>
#include <Adafruit_NeoPixel.h>

// Parameters
#define BUTTON_PIN     2   // Must be hardware interrupt pin
#define CMD_PIN        6   // Pin to send commands to strip
#define NUM_PIXELS     59  // Number of pixels (LEDs) in strip
#define BRIGHT_INC     0.4 // Amount to increment brightness
#define BRIGHT_DIV_MAX 5   // Max amount to divide brightness
#define PULSE_DELAY    2   // Milliseconds between pulse frames

// Spine constants. It's hardcoded because I'm lazy.
uint8_t spine[] = {
  10, 10, 12, 14, 16, 18, 20, 30, 40, 50,
  100, 50, 40, 30, 20, 20, 20, 30, 40, 50,
  60, 70, 80, 90, 100, 120, 160, 180, 200,
  250,
  200, 180, 160, 120, 100, 90, 80, 70, 60,
  50, 40, 30, 20, 20, 20, 30, 40, 50, 100,
  50, 40, 30, 20, 18, 16, 14, 12, 10, 10
};

// Global variables
uint8_t red;
uint8_t green;
float bright_div;
uint8_t bright_dir;
int i;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_PIXELS, 
                                            CMD_PIN,
                                            NEO_GRB + NEO_KHZ800);

// Setup
void setup() {
  
  // Initialize button with pullup
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  // Initialize and turn all pixels off
  strip.begin();
  strip.show();
}

// Loop
void loop() {
  
  // Clear spine
  for ( i = 0; i < strip.numPixels(); i++ ) {
    strip.setPixelColor(i, strip.Color(0, 0, 0));
  }
  strip.show();
  
  // Go to sleep. On button press, wake up and do Cylon thing
  enterSleep();
  
  // Wait for button release before starting Cylon thing
  while ( digitalRead(BUTTON_PIN) == 0 ) {
  }
  
  // Light up spine and pulsate
  bright_div = 1;
  bright_dir = 1;
  while(1) {
    
    // Shine bright like a Cylon spine
    for ( i = 0; i < strip.numPixels(); i++ ) {
      red = spine[i] / bright_div;
      green = 0; //red >> 3;
      strip.setPixelColor(i, strip.Color(red, green, 0));
      strip.show();
    } 
    delay(PULSE_DELAY);
    
    // Adjust brightness (for pulsating)
    if ( bright_dir == 1 ) {
      bright_div += BRIGHT_INC;
      if ( bright_div >= BRIGHT_DIV_MAX ) {
        bright_div = BRIGHT_DIV_MAX;
        bright_dir = 0;
      }
    } else {
      bright_div -= BRIGHT_INC;
      if ( bright_div <= 1 ) {
        bright_div = 1;
        bright_dir = 1;
      }
    }
    
    // On button push, stop pulsating
    if ( digitalRead(BUTTON_PIN) == 0 ) {
      break;
    }
  }
  
  // Wait for button release before going back to sleep
  while ( digitalRead(BUTTON_PIN) == 0 ) {
  }
}

// ISR - Wakes from sleep
void pin2Interrupt() {
  
  // Disabling sleep prevents forever sleep
  sleep_disable();
  
  // Disable interrupt
  detachInterrupt(0);
}

// Enter into sleep mode. Sets up another external interrupt.
void enterSleep() {
  
  // Enable sleep and enable interrupt vector
  sleep_enable();
  attachInterrupt(0, pin2Interrupt, LOW);
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  
  // Disable brown-out detector (BOD)
  //cli();
  //sleep_bod_disable();
  //sei();
  
  // Go to sleep
  sleep_cpu();
  
  // Program will continue from here when woken up
  sleep_disable();
}
