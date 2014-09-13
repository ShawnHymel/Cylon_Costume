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
#define BUTTON_PIN   2   // Must be hardware interrupt pin
#define CMD_PIN      6   // Pin to send commands to the LED strip
#define NUM_PIXELS   30  // Number of pixels (LEDs) in the strip
#define NUM_CYLON    4   // Num times to woosh (one direction)
#define FRAME_DELAY  40 // Milliseconds between frame updates
#define WHOOSH_DELAY 500 // Milliseconds between wooshes

// Global variables
int8_t whoosh_dir;
uint8_t led_num;
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
  
  // Go to sleep. On button press, wake up and do Cylon thing
  enterSleep();
  
  // Do Cylon thingy
  whoosh_dir = 1;
  for ( int i = 0; i < NUM_CYLON; i++ ) {
    
    // Single woosh
    for ( int j = 0; j < strip.numPixels(); j++ ) {
      
      // Determine main LED to light up
      if ( whoosh_dir == 1 ) {
        led_num = j;
      } else {
        led_num = (strip.numPixels() - 1) - j;
      }
      
      // Light up primary LED
      strip.setPixelColor(led_num, strip.Color(255, 0, 0));
      
      // Light up secondary LEDs
      if ( led_num > 0 ) {
        strip.setPixelColor(led_num - 1, strip.Color(20, 0, 0));
      }
      if ( led_num < (strip.numPixels() - 1) ) {
        strip.setPixelColor(led_num + 1, strip.Color(20, 0, 0));
      }
      
      // Light up tertiary LEDs
      if ( led_num > 1 ) {
        strip.setPixelColor(led_num - 2, strip.Color(5, 0, 0));
      }
      if ( led_num < (strip.numPixels() - 2) ) {
        strip.setPixelColor(led_num + 2, strip.Color(5, 0, 0));
      }
      
      // Display LED and wait 
      strip.show();
      if ( j == (strip.numPixels() - 1) ) {
        delay(WHOOSH_DELAY);
      } else {
        delay(FRAME_DELAY);
      }
      
      // Clear primary LED
      strip.setPixelColor(led_num, strip.Color(0, 0, 0));
      
      // Clear secondary LEDs
      if ( led_num > 0 ) {
        strip.setPixelColor(led_num - 1, strip.Color(0, 0, 0));
      }
      if ( led_num < (strip.numPixels() - 1) ) {
        strip.setPixelColor(led_num + 1, strip.Color(0, 0, 0));
      }
      
      // Clear tertiary LEDs
      if ( led_num > 1 ) {
        strip.setPixelColor(led_num - 2, strip.Color(0, 0, 0));
      }
      if ( led_num < (strip.numPixels() - 2) ) {
        strip.setPixelColor(led_num + 2, strip.Color(0, 0, 0));
      }
      
      // Display (clear) LEDs
      strip.show();
    }
    
    // Change directions
    if ( whoosh_dir == 1 ) {
      whoosh_dir = -1;
    } else {
      whoosh_dir = 1;
    }
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
