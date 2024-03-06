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

TCPClient TheClient;//global state (defines transmission control protocol connection)
//Sets up MQTT client class
Adafruit_MQTT_SPARK mqtt(&TheClient,AIO_SERVER,AIO_SERVERPORT,AIO_USERNAME,AIO_KEY);//object that defines connection

//FEEDS
Adafruit_MQTT_Subscribe subFeedPumpButton = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/waterPlant"); //object for each feed
Adafruit_MQTT_Publish pubFeedSoilMoisture = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/soilMoisture");

//declare variables
const int PUMPPIN=D7;//digital pin to turn pump on/off
const int SOILPIN=A1;//analog pin for soil probe
//unsigned int SOILPUBTIME;//for publishing time for soil sensor feed
unsigned const int WATERTIME=1000;//how long to water after switched to on?
unsigned const int SOILREAD=60000;//how often to read probe
unsigned int pumpTime;
unsigned int soilTime; 
int soilMoisture;//variable for soil data collected (to be published)
int buttonState;//from Dashboard waterPlant feed


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

  ///* Connect to Internet but not Particle Cloud  ***why not cloud??
  WiFi.on();
  WiFi.connect();
  while(WiFi.connecting()) {
    Serial.printf(".");
  }
  Serial.printf("\n\n");
  //*/

  // Setup MQTT subscription
  mqtt.subscribe(&subFeedPumpButton);

  pinMode(PUMPPIN,OUTPUT);
  pinMode(SOILPIN,INPUT);

  buttonState=LOW;//initial button state
  //pumpTime=millis();
  soilTime=millis();
  //pumpTime=millis();
  
}
  


// loop() runs over and over again, as quickly as it can execute.
void loop() {
  ///*
  MQTT_connect();
  MQTT_ping();
//wait for incoming subscription packets
  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(100))) {//subscription requires void setup to 1. subscribe anytime feed on cloud changes new data-let's you know-in setup
    if (subscription == &subFeedPumpButton) {//look at subscription feed for 1/10 sec to see if somethings there associated with feed created
      buttonState = atof((char *)subFeedPumpButton.lastread);//take last read and convert to ASCI from 0s and 1s and convert that to float.(or to integer)
      Serial.printf("recieved buttonstate= %i from adafruit\n",buttonState);
    }
  }
  //manual control of water pump from smart plant dashboard
  if(buttonState==HIGH){//put timer here for watering time
 digitalWrite(PUMPPIN,HIGH);
 Serial.printf("WATERING PLANT REMOTELY\n");
}
else{
  //digitalWrite(PUMPPIN,LOW);


//*/
///*

///*
if(millis()-soilTime>SOILREAD) {
soilMoisture=analogRead(SOILPIN);
Serial.printf("soil moisture is %i\n",soilMoisture);//add publishing here too
if(mqtt.Update()) {
      pubFeedSoilMoisture.publish(soilMoisture);
      Serial.printf("Publishing %i \n",soilMoisture); 
      } 

soilTime=millis();
if(soilMoisture>1500) { 
  pumpTime=millis();//can't set this initially in setup bc could be a long time before soil is dry...
}
}

  while(millis()-pumpTime<WATERTIME) {
    digitalWrite(PUMPPIN,HIGH);
    Serial.printf("Pump ON\n");
     
}
digitalWrite(PUMPPIN,LOW);
Serial.printf("Pump OFF\n");
} 
     
  }
  
//*/
/*timer for publishing soil data? or maybe just publish when read (timed there)
if((millis()-lastTime > SOILPUBGAP)) {//publishing (how often?)
    if(mqtt.Update()) {
      pubFeedSoilMoisture.publish(soilMoisture);
      Serial.printf("Publishing %i \n",soilMoisture); 
      } 
    lastTime = millis();
  }
  */

  

  
  
  /*
//publishing soil probe date to feed
if((millis()-lastTime > 10000)) {//publishing (how often?)
    if(mqtt.Update()) {
      pubFeedSoilMoisture.publish(soilMoisture);
      Serial.printf("Publishing %i \n",soilMoisture); 
      } 
    lastTime = millis();
  }
  
}
*/

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





