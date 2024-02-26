/* 
 * Project Birdsong_MP3_Smart_Plant
 * Author: Laura Green
 * Date: February 26, 2024
 * For comprehensive documentation and examples, please visit:
 * https://docs.particle.io/firmware/best-practices/firmware-template/
 */

// Include Particle Device OS APIs
#include "Particle.h"
#include "IoTClassroom_CNM.h"
#include "DFRobotDFPlayerMini.h"

// Let Device OS manage the connection to the Particle Cloud
SYSTEM_MODE(SEMI_AUTOMATIC);

// Run the application and system concurrently in separate threads
//SYSTEM_THREAD(ENABLED);
DFRobotDFPlayerMini myDFPlayer;//declare mp3 player object


// setup() runs once, when the device is first turned on
void setup() {
  Serial.begin(9600); 
  Serial1.begin(9600);
  waitFor(Serial.isConnected,15000);
  if (!myDFPlayer.begin(Serial1)) {  //Serial1 is name for TX RX
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    delay(10000);
    while(true);
  }
  Serial.println(F("DFPlayer Mini online."));

  //myDFPlayer.randomAll();//these top 2 are in Adam's code, just see if they make it work...YES?!?!No it works anyway....
  //myDFPlayer.pause();
  myDFPlayer.volume(20);  //Set volume value. From 0 to 30
  myDFPlayer.play(1);  //Play the first mp3
  //delay(2000);

}

  


// loop() runs over and over again, as quickly as it can execute.
void loop() {
  // The core of your code will likely live here.

  // Example: Publish event to cloud every 10 seconds. Uncomment the next 3 lines to try it!
  // Log.info("Sending Hello World to the cloud!");
  // Particle.publish("Hello world!");
  // delay( 10 * 1000 ); // milliseconds and blocking - see docs for more info!
}
