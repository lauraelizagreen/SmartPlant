/* 
 * Project Smart_PLant_OLED_BME280
 * Author: Laura Green  
 * Date: March 1, 2024
 * For comprehensive documentation and examples, please visit:
 * https://docs.particle.io/firmware/best-practices/firmware-template/
 */

// Include Particle Device OS APIs
#include "Particle.h"
//#include "IoTClassroom_CNM.h"//for timer
#include "Adafruit_GFX.h"//include this hfile general graphics
#include "Adafruit_SSD1306.h"//specific device only install this library
#include "Adafruit_BME280.h"
#include <bitmap.h>


// Let Device OS manage the connection to the Particle Cloud
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
const int BMEDELAY=1000;
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
///*

//*/






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
   bmeStart=millis(); 
   display.printf("temp=%0.2fC\npressure=%0.2f Pa\nhumidity=%0.2fRH\n",tempC,pressPA,humidRH);
   display.display();
  }

  
}
