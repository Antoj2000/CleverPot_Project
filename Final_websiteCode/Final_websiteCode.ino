
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include "homepage.h"
#include <DFRobot_DHT11.h>

DFRobot_DHT11 DHT;

#define DHT11_PIN 4
#define depth 100

#define MOTOR 23 

//Variables for loop functions to run simultaneously 

unsigned long previousLEDMillis = 0;
unsigned long previousPumpMillis = 0;
unsigned long previousCheckMillis = 0;
unsigned long previousSensorMillis = 0;
unsigned long previousWebsiteMillis = 0;

const unsigned long ledInterval = 250; // Interval for LED flashing in milliseconds
const unsigned long pumpInterval = 15000; // Interval for pump operation in milliseconds
const unsigned long checkInterval = 1000; // Interval for checking water sensor values in milliseconds
unsigned long previousPrintMillis = 0;
const unsigned long printInterval = 5000; // Interval for printing sensor readings in milliseconds


//Ultrasonic variables
const int LED_PIN = 13;   //LED
const int TRIG_PIN = 5;    //Trigger
const int ECHO_PIN = 18;   //Echo
long duration;    //Used to measure time from transmission until echo returns 
float cms;
int percentage = 0;

int lowPercentage = 20;
const float minDistance = 2; // distance from sensor to the surface of the water when tank is full
const float maxDistance = 6.74; // distance from sensor to the bottom of the tank

//DHT variables
int lastTemp = 0; 
int lastHumidity = 0;

//Soil moisture variables 
const int SOIL_PIN = 36;
int sensorValue = 0;
int value = 0;
int moistPercentage = 0;

//LDR Variables
const int ldrPin = 32; 
const int ledPin = 33;
int lightValue = 0;
//WIFI setup
const char* ssid = "Anto";
const char* password = "Hotspot123";

WebServer server(80);

// reads all sensors on the clever pot 
void readSensor() {
  //DHT Code 
  DHT.read(DHT11_PIN);
  lastTemp = DHT.temperature;
  lastHumidity = DHT.humidity;

   //Define inputs and outputs for ultrasonic 
  pinMode(LED_PIN, OUTPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Trigger the ultrasonic sensor 
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(5);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Read the signal from the sensor: a HIGH pulse whose duration
  // is the time (in microseconds) from the sending of the 
  // ping to the reception of its echo off of an object.
  duration = pulseIn(ECHO_PIN, HIGH);

  //Convert the time into centimetres 
  cms = (duration/2) / 29.1;

  //Map the distance to a percentage
  percentage = max(0,static_cast<int>(((maxDistance - cms) / (maxDistance - minDistance)) * 100));

  // Soil moisture reading 
  sensorValue = analogRead(SOIL_PIN);
  value = sensorValue/2.32;

  moistPercentage = map(value, 0, 1023, 0 , 100);

  //LDR sensor setup
  pinMode(ldrPin, INPUT);
  pinMode(ledPin, OUTPUT);

  lightValue = analogRead(ldrPin);


}

// turns moist percentage into a string
String getMoist(){
  return String(moistPercentage);
}

// turns water percentage into a string
String getWater(){
  return String(percentage);
}
// turns temp reading into a string
String getTemp() {
  return String(lastTemp);
}
// turns humidity reading into a string
 String getHumidity() {
   return String(lastHumidity);
 }

void handleRoot() {
  String message = homePagePart1 + getTemp() + homePagePart2 
  + getHumidity() + homePagePart3 + getWater() + homePagePart4
  + getMoist() + homePagePart5;

  if (server.method() == HTTP_POST && server.uri() == "/activatePump") {
    // Activate the pump
    digitalWrite(MOTOR, HIGH);
    delay(10000); // Run the pump for 1 second (adjust as needed)
    digitalWrite(MOTOR, LOW); // Turn off the pump
    message += "Pump activated";
  }

  server.send(200, "text/html", message);
}

void handleActivatePump() {
  digitalWrite(MOTOR, HIGH); // Activate the pump
  delay(10000); // Run the pump for 1 second (adjust as needed)
  digitalWrite(MOTOR, LOW); // Turn off the pump
  
  // Respond to the request with a redirection to the root page
  server.sendHeader("Location", "/", true);
  server.send(302, "text/plain", "Pump activated. Redirecting...");
}


void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}


void setup(void) {

  //Initialises WiFi code 
  Serial.begin(115200);           
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp32")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);
  server.on("/activatePump", HTTP_POST, handleActivatePump); // Register the handler for /activatePump POST requests
  server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");

  pinMode(MOTOR, OUTPUT);
}

void loop(void) {
  server.handleClient();
  delay(2);        //allow cpu to switch to other tasks 

  unsigned long currentMillis = millis();

  if (currentMillis - previousSensorMillis >= 2000) {
    previousSensorMillis = currentMillis;
    readSensor(); // Read sensor values
  }
  
   if (currentMillis - previousWebsiteMillis >= 5000) {
    previousWebsiteMillis = currentMillis;
    handleRoot(); // Update webpage with latest readings
  }
  

  // Check water percentage and flash LED if low
  if (currentMillis - previousCheckMillis >= checkInterval) {
    previousCheckMillis = currentMillis;
    
   if (percentage < 20) {
      digitalWrite(LED_PIN, !digitalRead(LED_PIN)); // Toggle LED state
      //Serial.println("Water level too low. Please refill your tank");
    } else {
      digitalWrite(LED_PIN, LOW);
    }
  }

  // Check moisture level and turn on pump if low
  if (currentMillis - previousPumpMillis >= pumpInterval) {
    previousPumpMillis = currentMillis;

    if (moistPercentage > 40) {
      digitalWrite(MOTOR, HIGH);
      Serial.println("Top speed");
      // Pump will run for a specified interval
    } else {
      digitalWrite(MOTOR, LOW);
      Serial.println("off");
    }
  }

  // Check light level and control LED accordingly
  if (currentMillis - previousLEDMillis >= ledInterval) {
    previousLEDMillis = currentMillis;

    if (lightValue < 500) {
      digitalWrite(ledPin, HIGH); // Turn on the LED
    } else {
      digitalWrite(ledPin, LOW); // Turn off the LED
    }
  }

  // Print readings on serial monitor for testing 
  if (currentMillis - previousPrintMillis >= printInterval) {

      previousPrintMillis = currentMillis;

      Serial.print("\nWater Level Percentage : ");
      Serial.println(percentage);

      Serial.print("Soil Moisture : ");
      Serial.println(moistPercentage);

      Serial.print("Temp : ");
      Serial.println(lastTemp);

      Serial.print("Humidity : ");
      Serial.println(lastHumidity);

      Serial.print("Light Value : ");
      Serial.println(lightValue);
  }
}
