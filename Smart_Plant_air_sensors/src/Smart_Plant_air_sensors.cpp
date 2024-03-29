/* 
 * Project Smart_Plant_air_sensors
 * Author: Laura Green
 * Date: March 1, 2024
 * For comprehensive documentation and examples, please visit:
 * https://docs.particle.io/firmware/best-practices/firmware-template/
 */

// Include Particle Device OS APIs
#include "Particle.h"
#include "math.h"
#include <Adafruit_MQTT.h>
#include "Adafruit_MQTT/Adafruit_MQTT_SPARK.h"
#include "Adafruit_MQTT.h"
#include "credentials.h"//credentials for connectiong to MQTT
#include "Air_Quality_Sensor.h"

TCPClient TheClient;

Adafruit_MQTT_SPARK mqtt(&TheClient,AIO_SERVER,AIO_SERVERPORT,AIO_USERNAME,AIO_KEY);

Adafruit_MQTT_Publish pubFeedDustSensor = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/dustSensor");
Adafruit_MQTT_Publish pubFeedAirQuality = Adafruit_MQTT_Publish(&mqtt,AIO_USERNAME "/feeds/airQuality");

const int DUSTPIN=D2;// Dust sensor needs 5V? or use D2
const float DUSTSAMPTIME=30000;//30 second time to sample
const int AIRQUALPIN=A0;//analog pin for air quality sensor
int currentQual;//not sure I need this
int slope;//quantitative measure
int sensorValue;//current voltage
unsigned int duration, dustStartTime,lastTimePubDust; //for publishing timing
unsigned int lowPulseOccupancy;
float ratio;
float concentration;

AirQualitySensor airQualSensor(AIRQUALPIN);//declare object for sensor

//call functions to connect
void MQTT_connect();
bool MQTT_ping();


// Let Device OS manage the connection to the Particle Cloud
SYSTEM_MODE(SEMI_AUTOMATIC);

// Run the application and system concurrently in separate threads
//SYSTEM_THREAD(ENABLED);




void setup() {
  Serial.begin(9600);
  waitFor(Serial.isConnected,10000);

 ///* Connect to Internet
  WiFi.on();
  WiFi.connect();
  while(WiFi.connecting()) {
    Serial.printf(".");
  }
  Serial.printf("\n\n");
//*/

Serial.printf("waiting for sensor to initialize");
delay(20000);

if(airQualSensor.init()) {//is initial voltage btw 10 and 798?
  Serial.printf("sensor ready.");
}
else {
  Serial.printf("sensor ERROR!");
}
currentQual=1;//set initial air quality value

  pinMode(DUSTPIN,INPUT);
  //pinMode(AIRQUALPIN,INPUT);
  dustStartTime=millis();//for current time
  lowPulseOccupancy=0;//start at 0

}
  


void loop() {
  MQTT_connect();
  MQTT_ping();

  slope=airQualSensor.slope();
  sensorValue=airQualSensor.getValue();

  duration=pulseIn(DUSTPIN,LOW);
  lowPulseOccupancy=lowPulseOccupancy + duration;
  

  if((millis()-dustStartTime)>DUSTSAMPTIME){
  ratio=lowPulseOccupancy/(DUSTSAMPTIME*10);//for percent of time the signal is low
  concentration=1.1*pow(ratio,3)-3.8*pow(ratio,2)+ratio+520*ratio+0.62;//power function -so just description of relationship curve btw ratio and concentration
  Serial.printf("low pulse occupancy=%i\nratio=%f\nconcentration=%f\n",lowPulseOccupancy,ratio,concentration);
  Serial.printf("air quality sensor slope is %i\nair quality sensor value is %i\n",slope,sensorValue);

  if(mqtt.Update()) {
   pubFeedDustSensor.publish(concentration);
   pubFeedAirQuality.publish(slope);
  Serial.printf("Publishing dust concentration=%f \n",concentration);
  Serial.printf("Publishing air quality =%i \n",slope); 
  }

  lowPulseOccupancy=0;
  dustStartTime=millis();

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

