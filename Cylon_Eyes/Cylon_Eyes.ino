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

// Global variables
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
  
  // Light up LEDs one at a time
  for ( int i = 0; i < strip.numPixels(); i++ ) {
    strip.setPixelColor(i, strip.Color(255, 0, 0));
    strip.show();
    delay(50);
    strip.setPixelColor(i, strip.Color(0, 0, 0));
    strip.show();
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
