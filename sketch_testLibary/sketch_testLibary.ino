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
#include <EEMSDataSend.h>
const int sleepTimeS = 60;

EEMSDataSend eemsDataStream1("fingerprint", "ssid", "password", "streamKey", "Bitpool2 authorizationToken");
void setup() {
  // put your setup code here, to run once;
  eemsDataStream1.sendData(5);
  eemsDataStream1.sendData(6);
  eemsDataStream1.sendData(7);

  Serial.println("Back to sleep");
  ESP.deepSleep(sleepTimeS * 1000000);

}

void loop(){}
