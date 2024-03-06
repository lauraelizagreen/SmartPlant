/* 
 * Project Smart_PLant_OLED_BME280
 * Author: Laura Green  
 * Date: March 1, 2024
 * For comprehensive documentation and examples, please visit:
 * https://docs.particle.io/firmware/best-practices/firmware-template/
 */

// Include Particle Device OS APIs
#include "Particle.h"
#include <Adafruit_MQTT.h>
#include "Adafruit_MQTT/Adafruit_MQTT_SPARK.h"
#include "Adafruit_MQTT.h"
#include "credentials.h"
#include "IoTClassroom_CNM.h"//for timer
#include "Adafruit_GFX.h"//include this hfile general graphics
#include "Adafruit_SSD1306.h"//specific device only install this library
#include "Adafruit_BME280.h"
#include <bitmap.h>

TCPClient TheClient;

Adafruit_MQTT_SPARK mqtt(&TheClient,AIO_SERVER,AIO_SERVERPORT,AIO_USERNAME,AIO_KEY);

Adafruit_MQTT_Publish pubFeedTemp = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/temp");
Adafruit_MQTT_Publish pubFeedHumid = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/humid");

SYSTEM_MODE(SEMI_AUTOMATIC);

const int OLED_RESET=-1;
const int ROTATION=1;
const int NUMFLAKES=10;
const int XPOS=0;
const int YPOS=1;
const int DELTAY=2;
const int CAT_HEIGHT=64; //do I need to define bitmap height width? never did before.....for animation? (for the 128x64)
const int CAT_WIDTH =128; 

const int OLEDADR=0x3C;//address for OLED
const int BMEADR=0x76;//address for BME280
const int BMEDELAY=30000;
float tempC;
unsigned int bmeStart;
//float tempF;
//float pressHg;
float pressPA;
float humidRH;
bool status;

//declare objects
Adafruit_BME280 bme;
Adafruit_SSD1306 display(OLED_RESET);
//IoTTimer bmeTimer;//not sure if I'll use this...
//call functions to connect
void MQTT_connect();
bool MQTT_ping();










// Run the application and system concurrently in separate threads
//SYSTEM_THREAD(ENABLED);


void setup() {
 Serial.begin(9600);
 waitFor(Serial.isConnected,10000);
 //connect to internet (in semi-automatic)
  WiFi.on();
  //WiFi.setCredentials("DDCIOT","ddcIOT2020");//forcing it to connect to this address
  WiFi.connect();
  while(WiFi.connecting()) {
    Serial.printf(".");
  }
  Serial.printf("\n\n");

  //OLED initialization   
display.begin(SSD1306_SWITCHCAPVCC, 0x3C); //initialize with 12C address----not sure I understand this....??
void setRotation(uint8_t rotation);//how to use this to flip?
display.clearDisplay();   // clears the screen and buffer
display.clearDisplay();
display.drawBitmap(0,0,CROCHETCAT,128,64,WHITE);
display.display();
delay(5000);
///*


 //delay(500);
 //Wire.begin();//do I need this or is it embedded?
 //delay(5000);

/*USED FOR TROUBLE SHOOTING WHEN MONITOR DIDN"T WORK---LEAVE IN JUST IN CASE
 pinMode(D7,OUTPUT);
digitalWrite(D7,HIGH);
  delay(1000);
  digitalWrite(D7,LOW);
  */

 status=bme.begin(BMEADR);
   if(status==FALSE){
    Serial.printf("BME280 at address 0x%02X failed to start",BMEADR);
   }
   bmeStart=millis();

}


void loop() {
  MQTT_connect();
  MQTT_ping();
  //Serial.printf("test\n");
  tempC =bme.readTemperature();//deg C
  pressPA =bme.readPressure();//pascals
  humidRH =bme.readHumidity();//%RH
  //Serial.printf("read\n");

display.clearDisplay();
display.setTextSize(1);//
display.setTextColor(WHITE);
display.setCursor(0,5);
/*
  digitalWrite(D7,HIGH);
  delay(1000);
  digitalWrite(D7,LOW);
  */

  if((millis()-bmeStart)>BMEDELAY){
    //bmeTimer.startTimer(BMEDELAY);
  //if(bmeTimer.isTimerReady()){ //use timer?
   Serial.printf("temp=%0.2fC\npressure=%0.2f pascals\nhumidity=%0.2fRH\n",tempC,pressPA,humidRH); 
   display.printf("temp=%0.2fC\npressure=%0.2f Pa\nhumidity=%0.2fRH\n",tempC,pressPA,humidRH);
   display.display();

   if(mqtt.Update()) {
    pubFeedTemp.publish(tempC);
    pubFeedHumid.publish(humidRH);
   }
   bmeStart=millis();
  }

  
}
//define functions (maybe put in h file?)
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


