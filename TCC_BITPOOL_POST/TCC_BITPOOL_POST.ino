/*
 * HTTPS Secured Client POST Request
 * Copyright (c) 2019, circuits4you.com
 * All rights reserved.
 * https://circuits4you.com 
 * Connects to WiFi HotSpot. */

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h> 
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <TimeLib.h>
#include <DHT.h>

/* Definitions */ 
#define DHTPIN D4 
#define DHTTYPE DHT11
const int sleepTimeS = 60;
/* Set these to your desired credentials. */
const char *ssid = "XXXXX";  //ENTER YOUR WIFI SETTINGS
const char *password = "XXXXX";

//Web/Server address to read/write from 
const char *host = "api.bitpool.com";
const int httpsPort = 443;  //HTTPS= 443 and HTTP = 80

//SHA1 finger print of certificate use web browser to view and copy
const char fingerprint[] PROGMEM = "9d 81 3e 65 bb c4 26 9c 1e d5 f2 1f 8b c4 47 cd 38 6f 19 ae";
String streamKey = "XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX";

//API Key E.G. 
String authorizationToken = "Bitpool2 XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX";

DHT dht(DHTPIN, DHTTYPE);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "au.pool.ntp.org", -7200);
String Link = "/public/v2/streams/" + streamKey + "/logs";


//=======================================================================
//                    setup
//=======================================================================

void setup() {
  delay(1000);
  Serial.begin(115200);
  WiFi.mode(WIFI_OFF);        //Prevents reconnection issue (taking too long to connect)
  delay(1000);
  WiFi.mode(WIFI_STA);        //Only Station No AP, This line hides the viewing of ESP as wifi hotspot
  
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
  timeClient.begin();
  dht.begin(); 



//=======================================================================
//                    Main Program Loop
//=======================================================================

  WiFiClientSecure httpsClient;    //Declare object of class WiFiClient

  Serial.println(host);
  timeClient.update();
  Serial.println(timeClient.getEpochTime());
  char dateTimeBuffer[24];
  sprintf(dateTimeBuffer, "%4d-%02d-%02dT%02d:%02d:%02d.000Z", year(timeClient.getEpochTime()), month(timeClient.getEpochTime()), day(timeClient.getEpochTime()),hour(timeClient.getEpochTime()), minute(timeClient.getEpochTime()), second(timeClient.getEpochTime()));
  Serial.println(dateTimeBuffer);

  
  Serial.printf("Using fingerprint '%s'\n", fingerprint);
  httpsClient.setFingerprint(fingerprint);
  httpsClient.setTimeout(15000); // 15 Seconds
  delay(1000);
  
  Serial.print("HTTPS Connecting");
  int r=0; //retry counter
  while((!httpsClient.connect(host, httpsPort)) && (r < 30)){
      delay(100);
      Serial.print(".");
      r++;
  }
  if(r==30) {
    Serial.println("Connection failed");
  }
  else {
    Serial.println("Connected to web");
  }
  

  Serial.print("requesting URL: ");
  Serial.println(host);

  float t = dht.readTemperature();
  String payloadData = "[{\"Ts\": \"" + String(dateTimeBuffer) + "\",\"Val\": " + String(t) + ",\"ValStr\": \"string\",\"Calculated\": true}]";
  Serial.println(payloadData);
  httpsClient.print(String("POST ") + Link + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Authorization: "+ authorizationToken + "\r\n" +
               "Content-Type: application/json"+ "\r\n" +
               "Content-Length: " + payloadData.length() + "\r\n" +
               "Connection: close" +" \r\n\r\n" +
               payloadData + "\r\n");

  Serial.println("request sent");
                  
  while (httpsClient.connected()) {
    String line = httpsClient.readStringUntil('\n');
    if (line == "\r") {
      Serial.println("headers received");
      break;
    }
  }

  Serial.println("reply was:");
  Serial.println("==========");
  String line;
  while(httpsClient.available()){        
    line = httpsClient.readStringUntil('\n');  //Read Line by Line
    Serial.println(line); //Print response
  }
  Serial.println("==========");
  Serial.println("closing connection");
  Serial.println("Back to sleep");
  ESP.deepSleep(sleepTimeS * 1000000);
}

void loop() {}
//=======================================================================
