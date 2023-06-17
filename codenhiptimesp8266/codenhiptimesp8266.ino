#include <Wire.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include "Adafruit_GFX.h"
#include "OakOLED.h"
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include "MAX30100_PulseOximeter.h"
#define REPORTING_PERIOD_MS 2000
PulseOximeter pox;
OakOLED oled;
int bpm, SpO2;
uint32_t tsLastReport = 0;
/* Set these to your desired credentials. */
const char *ssid = "Gia Dinh Nho";  //ENTER YOUR WIFI SETTINGS
const char *password = "Thaikimdong@#2021#@.";

void setup() {
  delay(1000);
  Serial.begin(115200);
  WiFi.mode(WIFI_OFF);        //Prevents reconnection issue (taking too long to connect)
  delay(1000);
  WiFi.mode(WIFI_STA);        //This line hides the viewing of ESP as wifi hotspot
  
  WiFi.begin(ssid, password);     //Connect to your WiFi router
  Serial.println("");

  Serial.print("Connecting");
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  //If connection successful show IP address in serial monitor
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //IP address assigned to your ESP
  oled.begin();
  oled.clearDisplay();
  oled.setTextSize(1);
  oled.setTextColor(1);
  if (pox.begin()){ 
  pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);
  }
}

void loop() {
  pox.update();
  if (millis() - tsLastReport > REPORTING_PERIOD_MS){
    bpm = pox.getHeartRate();
    SpO2 = constrain(pox.getSpO2(), 0, 100);
    oled.clearDisplay();
    oled.setCursor(0,0);
    oled.print("Heart Rate:");
    oled.setCursor(0,10);
    oled.print(bpm);
    oled.print(" bpm");

    oled.setCursor(74,0);
    oled.print("SpO2:");
    oled.setCursor(74,10);
    oled.print(SpO2);
    oled.print(" %");
    oled.display();
    tsLastReport = millis();
    HTTPClient http;
    if (SpO2 != 0 && bpm != 0){
      String postData, NHIPTIM, SPO2;
      NHIPTIM = String(bpm);
      SPO2 = String(SpO2); 
      //Post Data
      postData = "SpO2=" + SPO2 + "&bpm=" + NHIPTIM;
      Serial.print(postData);
      http.begin("http://192.168.1.107:81/nhiptim/postnhiptim.php");              //Specify request destination
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");    //Specify content-type header

      int httpCode = http.POST(postData);   //Send the request
      String payload = http.getString();    //Get the response payload

      Serial.println(httpCode);   //Print HTTP return code
      Serial.println(payload);    //Print request response payload
      SpO2 = 0;
      bpm = 0;
      http.end();  //Close connections
      delay(3000);
    }      
  }
}
 

