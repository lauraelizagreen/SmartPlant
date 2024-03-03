/* 
 * Project Smart_Plant_photo_diode
 * Author: Laura Green
 * Date: March 3, 2024
 * For comprehensive documentation and examples, please visit:
 * https://docs.particle.io/firmware/best-practices/firmware-template/
 */

// Include Particle Device OS APIs
#include "Particle.h"
#include "DFRobotDFPlayerMini.h"
#include "IoTClassroom_CNM.h"

// Let Device OS manage the connection to the Particle Cloud
SYSTEM_MODE(SEMI_AUTOMATIC);//MP3 player only works with semi-automatic?

const int TWINKLEPIN=D4;
const int PHOTODIODEPIN=A5;
const int SONGPIN=D3;
int light;
//declare objects
DFRobotDFPlayerMini myDFPlayer;
Button songButton(SONGPIN);

// Run the application and system concurrently in separate threads
//SYSTEM_THREAD(ENABLED);


void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);//for TXRX (MP3 player)
  waitFor(Serial.isConnected,15000);
  if (!myDFPlayer.begin(Serial1)) {  //Serial1 is name for TX RX
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    delay(10000);
    while(true);
  }
  Serial.println(F("DFPlayer Mini online."));
  myDFPlayer.volume(20);
  pinMode(TWINKLEPIN,OUTPUT);
  pinMode(PHOTODIODEPIN,INPUT);
}

// loop() runs over and over again, as quickly as it can execute.
void loop() {
  light=analogRead(PHOTODIODEPIN);
  if(light<100) {//if night, turn on stars
   digitalWrite(TWINKLEPIN,HIGH);//figure out how to stay high until changes maybe with bool....maybe make checking light level a function....
   myDFPlayer.play(1);//just to see what happens--ok this works, but I don't understand why....
  }
  else {//if day, turn on birdsong, turn off stars
  //myDFPlayer.play(1);//why is this playing when lights on??? only working with track 1.... blue blinking on MP3 when ask for another track....
  digitalWrite(TWINKLEPIN,LOW);
  }
 
  // The core of your code will likely live here.

  // Example: Publish event to cloud every 10 seconds. Uncomment the next 3 lines to try it!
  // Log.info("Sending Hello World to the cloud!");
  // Particle.publish("Hello world!");
  // delay( 10 * 1000 ); // milliseconds and blocking - see docs for more info!
}
