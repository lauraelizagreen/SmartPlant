/* 
 * Project Smart_Plant_pump
 * Author: Laura Green  
 * Date: February 23, 2024
 * For comprehensive documentation and examples, please visit:
 * https://docs.particle.io/firmware/best-practices/firmware-template/
 */

// Include Particle Device OS APIs
#include "Particle.h"
#include <Adafruit_MQTT.h>
#include "Adafruit_MQTT/Adafruit_MQTT_SPARK.h"
#include "Adafruit_MQTT/Adafruit_MQTT.h"
#include "credentials.h"

TCPClient TheClient;//global state
//Sets up MQTT client class
Adafruit_MQTT_SPARK mqtt(&TheClient,AIO_SERVER,AIO_SERVERPORT,AIO_USERNAME,AIO_KEY);

//FEEDS
Adafruit_MQTT_Subscribe subFeedPumpButton = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/waterPlant"); 
Adafruit_MQTT_Publish pubFeedSoilMoisture = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/soilMoisture");

const int PUMPPIN=D7;//digital pin to turn pump on/off
const int SOILPIN=A1;//analog pin for soil probe
unsigned int lastTime;//for publishing time
float subValue,pubValue;
int soilMoisture;
bool buttonState;//from Dashboard waterPlant feed


/************Declare Functions*************/
void MQTT_connect();
bool MQTT_ping();

// Let Device OS manage the connection to the Particle Cloud
SYSTEM_MODE(SEMI_AUTOMATIC);

// Run the application and system concurrently in separate threads
//SYSTEM_THREAD(ENABLED);



// setup() runs once, when the device is first turned on
void setup() {
  Serial.begin(9600);
  waitFor(Serial.isConnected,10000);

  // Connect to Internet but not Particle Cloud
  WiFi.on();
  WiFi.connect();
  while(WiFi.connecting()) {
    Serial.printf(".");
  }
  Serial.printf("\n\n");

  // Setup MQTT subscription
  mqtt.subscribe(&subFeedPumpButton);
  pinMode(PUMPPIN,OUTPUT);
  pinMode(SOILPIN,INPUT);
  buttonState=LOW;
}
  


// loop() runs over and over again, as quickly as it can execute.
void loop() {
  MQTT_connect();
  MQTT_ping();
//wait for incoming subscription packets
  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(100))) {//subscription requires void setup to 1. subscribe anytime feed on cloud changes new data-let's you know-in setup
    if (subscription == &subFeedButtonOnOff) {//look at subscription feed for 1/10 sec to see if somethings there associated with feed created
      buttonState = atof((char *)subFeedPumpButton.lastread);//take last read and convert to ASCI from 0s and 1s and convert that to float.(or to integer)
    }
  }
  //manual control of water pump
  if(buttonState==HIGH){
 digitalWrite(PUMPPIN,HIGH);
 Serial.printf("WATERING PLANT\n");
}
else{
  digitalWrite(PUMPPIN,LOW);
}

soilMoisture=analogRead(SOILPIN);
//publishing soil probe date to feed
if((millis()-lastTime > 10000)) {//publishing (how often?)
    if(mqtt.Update()) {
      pubFeedSoilMoisture.publish(soilMoisture);
      Serial.printf("Publishing %i \n",soilMoisture); 
      } 
    lastTime = millis();
  }
}
void MQTT_connect() {//actually connects to server, if not connected stuck in loop
  int8_t ret;//photon 2 thinks of integers as 32bits
 
  // Return if already connected.
  if (mqtt.connected()) {
    return;
  }
 
  Serial.print("Connecting to MQTT... ");
 
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.printf("Error Code %s\n",mqtt.connectErrorString(ret));
       Serial.printf("Retrying MQTT connection in 5 seconds...\n");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds and try again
  }
  Serial.printf("MQTT Connected!\n");
}
///Declare functions

void MQTT_connect() {//actually connects to server, if not connected stuck in loop
  int8_t ret;//photon 2 thinks of integers as 32bits
 
  // Return if already connected.
  if (mqtt.connected()) {
    return;
  }
 
  Serial.print("Connecting to MQTT... ");
 
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.printf("Error Code %s\n",mqtt.connectErrorString(ret));
       Serial.printf("Retrying MQTT connection in 5 seconds...\n");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds and try again
  }
  Serial.printf("MQTT Connected!\n");
}



bool MQTT_ping() {//broker will disconnect if doesn't hear anything, just reminding broker still here so don't disconnect
  static unsigned int last;
  bool pingStatus;

  if ((millis()-last)>120000) {
      Serial.printf("Pinging MQTT \n");
      pingStatus = mqtt.ping();
      if(!pingStatus) {
        Serial.printf("Disconnecting \n");
        mqtt.disconnect();
      }
      last = millis();
  }
  return pingStatus;
}




