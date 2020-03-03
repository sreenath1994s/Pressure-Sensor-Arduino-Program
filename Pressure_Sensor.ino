//Header Files
#include <SPI.h>
#include <MS5803.h>
#include <WiFi.h>
#include <WebServer.h>
#include <NTPClient.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <FS.h>
#include <SD.h>
#include "senconsole.h"
#include "index.h"
#include "SPIFFS.h" 

// Chip Select pin for devices
#define SENSOR_CS_PIN1 26
#define SENSOR_CS_PIN2 25 
#define SENSOR_CS_PIN3 33 
#define SENSOR_CS_PIN4 32
#define SD_CS 15
#define NTP_ADDRESS  "pool.ntp.org"
#define LED_BUILTIN 2

// HSPI Class for SD Card
SPIClass spiSD(HSPI);

// Sensor Defenitions
MS5803 sensor1 = MS5803(SENSOR_CS_PIN1); //Red
MS5803 sensor2 = MS5803(SENSOR_CS_PIN2); //Green
MS5803 sensor3 = MS5803(SENSOR_CS_PIN3); //Yellow 
MS5803 sensor4 = MS5803(SENSOR_CS_PIN4); //Blue

TaskHandle_t Task1;

// Global Variables
float offset1 = 0;
float offset2 = 0;
float offset3 = 0;
float offset4 = 0;
float temperature1 = 0;
float temperature2 = 0;
float temperature3 = 0;
float temperature4 = 0;
float pressure1 = 0;
float pressure2 = 0;
float pressure3 = 0;
float pressure4 = 0;
String dataMessage;
int i = 0, j=0;
int senstat = 0;
const int output22 = 22; //white led
const int output21 = 21; //red led
unsigned long starttime=0, runtime=0, a1=0, a2=0, a3=0;
const int ch_2_pin = 4;
int rcsignal = 0;

//Wifi Details
const char* ssid = "ESP32";  // Enter SSID here
const char* password = "12345678";  //Enter Password here
WebServer server1(80);
WebServer server2(81);

// Initial Setup
void setup() {
  
  // Start the serial ports.
  Serial.begin( 115200 );
  delay(3000);

  if(!SPIFFS.begin(true)){
     Serial.println("An Error has occurred while mounting SPIFFS");
     return;
  }
  //Initalise wifi and server
  
  WiFi.mode(WIFI_AP_STA); //For AP and Station
  //access point part
  Serial.println("Creating Accesspoint");
  WiFi.softAP(ssid,password);
  Serial.print("IP address:\t");
  Serial.println(WiFi.softAPIP());
  //station part
  Serial.print("connecting to...");
  Serial.println("sreenath");

  WiFi.begin("sreenath","saravana");
  int wifitime = millis();
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
    if((millis()-wifitime)>5000){goto next;}
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
  //WiFi.softAP(ssid, password); //for AP Alone

  //server handles
  next: server1.on("/", handle_OnConnect);
  server1.on("/sensoron", handle_sensoron);
  server1.on("/sensoroff", handle_sensoroff);
  server1.on("/reset", handle_reset);
  server1.on("/download", downloaddata);
  server1.on("/delete", deletedata);
  server2.on("/Chart.min.js", handle_js);
  server2.on("/graph", handle_graph);
  server2.on("/data", handle_data);
  server1.onNotFound(handle_NotFound);
  server1.begin();
  server2.begin();
  Serial.println("HTTP server started");

  // Initialize NTP Server time check.
  WiFiUDP ntpUDP;
  NTPClient timeClient(ntpUDP, NTP_ADDRESS, 32400);

  // Initialize SD Card 
  spiSD.begin(14,27,13,15); //SCK,MISO,MOSI,CS//
  CheckSD();
  
  // Create a file on the SD card and write the data labels
  char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
  String formattedtime;
  String formattedday;
  timeClient.begin();
  timeClient.update();
  formattedday = daysOfTheWeek[timeClient.getDay()];
  formattedtime = timeClient.getFormattedTime();
  File file = SD.open("/data.txt");
  if(!file) {
    Serial.println("File doens't exist");
    Serial.println("Creating file...");
    String Message = "Experiment conducted on " + formattedday + " at time: " + formattedtime + "\r\n" + "Signal,Time,TemperatureR,PressureR,TemperatureG,PressureG,TemperatureY,PressureY,TemperatureB,PressureB, \r\n";
    writeFile(SD, "/data.txt", Message.c_str());
    Serial.println(Message);
  }
  else {
    Serial.println("File already exists");  
  }
  file.close();

  // Initalize the sensor which resets the sensor, downloads the needed coeffecients, 
  // and does a CRC check on the returned data. This will verify that we are talking to the device and that all is well.
  
  if ( sensor1.initalizeSensor() ) {
    Serial.println( "Sensor1 (Red) CRC check OK." );
  } 
  else {
    Serial.println( "Sensor1 (Red) CRC check FAILED! There is something wrong!" );
  }
  if ( sensor2.initalizeSensor() ) {
    Serial.println( "Sensor2 (Green) CRC check OK." );
  } 
  else {
    Serial.println( "Sensor2 (Green) CRC check FAILED! There is something wrong!" );
  }
   if ( sensor3.initalizeSensor() ) {
    Serial.println( "Sensor3 (Yellow) CRC check OK." );
  } 
  else {
    Serial.println( "Sensor3 (Yellow) CRC check FAILED! There is something wrong!" );
  }
   if ( sensor4.initalizeSensor() ) {
    Serial.println( "Sensor4 (Blue) CRC check OK." );
  } 
  else {
    Serial.println( "Sensor4 (Blue) CRC check FAILED! There is something wrong!" );
  }

  //Intialize indicator LED Pins
  pinMode(output21, OUTPUT);
  pinMode(output22, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(output21, LOW);
  digitalWrite(output22, LOW);
  digitalWrite(LED_BUILTIN, LOW);

 xTaskCreatePinnedToCore(
                    Task1code,   /* Task function. */
                    "Task1",     /* name of task. */
                    10000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    1,           /* priority of the task */
                    &Task1,      /* Task handle to keep track of created task */
                    0);          /* pin task to core 0 */                  
  delay(500);
  

}

void Task1code( void * pvParameters ){
  Serial.print("Task1 running on core ");
  Serial.println(xPortGetCoreID());

  for(;;){
   int ch_2 =  pulseIn(ch_2_pin, HIGH, 20000);
   if(ch_2>1000){
    rcsignal = 1;
   }
   else {
   rcsignal = 0;
   }
   delay(2);
  } 
}

//Primary Program loop
void loop() {
  
  //calculate the offset of the sensors once in the loop
  if (i<1) {
    for (int a=0; a<50; a++) {
    sensor1.readSensor();
    offset1 += sensor1.pressure();
    sensor2.readSensor();
    offset2 += sensor2.pressure();
    sensor3.readSensor();
    offset3 += sensor3.pressure();
    sensor4.readSensor();
    offset4 += sensor4.pressure();
    }
    offset1 = offset1/50;
    offset2 = offset2/50;
    offset3 = offset3/50;
    offset4 = offset4/50;
    Serial.println("Offsets are = ");
    Serial.println(offset1);
    Serial.println(offset2);
    Serial.println(offset3);
    Serial.println(offset4);
    i = 1;    
  }
  digitalWrite(LED_BUILTIN, HIGH);
  server1.handleClient();
  server2.handleClient();
  
  if (senstat==1) {
  //Blink Led
  if(j<1){
  starttime = millis();
  j=1;}
  
  //Read Sensor Temperature and Pressure

  a1 = millis()-starttime;
  sensor1.readSensor();
  Serial.print("Pressure = ");
  temperature1 = sensor1.temperature();
  pressure1 = sensor1.pressure() - offset1;
  Serial.print(pressure1);
  
  Serial.print(",");
  sensor2.readSensor();
  temperature2 = sensor2.temperature();
  pressure2 = sensor2.pressure() - offset2;
  Serial.print(pressure2);

  digitalWrite(output22, HIGH); //sensor start is indicated by white led high and red led low 
  digitalWrite(output21, LOW);  //the position of the light is kept at the middle for the average time

  Serial.print(",");
  sensor3.readSensor();
  temperature3 = sensor3.temperature();
  pressure3 = sensor3.pressure() - offset3;
  Serial.print(pressure3);

  Serial.print(",");
  sensor4.readSensor();
  temperature4 = sensor4.temperature();
  pressure4 = sensor4.pressure() - offset4;
  Serial.println(pressure4);
  a2 = millis()-starttime;
  
  digitalWrite(output22, LOW); //at finish read sensor red is 
  digitalWrite(output21, HIGH);
  
  runtime = (a1+a2)/2;
  if(rcsignal==1){
    Serial.println("Autopilot is on");
  }
  else {
    Serial.println("Autopilot is off");
  }
  //log data to SD Card
  logSDCard();

  //Blink Led
  digitalWrite(LED_BUILTIN, LOW);
  
  //Delay is just to make it easier to read. 
  //delay(100); 
  }
}


//Custom Functions for Server Handles
void handle_OnConnect() {
  senstat = 0;
  Serial.println("Sensor Status: OFF");
  server1.send(200, "text/html", SendHTML(senstat)); 
}

void handle_sensoron() {
  senstat = 1;
  Serial.println("Sensor Status: ON");
  server1.send(200, "text/html", SendHTML(senstat));  
}

void handle_sensoroff() {
  senstat = 0;
  Serial.println("Sensor Status: OFF");
  server1.send(200, "text/html", SendHTML(senstat)); 
}

void handle_reset() {
  Serial.println("Restarting ESP");
  server1.send(200, "text/html", SendHTML(2)); 
  deleteFile(SD, "/data.txt");
  ESP.restart();
}

void handle_graph() {
 String s = MAIN_page; //Read HTML contents
 server2.send(200, "text/html", s); //Send web page
}

void handle_js() {
 File file = SPIFFS.open("/Chart.min.js", "r");
 size_t sent = server2.streamFile(file, "text/javascript");
 file.close(); 
}

void handle_data() {
 String data = "{\"Pressure1\":\""+String(pressure1)+"\", \"Pressure2\":\""+ String(pressure2) +"\", \"Pressure3\":\""+ String(pressure3) +"\", \"Pressure4\":\""+ String(pressure4) +"\"}";
 server2.send(200, "text/plane", data); //Send pressure readings, JSON to client ajax request
}

void handle_NotFound(){
  server1.send(404, "text/plain", "Not found");
}

void downloaddata(){
  senstat = 0;
  File download = SD.open("/data.txt");
  if(download){
    server1.sendHeader("Content-Type", "text/text");
    server1.sendHeader("Content-Disposition", "attachment; filename=data.txt");
    server1.sendHeader("Connection", "close");
    server1.streamFile(download, "application/octet-stream");
    download.close();
    }
  else
  { server1.send(404, "text/plain", "The File is not found or the SD Card System is not mounted");
    Serial.println("Data File doens't exist");}
}

void deletedata(){
  server1.send(404, "text/plain", "Deleting the existing data file in the SD Card");
  deleteFile(SD, "/data.txt");
}
/*
//Custom function for serving sensor control webpage
//Check senconsole.h header
*/


//Custom Functions for SD Card
void logSDCard() {

dataMessage = String(rcsignal) + "," + String(runtime) + "," + String(temperature1) + "," + String(pressure1) + "," + String(temperature2) + "," + String(pressure2) + "," + String(temperature3) + "," + String(pressure3) + "," + String(temperature4) + "," + String(pressure4)+ "\r\n";
  Serial.print("Save data: ");
  //Serial.println(dataMessage);
  appendFile(SD, "/data.txt", dataMessage.c_str());
}


void CheckSD() {
  if (!SD.begin(15, spiSD)) {
    Serial.println("Card Mount Failed");
    return;
  }
  uint8_t cardType = SD.cardType();

  if (cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    return;
  }

  Serial.print("SD Card Type: ");
  if (cardType == CARD_MMC) {
    Serial.println("MMC");
  } else if (cardType == CARD_SD) {
    Serial.println("SDSC");
  } else if (cardType == CARD_SDHC) {
    Serial.println("SDHC");
  } else {
    Serial.println("UNKNOWN");
  }

  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);

}


void writeFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Writing file: %s\n", path);

  File file = fs.open(path, FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  if (file.print(message)) {
    Serial.println("File written");
  } else {
    Serial.println("Write failed");
  }
}

void appendFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Appending to file: %s\n", path);

  File file = fs.open(path, FILE_APPEND);
  if (!file) {
    Serial.println("Failed to open file for appending");
    return;
  }
  if (file.print(message)) {
    Serial.println("Message appended");
  } else {
    Serial.println("Append failed");
  }
}

void deleteFile(fs::FS &fs, const char * path) {
  Serial.printf("Deleting file: %s\n", path);
  if (fs.remove(path)) {
    Serial.println("File deleted");
  } else {
    Serial.println("Delete failed");
  }
}
