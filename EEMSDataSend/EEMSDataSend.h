/*
  EEMSDataSend.h - Library for sending live sensor data to Bitpool streams.
  
  EEMSDataSend constructor parameters:
  fingerprint - the ssl certificate Thumbprint of the api.bitpool.com site. (may need to be updated when it expires yearly)
  ssid - the ssid of the wifi network
  password- the password of the wifi network
  streamKey - the stream key of the bitpool stream to send the data to
  authorizationToken - the api key used to access bitpool
  
  sendData Parameters:
  sensorValue - the value to send to the bitpool stream
*/
#ifndef EEMSDataSend_h
#define EEMSDataSend_h

#include "Arduino.h"


class EEMSDataSend
{
  public:
    EEMSDataSend(char* fingerprint, char* ssid, char* password, String streamKey, String authorizationToken);
    void sendData(float sensorValue);
    void dash();
  private:
	char* _ssid;
	char* _password;
	char* _host;
	int _httpsPort;
	char* _fingerprint;
	String _streamKey;
	String _authorizationToken;
	float _sensorValue;
	
};

#endif