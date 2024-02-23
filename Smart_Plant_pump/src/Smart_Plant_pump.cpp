/* 
 * Project Smart_Plant_pump
 * Author: Laura Green  
 * Date: February 23, 2024
 * For comprehensive documentation and examples, please visit:
 * https://docs.particle.io/firmware/best-practices/firmware-template/
 */

// Include Particle Device OS APIs
#include "Particle.h"

const int PUMPPIN=D7;//digital pin to turn pump on/off

// Let Device OS manage the connection to the Particle Cloud
SYSTEM_MODE(AUTOMATIC);

// Run the application and system concurrently in separate threads
SYSTEM_THREAD(ENABLED);

// Show system, cloud connectivity, and application logs over USB
// View logs with CLI using 'particle serial monitor --follow'
SerialLogHandler logHandler(LOG_LEVEL_INFO);

// setup() runs once, when the device is first turned on
void setup() {
  pinMode(PUMPPIN,OUTPUT);
}

// loop() runs over and over again, as quickly as it can execute.
void loop() {
 digitalWrite(PUMPPIN,HIGH);
}
