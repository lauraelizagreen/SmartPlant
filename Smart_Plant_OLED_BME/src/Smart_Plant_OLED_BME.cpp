/* 
 * Project Smart_PLant_OLED_BME280
 * Author: Laura Green  
 * Date: March 1, 2024
 * For comprehensive documentation and examples, please visit:
 * https://docs.particle.io/firmware/best-practices/firmware-template/
 */

// Include Particle Device OS APIs
#include "Particle.h"
#include "Adafruit_BME280.h"
#include <IoTTimer.h>

// Let Device OS manage the connection to the Particle Cloud
SYSTEM_MODE(AUTOMATIC);//takes care of connecting to wifi and particle cloud

//const int OLED_RESET=-1;
//const int OLEDADR=0x3C;//address for OLED
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
IoTTimer bmeTimer;



// Run the application and system concurrently in separate threads
//SYSTEM_THREAD(ENABLED);


void setup() {
 Serial.begin(9600);
 waitFor(Serial.isConnected,10000);

 status=bme.begin(BMEADR);
   if(status==FALSE){
    Serial.printf("BME280 at address 0x%02X failed to start",BMEADR);
   }
   bmeStart=millis();

}


void loop() {
  tempC =bme.readTemperature();//deg C
  pressPA =bme.readPressure();//pascals
  humidRH =bme.readHumidity();//%RH

  if((millis()-bmeStart)>BMEDELAY){
    
  //bmeTimer.startTimer(BMEDELAY);
  //if(bmeTimer.isTimerReady()){ //use timer?
   Serial.printf("temp=%0.2fC\npressure=%0.2f pascals\nhumidity=%0.2f%RH\n",tempC,pressPA,humidRH);
   bmeStart=millis(); 
  }

  
}
