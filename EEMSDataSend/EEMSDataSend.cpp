/*
  EEMSDataSend.h - Library for sending live sensor data to Bitpool streams.
  
  EEMSDataSend constructor parameters:
  fingerprint - the ssl certificate Thumbprint of the api.bitpool.com site (with spaces evey 2 digits). (may need to be updated when it expires yearly)
  ssid - the ssid of the wifi network
  password- the password of the wifi network
  streamKey - the stream key of the bitpool stream to send the data to
  authorizationToken - the api key used to access bitpool
  
  sendData Parameters:
  sensorValue - the value to send to the bitpool stream
*/

#include "Arduino.h"
#include "EEMSDataSend.h"

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h> 
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <TimeLib.h>

EEMSDataSend::EEMSDataSend(char* fingerprint, char* ssid, char* password, String streamKey, String authorizationToken)
{
 	_ssid = ssid;
	_password = password;
	_host = "api.bitpool.com";
	_httpsPort = 443;
	_fingerprint = fingerprint;
	_streamKey = streamKey;
	_authorizationToken = authorizationToken;
}

void EEMSDataSend::sendData(float sensorValue)
{
  _sensorValue = sensorValue;
  WiFiUDP ntpUDP;
  NTPClient timeClient(ntpUDP, "au.pool.ntp.org", -7200);
  String Link = "/public/v2/streams/" + _streamKey + "/logs";

  delay(1000);
  Serial.begin(115200);
  WiFi.mode(WIFI_OFF);        //Prevents reconnection issue (taking too long to connect)
  delay(1000);
  WiFi.mode(WIFI_STA);        //Only Station No AP, This line hides the viewing of ESP as wifi hotspot
  
  WiFi.begin(_ssid, _password);     //Connect to your WiFi router

  Serial.print("Connecting");
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  //If connection successful show IP address in serial monitor
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(_ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //IP address assigned to your ESP
  timeClient.begin();

  WiFiClientSecure httpsClient;    //Declare object of class WiFiClient

  Serial.println(_host);
  timeClient.update();
  Serial.println(timeClient.getEpochTime());
  char dateTimeBuffer[24];
  int epochTime = timeClient.getEpochTime();
  sprintf(dateTimeBuffer, "%4d-%02d-%02dT%02d:%02d:%02d.000Z", year(epochTime), month(epochTime), day(epochTime),hour(epochTime), minute(epochTime), second(epochTime));
  Serial.println(dateTimeBuffer);

  
  Serial.printf("Using fingerprint '%s'\n", _fingerprint);
  httpsClient.setFingerprint(_fingerprint);
  httpsClient.setTimeout(15000); // 15 Seconds
  delay(1000);
  
  Serial.print("HTTPS Connecting");
  int r=0; //retry counter
  while((!httpsClient.connect(_host, _httpsPort)) && (r < 30)){
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
  Serial.println(_host);

  String payloadData = "[{\"Ts\": \"" + String(dateTimeBuffer) + "\",\"Val\": " + String(_sensorValue) + ",\"ValStr\": \"string\",\"Calculated\": true}]";
  Serial.println(payloadData);
  httpsClient.print(String("POST ") + Link + " HTTP/1.1\r\n" +
               "Host: " + _host + "\r\n" +
               "Authorization: "+ _authorizationToken + "\r\n" +
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

}